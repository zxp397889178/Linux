#include <iostream>
#include <sstream>

#include <time.h>
#include <string.h>
#include <algorithm>

#include "HttpClient.h"

namespace http {

static bool g_bCurlInit =  false;

void CHttpClient::GlobalInit(void)
{
    if (!g_bCurlInit)
    {
        LogImpt("init curl global env");
        curl_global_init(CURL_GLOBAL_ALL);
        g_bCurlInit = true;
    }
}


void CHttpClient::GlobalFini(void)
{
    if (g_bCurlInit)
    {
        g_bCurlInit = false;
        curl_global_cleanup();
    }
}                                         

#if 1
struct MemoryStruct
{                                                                     
    char *memory;
    size_t size;
};
#else
struct MemoryStruct
{
    char *memory;
    size_t capacity;    // 总容量
    size_t size;        // 可用容量
};
#endif


// HTTP 返回数据回调
static size_t OnWriteData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t realSize = size * nmemb;
    //LogWarn("recv resp data %lu bytes.", realSize);
    struct MemoryStruct *mem = (struct MemoryStruct *)stream;
    mem->memory = (char*)realloc(mem->memory, mem->size+realSize+1);
    if (mem->memory == NULL)
    {
        LogErr("not enough memory OnWriteData(realloc return NULL), current size [%d], new size [%d]",
                mem->size, mem->size+realSize+1);
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), ptr, realSize);
    mem->size += realSize;
    mem->memory[mem->size]=0;
    
    return realSize;
}

// HTTP 返回头回调
static size_t OnRespHeader(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t realSize = size*nmemb;
    //LogWarn("recv resp header %lu bytes.", realSize);
    struct MemoryStruct *mem = (struct MemoryStruct *)stream;
    mem->memory = (char*)realloc(mem->memory, mem->size+realSize+1);
    if (mem->memory == NULL)
    {
        LogErr("not enough memory OnRespHeader (realloc return NULL), current size[%d], new size [%d]",
                mem->size, mem->size+realSize+1);
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), ptr, realSize);
    mem->size += realSize;
    mem->memory[mem->size] = 0;

    return realSize;
}

CHttpClient::CHttpClient()
	: m_nHttpFailureTimes(0)
	, m_nHttpReInitTimes(0)
	, m_nHttpTimeout(0)
{
	msErrMsg = "";
	this->Init();
}


CHttpClient::~CHttpClient()
{
}


void CHttpClient::Init(int32_t nDealTime, int32_t nHttpTimeout)
{
	m_nHttpReInitTimes = nDealTime;
	m_nHttpTimeout = nHttpTimeout;

}

void CHttpClient::ReInitCurl()
{
	
	m_nHttpFailureTimes = 0;
	this->Init(m_nHttpReInitTimes, m_nHttpTimeout);
}

bool CHttpClient::getError(int32_t err, CMyString &strError)
{
    if (err <=0)
    {
        return false;
    }
    strError = curl_easy_strerror((CURLcode)err);
    if (strError.empty())
    {
        return false;
    }
    return true;
}

CURL* CHttpClient::CreateHttp(const char* url, void *pHeader, void *pChunk)
{
    msErrMsg = "";
	CURLcode res = CURLE_OK;
	CURL* pCurl = NULL;

	if (m_nHttpFailureTimes >= m_nHttpReInitTimes)
	{
		this->ReInitCurl();
	}

	pCurl = curl_easy_init();
	if (pCurl == NULL){
		LogErr("http create error, reason: curl_easy_init error");
		return pCurl;
    }

	curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1);
	do 
	{
		res = curl_easy_setopt(pCurl, CURLOPT_URL, url);
		if (res != CURLE_OK){
			msErrMsg = msErrMsg + "curl_easy_setopt CURLOPT_URL error :" + curl_easy_strerror(res);
			break;
		}

		curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, m_nHttpTimeout);
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, m_nHttpTimeout);
		curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, false);

		curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, NULL);

        // 设置回写的数据
		res = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, OnWriteData);
		if (res != CURLE_OK){
			msErrMsg = msErrMsg + "curl_easy_setopt CURLOPT_WRITEFUNCTION error :" + curl_easy_strerror(res);
			break;
		}
		res = curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pChunk);
		if (res != CURLE_OK){
			msErrMsg = msErrMsg + "curl_easy_setopt CURLOPT_WRITEDATA error :" + curl_easy_strerror(res);
			break;
		}

        // 设置响应的头
        res = curl_easy_setopt(pCurl, CURLOPT_HEADERFUNCTION, OnRespHeader);
        if (res != CURLE_OK)
        {
			msErrMsg = msErrMsg + "curl_easy_setopt CURLOPT_HEADERFUNCTION error :" + curl_easy_strerror(res);
            break;
        }
        res = curl_easy_setopt(pCurl, CURLOPT_HEADERDATA, pHeader);
        if (res != CURLE_OK)
        {
			msErrMsg = msErrMsg + "curl_easy_setopt CURLOPT_HEADERDATA error :" + curl_easy_strerror(res);
            break;
        }

	} while (0);
	
	if (res != CURLE_OK)
	{
		LogErr("http create error, reason: %s", msErrMsg.c_str());
		curl_easy_cleanup(pCurl);
		pCurl = NULL;
	}
	return pCurl;
}


