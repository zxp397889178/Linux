#ifndef  __IDENTIFYCODE_HTTP_CLIENT__
#define  __IDENTIFYCODE_HTTP_CLIENT__
#include <string>
#include <iostream>
#include <stdint.h>
#include "BaseCode.h"

#include <curl/curl.h>

namespace http {

typedef std::vector<std::string>    VEC_STRING;

class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

    static void GlobalInit(void);
    static void GlobalFini(void);
	
public:
	void Init(int32_t nDealTime = 3, int32_t nHttpTimeout = 10);

    /* 返回值：=0 成功
     *         <0 封装库错误
     *         >0 curl错误， 详见 http://www.ttlsa.com/linux/curl-error-code-list/
     *         其中：
     *         ==> 无法连接上主机
     *              CURLE_FAILED_INIT            (2)
     *              CURLE_URL_MALFORMAT          (3)
     *              CURLE_COULDNT_RESOLVE_PROXY  (5)
     *              CURLE_COULDNT_RESOLVE_HOST   (6)
     *              CURLE_COULDNT_CONNECT        (7)
     *              CURLE_FTP_WEIRD_SERVER_REPLY (8)
     *         ==> 超时
     *              CURLE_OPERATION_TIMEDOUT     (28)
     */
	int HttpPost(const char* url, std::string &respMsg, std::string &respHeader, const char* httpBody="");
    int HttpPostWithHeaders(const char* url, std::string &respMsg, std::string &respHeader, VEC_STRING& header_list, const char* httpBody="");
    int HttpPostWithCookie(const char* url, std::string &respMsg, std::string &respHeader, const char *cookie, const char* httpBody="");
    int HttpPostWithHeaderCookie(const char* url, std::string &respMsg, std::string &respHeader, VEC_STRING& header_list, const char *cookie, const char* httpBody="");

    int HttpGet(const char* url, std::string &respMsg, std::string &respHeader);
    int HttpGetWithHeaders(const char* url, std::string &respMsg, std::string &respHeader, VEC_STRING &header_list);
	int HttpGetWithCookie(const char* url, std::string &respMsg, std::string &respHeader,const char* cookie);
	int HttpGetWithHeaderCookie(const char* url, std::string &respMsg, std::string &respHeader, VEC_STRING &header_list, const char* cookie);

public:
    bool getError(int32_t err, CMyString &strError);

private:
	void ReInitCurl();
    CURL* CreateHttp(const char* url, void *pHeader, void *pChunk);
	void SetPost(CURL*pCurl, bool bPost, const char* httpBody);
	struct curl_slist * SetHeader(CURL*pCurl, VEC_STRING& header_list);
	void SetCookie(CURL*pCurl, const char* cookie);

    int32_t HttpRequest(CURL*pCurl, VEC_STRING &vecHeader, void *pHeader, void *pChunk, std::string &respHeader, std::string &strResp);
private:
	std::string msErrMsg;

	int32_t	m_nHttpFailureTimes;
	int32_t	m_nHttpReInitTimes;
	int32_t	m_nHttpTimeout;
};
} // namespace httpc
#endif //__REST_CLIENT__
