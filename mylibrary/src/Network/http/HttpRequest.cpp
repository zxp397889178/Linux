/*
 * =====================================================================================
 *
 *       Filename:  HttpRequest.cpp
 *
 *    Description:  implement of HttpRequest
 *
 *        Version:  1.0
 *        Created:  05/21/2015 03:21:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include "HttpRequest.h"
#include "Security.h"
#include "json/json.h"
#include <algorithm>

namespace http {

CHttpRequest::CHttpRequest()
	: CHttpData()
{
	m_begin_time = Now();
}

CHttpRequest::~CHttpRequest()
{

}

CHttpRequest &CHttpRequest::operator =(CHttpRequest &obj)
{
	if (this == &obj)
	{
		return *this;
	}

	CHttpData::operator=(obj);
	m_method = obj.getMethod();
	m_url = obj.getUrl();
	m_strIp = obj.getIp();	

	m_params = obj.getParams();
	m_begin_time = obj.getBeginTime();	

	return *this;
}

int32_t CHttpRequest::onHeaderBegin(const CMyString &strLine)
{
	//LogDebug("strParam = [%s]", strLine.c_str());
	
	CMyString::size_type pos = strLine.find_first_of(" \t");
	if (pos == CMyString::npos)
	{
		return -1;
	}
	m_method = strLine.substr(0, pos);
	m_method.trim();
    m_method.MakeUpper();
	
	
	CMyString::size_type end = strLine.find_first_of(" \t", pos+1);
	if (end == CMyString::npos)
	{
		return -2;
	}
	
	CMyString param = strLine.substr(pos+1, end-pos-1);
#if 1
    char c = param[0];
    if (c=='/')
    {
        //LogDebug("cgi check, param:[%s]", param.c_str());
        param = param.substr(1);
    }
    else if (!strncasecmp(param.c_str(), "http://", 7))
    {
        LogTrace("http check, param:[%s]", param.c_str());
        // 对 URL 进行重新解析
        CMyString::size_type pos_tmp = param.find_first_of("/", 8);
        if (pos_tmp == CMyString::npos)
        {
            param.clear();
        }
        else
        {
            param = param.substr(pos_tmp+1);
        }
    }
    else
    {
        LogFatal("unknown cgi [%s]", param.c_str());
        // TODO: 未知的方法，不做解析
    }

    //printf("[param] [%s]", param.c_str());
    pos = param.find("?");
    if (pos == CMyString::npos)
    {
        m_url = param;
    }
    else
    {
        m_url = param.substr(0, pos);
		CMyString url_params = param.substr(pos+1);
		this->parseParam(url_params);
    }
#else
	pos = param.find("?");
	if (pos == CMyString::npos)
	{
		m_url = param.substr(1);
	}
	else
	{
		m_url = param.substr(1, pos-1);
		CMyString url_params = param.substr(pos+1);
		this->parseParam(url_params);
	}
#endif

	m_url.trim();
    LogTrace("[%s:%s:%d]m_url=[%s]",__FILE__, __func__, __LINE__, m_url.c_str());
	
	m_version = strLine.substr(end);
	m_version.trim();

	return 0;
}

void CHttpRequest::parseParam(const CMyString &strLine)
{
    CMyString strUrl;
    CHttpWrapper::urlDecode(strLine, strUrl);
    LogDebug("HTTP-DBG>NEW LINE: [%s]", strUrl.c_str());

	char szText[10240] = "";
	safe_strcpy(szText, sizeof(szText), strUrl.c_str());

	char *end_str;
	char *pc = strtok_r(szText, "&", &end_str);
	if (pc == NULL)
	{
		this->addOneParam(szText);
        return;
	}
	while(pc != NULL)
	{
		
		this->addOneParam(pc);
		pc = strtok_r(NULL, "&", &end_str);
	}
}

int32_t CHttpRequest::addOneParam(const CMyString &strParam)
{
	Node node;
	CMyString::size_type pos = strParam.find("=");
	if (pos == CMyString::npos)
	{
		node.value = strParam;
	}
	else
	{
		CMyString strKey = strParam.substr(0, pos);
		CMyString strVal = strParam.substr(pos+1);
		strKey.trim();
		strVal.trim();
        CMyString strValNew;
        CHttpWrapper::urlDecode(strVal, strValNew);
		node.name = strKey.c_str();
		node.value = strValNew.c_str();
	}
	m_params.push_back(node);
    return (int32_t)strParam.length();
}

const char* CHttpRequest::getParamStr(const CMyString &param)
{
    CMyString strVal = node_get(m_params, strVal);
    return strVal.c_str();
}

int32_t CHttpRequest::getParamInt(const CMyString &param)
{
    CMyString val = this->getParam(param);
    return StrToInt32(val.c_str());
}

int64_t CHttpRequest::getParamInt64(const CMyString &param)
{
    CMyString val = this->getParam(param);
    return StrToInt64(val.c_str());
}

CMyString CHttpRequest::getParam(const CMyString &strParam)
{
	return node_get(m_params, strParam);
}

bool CHttpRequest::getParam(const CMyString &strParam, CMyString &val)
{
    val = node_get(m_params, strParam);
    return (!val.empty());
}

bool CHttpRequest::getParam(const CMyString &strParam, int32_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToInt32(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParam(const CMyString &strParam, int64_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToInt64(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParam(const CMyString &strParam, uint32_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToUint32(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParam(const CMyString &strParam, uint64_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToUint64(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParamAsTime(const CMyString &strParam, time_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToTime(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParamAsDate(const CMyString &strParam, time_t &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToTime(strVal.c_str(), "yyyy-mm-dd");
    }
    return ret;
}


bool CHttpRequest::getParam(const CMyString &strParam, double &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToDouble(strVal.c_str());
    }
    return ret;
}

bool CHttpRequest::getParam(const CMyString &strParam, bool &val)
{
    CMyString strVal;
    bool ret = this->getParam(strParam, strVal);
    if (ret)
    {
        val = StrToBoolean(strVal.c_str());
    }
    return ret;
}

void CHttpRequest::setParam(const CMyString &strParam, const CMyString &strVal)
{
	node_set(m_params, strParam, strVal);
}

void CHttpRequest::setParam(const CMyString &strParam, int32_t val)
{
	CMyString strVal("%d", val);
	this->setParam(strParam, strVal.c_str());
}

void CHttpRequest::setParam(const CMyString &strParam, uint32_t val)
{
	CMyString strVal("%u", val);
	this->setParam(strParam, strVal.c_str());
}

void CHttpRequest::setParam(const CMyString &strParam, int64_t val)
{
	CMyString strVal("%ld", val);
	this->setParam(strParam, strVal.c_str());
}

void CHttpRequest::setParam(const CMyString &strParam, uint64_t val)
{
	CMyString strVal("%lu", val);
	this->setParam(strParam, strVal.c_str());
}

void CHttpRequest::onHeaderEnd(void)
{
	CHttpData::onHeaderEnd();
#if 1
	LogDebug("Header> method = [%s]", m_method.c_str());
	LogDebug("Header> url = [%s]", m_url.c_str());
	LogDebug("Header> version = [%s]", m_version.c_str());
	NodeVector::iterator it(m_params.begin());
	for(; it != m_params.end(); ++it)
	{
		Node &node = *it;
		LogDebug("Params> [%s] = [%s]", node.name.c_str(), node.value.c_str());
	}
#endif
}

void CHttpRequest::clear(void)
{
	CHttpData::clear();

	m_method.clear();
	m_url.clear();
	m_params.clear();
}

void CHttpRequest::pack(CByteStream &data, bool bSetContentLength)
{
	static size_t s_maxLineLength = 1024;
	

	CMyString header("%s /%s", m_method.c_str(), m_url.c_str());
	if (!m_params.empty())
	{
		header.append("?");
		header.append(CHttpData::node_encode(m_params, "=", "&", s_maxLineLength));
	}
	header.Append(" %s\r\n", m_version.c_str());
	data.Write((void*)header.c_str(), header.size());

	CHttpData::pack(data, bSetContentLength);
}

void CHttpRequest::getPost(CByteStream &buff)
{
    buff = GetPost();
}

// 返回值： 0 解析出错
//          1 仅有strContentType
//          2 含有后续的key value
// strContent 类似：multipart/form-data; boundary=--------------------------364087631546366941314994
int32_t decode_content_string(const CMyString &strContent, CMyString &strContentType, CMyString &strKey, CMyString &strValue)
{
    char str1[128] = "";
    char str2[128] = "";
    char str3[128] = "";
    int32_t ret = sscanf(strContent.c_str(),  "%[^;];%[^=]=%[^; ]", str1, str2, str3);
    if (ret<1)
    {
        return -1;
    }
    if (ret==1)
    {
        strContentType = str1;
        strContentType.trim();
        return 1;
    }
    strContentType = str1;
    strKey = str2;
    strValue = str3;
    strContentType.trim();
    strKey.trim();
    strValue.trim();
    return 2;
}

void CHttpRequest::parsePostData(void)
{
    CHttpData::parsePostData();
    CMyString method = this->getMethod();
    if (strcasecmp(method.c_str(), "POST") != 0)
    {
        LogErr("method is not POST");
        return;
    } 
    CMyString strFullType = CHttpData::getHeader("Content-Type");
    if (strFullType.empty())
    {
        LogErr("not found Content-Type");
        return;
    }

    // 解析Content-Type,可能含有charset boundary，暂时忽略两者的处理
    // https://blog.csdn.net/u014209205/article/details/81147783
    // https://baijiahao.baidu.com/s?id=1630488435675563607&wfr=spider&for=pc
    //
    CMyString strContentType;
    CMyString strContentKey;
    CMyString strContentValue;
    int32_t ret = decode_content_string(strFullType, strContentType, strContentKey, strContentValue);
    if (ret <= 0)
    {
        LogErr("content-type string [%s], ret=%d", strFullType.c_str(), ret);
        return;
    }
    LogDebug("Content-Type [%s], Key[%s], Value[%s]", strContentType.c_str(), strContentKey.c_str(), strContentValue.c_str());
    strContentType.MakeLower();
    // LogDebug("[%s]%s", strType.c_str(), strData.c_str());
    CMyString strData;
    m_post.SetPeekMode(true);
    m_post.Read(strData);
    m_post.SetPeekMode(false);
    if (strContentType=="text/plain" || strContentType=="application/x-www-form-urlencoded")
    {
        LogDebug("[%s]%s", strContentType.c_str(), strData.c_str());
        this->parseParam(strData);
    }
    else if (strContentType == "application/json" || strContentType=="text/json")
    {
        LogDebug("[%s]%s", strContentType.c_str(), strData.c_str());
        this->parseParamByJson(strData);
        // NOTE: 对于比较复杂的Json串，得自行解析
    }
    else if (strContentType == "text/xml")
    {
        LogDebug("[%s]%s", strContentType.c_str(), strData.c_str());
        this->parseParamByXml(strData);
    }
    else if (strContentType == "multipart/form-data")
    {
        // 多个部分进行解析
        if (strcasecmp(strContentKey.c_str(), "boundary")==0)
        {
            this->parseMultiPart(m_post, strContentValue);
        }
        else
        {
            LogFatal("not support ContentKey[%s]", strContentKey.c_str());
        }
    }
    else
    {
        // TODO: 其他格式暂时不支持主动解析 
        LogWarn("unsupport Content-Type: %s", strContentType.c_str());
    }
}

void CHttpRequest::setIp(const CMyString &strIp)
{
	m_strIp = strIp;	
}

CMyString CHttpRequest::getUrl(std::vector<CMyString> &except_key)
{
	return node_get_all(m_params, except_key);
}

void CHttpRequest::parseParamByJson(const CMyString &strJson)
{
    //LogWarn("json data [%s]", strJson.c_str());
    CJsonWrapper jr(strJson);
    Json::Value &val = jr.getJson();

    Json::Value::Members mem = val.getMemberNames();
    for(Json::Value::Members::iterator it=mem.begin(); it != mem.end(); ++it)
    {
        switch(val[*it].type())
        {
        case Json::objectValue:
            break;
        case Json::arrayValue:
            break;
        default:
            {
                CMyString strKey = *it;
                CMyString strVal = val[*it].asString();
                Node node;
                strKey.trim();
                strVal.trim();
                node.name = strKey.c_str();
                node.value = strVal.c_str();
                m_params.push_back(node);
            }
            break;
        }
    }
}

void CHttpRequest::parseParamByXml(const CMyString &strXml)
{
    
}

void CHttpRequest::setMethod(const CMyString& method)
{
	m_method = method;
}

void CHttpRequest::setUrl(const CMyString& url)
{
    LogErr("[%s:%s:%d]old url[%s], new url[%s]", __FILE__, __func__, __LINE__, m_url.c_str(), url.c_str());
	m_url = url;
}

void CHttpRequest::setPost(void *pBuff, size_t cbBuff)
{
	m_buffer.Write(pBuff, cbBuff);
}

const CMyString CHttpRequest::getUrl(void)
{
    CMyString strUrl = m_url;
    return strUrl;
}

const CMyString CHttpRequest::getMethod(void)
{
    CMyString strMethod = m_method;
    return strMethod;
}

const CMyString CHttpRequest::getFullUrl(void)
{
	CMyString strUrl = m_url;
	if (!m_params.empty())
	{
		strUrl += "?";
	}
	for(NodeVector::iterator it(m_params.begin()); it != m_params.end(); ++it)
	{
		struct Node &node = *it;
		strUrl.Append("%s=%s&", node.name.c_str(), node.value.c_str());		
	}
	strUrl.erase(strUrl.size()-1);

	return strUrl.c_str();
}

void CHttpRequest::getFullParamStr(CMyString &strParamStr)
{
	for(NodeVector::iterator it(m_params.begin()); it != m_params.end(); ++it)
	{
		struct Node &node = *it;
		strParamStr.Append("%s=%s&", node.name.c_str(), node.value.c_str());		
	}
    if (!strParamStr.empty())
    {
	    strParamStr.erase(strParamStr.size()-1);
    }
}

void CHttpRequest::getMapNode(map<string, string> &mapNode)
{
	for (NodeVector::iterator it = m_params.begin(); it != m_params.end(); ++it)
	{
		mapNode.insert(make_pair(it->name,it->value));
	}
	return;
}

void CHttpRequest::UrlEncode()
{
	for (NodeVector::iterator it = m_params.begin(); it != m_params.end(); ++it)
	{
        CMyString str(it->value);
        // it->value = CHttpWrapper::UrlEncode(str);
        it->value = CHttpWrapper::urlEncode(str);

		//it->value = UTILS::CryptHelper::UrlEncode(it->value.c_str(), it->value.size());
	}
}

void CHttpRequest::UrlDecode()
{
	for (NodeVector::iterator it = m_params.begin(); it != m_params.end(); ++it)
	{
        CMyString str(it->value);
        // it->value = CHttpWrapper::UrlDecode(str);
        CMyString strClear;
        CHttpWrapper::urlDecode(str, strClear);
        it->value = strClear;

		//it->value = UTILS::CryptHelper::UrlDecode(it->value.c_str(), it->value.size());
	}
}

void CHttpRequest::getPostFromMultipart(map<string, string>& mapPara)
{
	string strContentType = this->getHeader("Content-Type");

	vector<string> vecSeg;
	safe_strtok(strContentType.c_str(), vecSeg, "=");

	if (vecSeg.size() != 2)
	{
		LogErr("getPostFromMultipart: seg size:%d", vecSeg.size());
		return;
	}

	string strSeg = "--";
	strSeg += vecSeg[1] ;

	
    

	char szData[10240];
	m_buffer.Read((void*)szData, m_buffer.size());

	vector<string> vecPara;
	safe_strsplit(szData, vecPara, strSeg.c_str());
	vector<string>::iterator it_pa = vecPara.begin();
	for (; it_pa != vecPara.end(); ++it_pa)
	{
		vector<string> vecPair;
		safe_strtok((*it_pa).c_str(), vecPair, "\r\n");

		if (vecPair.size() == 0)
		{			
			continue;
		}

		int kenLen = strlen("Content-Disposition: form-data; name=\"");
		if ((int) vecPair[0].length() <= kenLen)
		{
			continue;
		}
		string strName = vecPair[0].substr(kenLen, vecPair[0].size() - kenLen - 1);
		if (vecPair.size() == 2)
		{
			mapPara[strName] = vecPair[1];
		}
		else
		{
			mapPara[strName] = "";
		}
	} 
}

// 解析 Content-Disposition: xxxx
int32_t decode_disposition_string(const char* pszDesposition, CMyString &strType, CMyString &strKey1, CMyString &strValue1, CMyString &strKey2, CMyString &strValue2)
{
    char str1[128] = "";
    char str2[128] = "";
    char str3[128] = "";
    char str4[128] = "";
    char str5[128] = "";
    char str6[128] = "";
    int32_t ret = sscanf(pszDesposition, 
            "%[^: ]:%[^;];%[^=]=\"%[^\"]\";%[^=]=\"%[^\"]", 
            str1, str2, str3, str4, str5, str6); 
    if (ret <4)
    {
        return -1;
    }
    else if (ret>=4 && ret<=5)
    {
        strType = str2;
        strKey1 = str3;
        strValue1 = str4;
        strType.trim();
        strKey1.trim();
        strValue1.trim();
        return 1;
    }
    strType = str2;
    strKey1 = str3;
    strValue1 = str4;
    strKey2 = str5;
    strValue2 = str6;
    strType.trim();
    strKey1.trim();
    strValue1.trim();
    strKey2.trim();
    strValue2.trim();
    return 2;
}

int32_t CHttpRequest::parseMultiPart(CByteStream &buff, const CMyString &strBoundary)
{
    uint32_t nLength = getContentLength();
    CMultiPartData mpd;
    int32_t ret = mpd.Decode(*this, buff, nLength, strBoundary);
    LogDebug("HTTP>deal multipart/form-data result [%d]", ret);
    return 0;
}

bool CHttpRequest::ValidParam(const CMyString &params, CMyString &strResult)
{
    CStrSpliter css; 
    if (!css.Split(params, ','))
    {
        strResult.Format("params [%s] not valid list.", params.c_str());
        return false;
    }

    std::vector<CMyString> vecParam;
    for(int32_t i(0); i<css.size(); ++i) 
    {    
        CMyString str1 = css.at(i);
        str1.trim();
        vecParam.push_back(str1);
    }

    return this->ValidParam(vecParam, strResult);
}

bool CHttpRequest::ValidParam(std::vector<CMyString> &vecParam, CMyString &strResult)
{
    int32_t cnt(0);
    strResult.Format("miss parameter:");
    std::vector<CMyString>::iterator it(vecParam.begin());
    for(; it != vecParam.end(); ++it)
    {
        CMyString param = *it;
        CMyString strVal;
        if (!node_get(m_params, param, strVal))
        {
            strResult += param;
            strResult +=',';
            ++cnt;
        }
    }
    if (cnt > 0)
    {
        strResult.erase(strResult.length()-1);
        return false;
    }
    return true;
}

int32_t CHttpRequest::BuildRespMaker(CHttpRespMaker &respMaker)
{
    int32_t cnt(0);
    NodeVector::iterator it(m_params.begin());
    for(; it != m_params.end(); ++it)
    {
        Node &node = *it;
        respMaker.add(node.name, node.value);
        ++cnt;
    }
    return cnt;
}

int32_t CHttpRequest::BuildRespMaker(std::vector<CMyString> &vecParam, CHttpRespMaker &respMaker)
{
    int32_t cnt(0);

    std::vector<CMyString>::iterator it(vecParam.begin());
    for(; it != vecParam.end(); ++it)
    {
        CMyString strKey = *it;
        CMyString strVal = node_get(m_params, strKey);
        if (!strVal.empty())
        {
            respMaker.add(strKey, strVal);
            ++cnt;
        }
    }
    return cnt;
}

int32_t CHttpRequest::BuildRespMaker_Except(std::vector<CMyString> &vecParam, CHttpRespMaker &respMaker)
{
    int32_t cnt(0);
    NodeVector::iterator it(m_params.begin());
    for(; it != m_params.end(); ++it)
    {
        Node &node = *it;
        CMyString strKey = node.name;
        if (std::find(vecParam.begin(), vecParam.end(), strKey) != vecParam.end())
        {
            continue;
        }
        respMaker.add(node.name, node.value);
        ++cnt;
    }
    return cnt;
}

bool CHttpRequest::IsParamExist(const CMyString &strParam)
{
    return this->node_find(m_params, strParam);
}


} 