void CHttpClient::SetPost(CURL*pCurl, bool bPost, const char* httpBody)
{
	
	if (bPost && NULL != httpBody && NULL != pCurl)
	{
		curl_easy_setopt(pCurl, CURLOPT_POST, 1);
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, httpBody);
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strlen(httpBody));
	}
}


struct curl_slist * CHttpClient::SetHeader(CURL*pCurl, VEC_STRING& header_list)
{
	
	if (NULL == pCurl)
	{
		return NULL;
	}

	struct curl_slist * pHeaderList = NULL;

	if (header_list.size() > 0)
	{
		for (size_t i = 0; i < header_list.size(); i++)
		{
			pHeaderList = curl_slist_append(pHeaderList, header_list[i].c_str());
		}
		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaderList);
	}

	return pHeaderList;
}

void CHttpClient::SetCookie(CURL*pCurl, const char* cookie)
{
	
	if (NULL == pCurl || NULL==cookie || '\0'==cookie[0])
	{
		return;
	}
	curl_easy_setopt(pCurl, CURLOPT_COOKIE, cookie);
}

int32_t CHttpClient::HttpRequest(CURL*pCurl, VEC_STRING &vecHeader, 
        void *pHeader, void *pChunk, 
        std::string &strHeader, std::string &strResp)
{
	//uint64_t t1 = Now(true);

	if (NULL == pCurl)
	{
		LogErr("http request error, reason: curl not init");
		return CURLE_FAILED_INIT;
	}

	struct curl_slist * pHeaderList = NULL;
    if (!vecHeader.empty())
	{
		pHeaderList = this->SetHeader(pCurl, vecHeader);
	}

	int32_t respCode = 0;
	CURLcode res = CURLE_OK;
	//uint64_t t2 = Now(true);
    //uint64_t t3(0), t4(0);
	do 
	{
		res = curl_easy_perform(pCurl);
		if (res != CURLE_OK){
			msErrMsg = msErrMsg + "curl_easy_perform error :" + curl_easy_strerror(res);
			break;
		}
        //t3 = Now(true);
		res = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &respCode);
		if (res != CURLE_OK){
			msErrMsg = msErrMsg + "curl_easy_getinfo CURLINFO_RESPONSE_CODE error :" + curl_easy_strerror(res);
			break;
		}
        //t4 = Now(true);
	} while (0);

    struct MemoryStruct *pMem = NULL;
    pMem = (struct MemoryStruct *)pChunk;
    char *pStr = (char*)&pMem->memory[0];
    strResp.append(pStr, pMem->size);
    //uint64_t t5 = Now(true);
    
    struct MemoryStruct *pMemH = NULL;
    pMemH = (struct MemoryStruct *)pHeader;
    char *pStrH = (char*)&pMemH->memory[0];
    strHeader.append(pStrH, pMemH->size);
    //uint64_t t6 = Now(true);
    
	if (NULL != pHeaderList)
	{
		curl_slist_free_all(pHeaderList);
		pHeaderList = NULL;
	}

	curl_easy_cleanup(pCurl);

    free(pMem->memory);
    free(pMemH->memory);

	if (CURLE_OK != res && 200 != respCode)
	{
		if (200 != respCode)
		{
			m_nHttpFailureTimes++;
		}
		LogErr("http request error, reason: %s", msErrMsg.c_str());
	}
    //uint64_t t7 = Now(true);

    //LogWarn("http execute cost: before:%ldus, perform:%ldus, getinfo:%ldus, resp:%ldus, header:%ldus, clean:%ldus",
    //        (t2-t1), (t3-t2), (t4-t3), (t5-t4), (t6-t5), (t7-t6));
	pCurl = NULL;
	return res;
}

int CHttpClient::HttpPost(const char* url, 
        std::string& respMsg, std::string &respHeader, 
        const char* httpBody)
{
    VEC_STRING header_list;
    return HttpPostWithHeaderCookie(url, respMsg, respHeader,
            header_list, NULL, httpBody);
}

int CHttpClient::HttpPostWithCookie(const char* url, 
        std::string &respMsg, std::string &respHeader,
        const char * cookie, const char* httpBody)
{
    VEC_STRING header_list;
    return HttpPostWithHeaderCookie(url, respMsg, respHeader,
            header_list, cookie, httpBody);
}

int CHttpClient::HttpPostWithHeaders(const char* url, 
        std::string& respMsg, std::string &respHeader,
        VEC_STRING& header_list, const char* httpBody)
{
    return HttpPostWithHeaderCookie(url, respMsg, respHeader, 
            header_list, NULL, httpBody);
}

int CHttpClient::HttpPostWithHeaderCookie(const char* url, 
        std::string& respMsg, std::string &respHeader,
        VEC_STRING& header_list, const char *cookie, const char* httpBody)
{
    //uint64_t t1 = Now(true);

	int32_t res = 0;
    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1); 
    chunk.size = 0; 
    void *pChunk = (void*)&chunk;

    struct MemoryStruct header;
    header.memory = (char*)malloc(1);
    header.size = 0;
    void *pHeader = (void*)&header;

    //uint64_t t2 = Now(true);
	CURL* pCurl = this->CreateHttp(url, pHeader, pChunk);
	if (NULL == pCurl)
	{
		
		m_nHttpFailureTimes++;
		return -1000;
	}
    //uint64_t t3 = Now(true);

    if(NULL != cookie && cookie[0] !='\0')
    {
	    this->SetCookie(pCurl, cookie);
    }

	this->SetPost(pCurl, true, httpBody);

    //uint64_t t4 = Now(true);
	res = this->HttpRequest(pCurl, header_list, 
            pHeader, pChunk, respHeader, respMsg);
    // TODO: 此函数调用居然会发生 signal 11的错误，需要好好检查下

    //uint64_t t5 = Now(true);

    //LogWarn("http post time, total[%ldus] [before:%ldus, create http:%ldus, set value:%ldus, post:%ldus]",
    //        (t5-t1), (t2-t1), (t3-t2), (t4-t3), (t5-t4) );
	return (int)res;
}

int CHttpClient::HttpGet(const char* url, std::string &respMsg, std::string &respHeader)
{
    VEC_STRING header_list;
    return this->HttpGetWithHeaderCookie(url, 
            respMsg, respHeader, header_list, NULL);
}

int CHttpClient::HttpGetWithHeaders(const char* url, 
        std::string &respMsg, std::string &respHeader, 
        VEC_STRING &header_list)
{
    return this->HttpGetWithHeaderCookie(url, 
            respMsg, respHeader, header_list, NULL);
}

int CHttpClient::HttpGetWithCookie(const char* url, 
        std::string &respMsg, std::string &respHeader, 
        const char* cookie)
{    
    VEC_STRING header_list;
    return this->HttpGetWithHeaderCookie(url, 
            respMsg, respHeader, header_list, cookie);
}

int CHttpClient::HttpGetWithHeaderCookie(const char* url, 
        std::string &respMsg, std::string &respHeader, 
        VEC_STRING &header_list, const char* cookie)
{    
	int32_t res = 0;

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1); 
    chunk.size = 0; 
    void *pChunk = (void*)&chunk;

    struct MemoryStruct header;
    header.memory = (char*)malloc(1);
    header.size = 0;
    void *pHeader = (void*)&header;

	CURL* pCurl = this->CreateHttp(url, pHeader, pChunk);
	if (NULL == pCurl)
	{
		m_nHttpFailureTimes++;
		return -1000;
	}

    if (NULL != cookie && cookie[0]!='\0')
    {
	    this->SetCookie(pCurl, cookie);
    }

	res = this->HttpRequest(pCurl, header_list, 
            pHeader, pChunk, respHeader, respMsg);
	return (int)res;	
}

} 


