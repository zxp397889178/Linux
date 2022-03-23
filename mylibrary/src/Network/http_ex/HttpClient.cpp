/*
 * =====================================================================================
 *
 *       Filename:  HttpClient.cpp
 *
 *    Description:  HTTP 客户端，基于 TcpClient.h
 *
 *        Version:  1.0
 *        Created:  2020年02月21日 19时01分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "HttpClient.h"
#include "../HttpFunc.h"
#include "../TcpClient.h"
#include "Security.h"

namespace httpex{


CHttpClient::CHttpClient()
{

}

CHttpClient::~CHttpClient()
{

}

int32_t CHttpClient::Exec(http::CHttpRespone &resp)
{
    m_errno = errOK;
    m_errNote.clear();

    // 解析URL
    CMyString strHost, strCgi, strParam;
    bool bSSL(false);
    int32_t port;
    if (!decode_url(m_strHost, strHost, port, strCgi, strParam, bSSL))
    {
        m_errno = errUrlInvalid;
        m_errNote.Format("host url [%s] invalid", m_strHost.c_str());
        LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, m_errNote.c_str());
        return -1;
    }

    // 构建HTTP头
    CMyString strHeader;
    int32_t ret = this->build_header(strHeader);
    if (ret < 0)
    {
        m_errno = errUnsupportMethod;
        m_errNote.Format("unsupport method %s", m_strMethod.c_str());
        return -3;
    }

    // 连接服务器
    // printf("host [%s], port[%d], ssl[%s]\n", strHost.c_str(), port, bSSL ? "true" : "false");
    net::CTcpClient net;
    ret = net.Open(strHost, port, bSSL);
    if (ret < 0)
    {
        m_errno = errHostUnreach;
        m_errNote.Format("connect to %s:%d failure", strHost.c_str(), port);
        LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, m_errNote.c_str());
        return -5;
    }

    // 向服务端发送header
    ret = net.SendMsg((void*)strHeader.c_str(), strHeader.size());
    if (ret < 0)
    {
        m_errno = errSendHeaderFailure;
        m_errNote.Format("send header to %s:%d failure, errno:%d, error:%s", strHost.c_str(), port, errno, strerror(errno));
        LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, m_errNote.c_str());
        return -7;
    }

    // 发送数据
    size_t cbBuff = m_data.size();
    byte_t *pBuff = (byte_t*)calloc(1, cbBuff+1);
    m_data.Read(pBuff, cbBuff);
#if 0    
    // 看下是否需要解析数据
    if (this->IsUrlEncodeParam())
    {
        CMyString strOutput;
        this->parseUrlEncodeParam(pBuff, cbBuff, strOutput);
        printf("urlencode data[%s]\n", strOutput.c_str());
        ret = net.SendMsg((void*)strOutput.c_str(), strOutput.size());
    }
    else
    {
        ret = net.SendMsg(pBuff, cbBuff);
    }
#else
    ret = net.SendMsg(pBuff, cbBuff);
#endif    
    if (ret < 0)
    {
        free(pBuff);
        m_errno = errSendDataFailure;
        m_errNote.Format("send data to %s:%d failure, errno:%d, error:%s", strHost.c_str(), port, errno, strerror(errno));
        LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, m_errNote.c_str());
        return -9;
    }
    free(pBuff);

    // 准备接收结果
    // http::CHttpRespont resp;
    char szText[10240] = "";
    size_t cbText = sizeof(szText);
    int32_t result(0);
    //uint64_t t1 = Now();
    do {
        if (resp.IsFinished()) break;
        ret = net.RecvMsg(szText, cbText, 10);
        if (ret > 0)
        {
            resp.read((void*)szText, ret);
        }
        else if (ret == 0)
        {
            break;
        }
        else
        {
            m_errno = errRecvDataFailure;
            m_errNote.Format("recv resp from %s:%d failure, errno:%d, error:%s", strHost.c_str(), port, errno, strerror(errno));
            LogFatal("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, m_errNote.c_str());
            // 远端异常
            result = -11;
            break;
        }
    }while(true);

    // 断开网络连接
    net.Close();
    return result;
}

bool CHttpClient::setHost(const CMyString &host)
{
    CMyString strHost, strCgi, strParam;
    bool bSSL(false); // SSL 模式暂时不支持
    int32_t port;
    if (!decode_url(host, strHost, port, strCgi, strParam, bSSL))
    {
        LogFatal("[%s:%s:%d]host url [%s] invalid.", __FILE__, __func__, __LINE__, m_strHost.c_str());
        return false;
    }

    // TODO: 是否需要去解析 strHost 是否合法

    m_strHost = host;
    return true;
}

bool CHttpClient::setMethod(const CMyString &method)
{
    bool result(false);
    m_strMethod = method;
    m_strMethod.MakeUpper();
    if (m_strMethod == "GET")
    {
        m_method = GET;
        result = true;
    }
    else if (m_strMethod=="POST")
    {
        m_method = POST;
        result = true;
    }
    else if (m_strMethod == "OPTIONS")
    {
        m_method = OPTIONS;
    }
    else if (m_strMethod == "PUT")
    {
        m_method = PUT;
    }
    else if (m_strMethod == "DELETE")
    {
        m_method = DELETE;
    }
    else if (m_strMethod == "HEAD")
    {
        m_method = HEAD;
    }
    else if (m_strMethod == "TRACE")
    {
        m_method = TRACE;
    }
    else
    {
        LogErr("unknown method [%s]", m_strMethod.c_str());
    }
    return result;
}

void CHttpClient::setKeepAlive(bool keep_alive)
{
    m_bKeepAlive = keep_alive;
}

int32_t CHttpClient::addKvItem(VEC_HEADER &cont, const CMyString &key, const CMyString &value, bool bOverride)
{
    VEC_HEADER::iterator it(cont.begin());
    for(; it != cont.end(); ++it)
    {
        HeaderItem &item = *it;
        if (item.key == key)
        {
            if (bOverride)
            {
                item.value = value;
                return 1;
            }
            return 0;
        }
    }

    // 未找到key, 则新增
    HeaderItem item;
    item.key = key;
    item.value = value;
    cont.push_back(item);
    return 1;
}

int32_t CHttpClient::decode_kvstring(const CMyString &kvstring, char delimiter, CMyString &key, CMyString &value)
{
    if (kvstring.empty())
    {
        LogErr("[%s:%s:%d]kv string is empty.", __FILE__, __func__, __LINE__);
        return -1;
    }

    size_t pos = kvstring.find_first_of(delimiter);
    if (pos == std::string::npos)
    {
        LogErr("[%s:%s:%d]kv string [%s] invalid.",
                __FILE__, __func__, __LINE__, kvstring.c_str());
        return -3;
    }
    key = kvstring.substr(0, pos);
    value = kvstring.substr(pos+1);
    key.trim();
    value.trim();
    return 0;
}


int32_t CHttpClient::addKvString(VEC_HEADER &cont, const CMyString &kv_strings, char delimiter1, char delimiter2, bool bOverride)
{
    if (kv_strings.empty())
    {
        return 0;
    }

    //key:value;key:vlaue;...
    int32_t cnt(0);
    CMyString kv_list(kv_strings);

    bool bLast(false);
    CMyString strItem;
    size_t pos = kv_list.find_first_of(delimiter1);
    if (pos == std::string::npos)
    {
        strItem = kv_list;
        kv_list.clear();
        bLast = true;
    }
    else
    {
        strItem = kv_list.substr(0, pos);
        kv_list = kv_list.substr(pos+1);
    }
    
    do {
        CMyString key_, value_;
        int32_t ret = this->decode_kvstring(strItem, delimiter2, key_, value_);
        if (ret == 0)
        {
            ret = this->addKvItem(cont, key_, value_, bOverride);
            if (ret > 0) ++cnt;
        }
        if (bLast)
        {
            break;
        }

        pos = kv_list.find_first_of(delimiter1);
        if (pos == std::string::npos)
        {
            // 最后一项了
            bLast = true;
            strItem = kv_list;
            kv_list.clear();
        }
        else
        {
            strItem = kv_list.substr(0, pos);
            kv_list = kv_list.substr(pos+1);
        }
    }while(true);

    return cnt;
}

int32_t CHttpClient::addHeader(const CMyString &key, const CMyString &value, bool bOverride)
{
    return this->addKvItem(m_vecHeader, key, value, bOverride);
}

int32_t CHttpClient::addHeader(const CMyString &headers, bool bOverride)
{
    //key:value;key:vlaue;...
    //printf("add header [%s]\n", headers.c_str());
    if (headers.empty())
    {
        return 0;
    }
    return this->addKvString(m_vecHeader, headers, ';', ':', bOverride);
}
        
int32_t CHttpClient::addCookie(const CMyString &key, const CMyString &value, bool bOverride)
{
    return this->addKvItem(m_vecCookie, key, value, bOverride);
}

int32_t CHttpClient::addCookie(const CMyString &cookies, bool bOverride)
{
    if (cookies.empty())
    {
        return 0;
    }
    return this->addKvString(m_vecCookie, cookies, ';', '=', bOverride);
}

int32_t CHttpClient::addParam(const CMyString &key, const CMyString &value, bool bOverride)
{
    return this->addKvItem(m_vecParam, key, value, bOverride);
}

int32_t CHttpClient::addParam(const CMyString &params, bool bOverride)
{
    //key=value&key=vlaue&...
    // printf("add param [%s]\n", params.c_str());
    if (params.empty())
    {
        return 0;
    }
    return this->addKvString(m_vecParam, params, '&', '=', bOverride);
}

int32_t CHttpClient::build_header(CMyString &strHeader)
{
    // 解析URL
    CMyString strHost, strCgi, strParam;
    bool bSSL(false); // SSL 模式暂时不支持
    int32_t port;
    if (!decode_url(m_strHost, strHost, port, strCgi, strParam, bSSL))
    {
        LogFatal("[%s:%s:%d]host url [%s] invalid.", __FILE__, __func__, __LINE__, m_strHost.c_str());
        return -1;
    }

    int32_t result(0);
    // 若不存在以下header,则增加之
    this->addHeader("User-Agent", "webmw/2.0", false);
    this->addHeader("Cache-Control", "no-cache", false);
    this->addHeader("Accept", "*/*", false);
    this->addHeader("Content-Type", "text/plain", false);
    this->addHeader("Accept-Encoding", "*", false);

    // 构建Content-Length
    this->buildContentLength();

    // 构建param
    if (!strParam.empty())
    {
        this->addParam(strParam, true);
    }
    build_param(strParam);

    // 构建header
    switch(m_method)
    {
    case GET:
        m_strMethod = "GET";
        break;
    case POST:
        m_strMethod = "POST";
        break;
    default:
        // TODO: 暂不处理其他方法
        LogFatal("[%s:%s:%d]unsupport method [ %s ].", __FILE__, __func__, __LINE__, m_strMethod.c_str());
        result = -3;
        break;
    }
    if (result < 0)
    {
        return result;
    }

    // 构建GET的第一行
    if (strCgi.empty())
    {
        strHeader.Append("%s / HTTP/1.1\r\n", m_strMethod.c_str());
    }
    else if (strParam.empty())
    {
        strHeader.Append("%s /%s HTTP/1.1\r\n", m_strMethod.c_str(), strCgi.c_str());
    }
    else
    {
        strHeader.Append("%s /%s?%s HTTP/1.1\r\n", m_strMethod.c_str(), strCgi.c_str(), strParam.c_str());
    }

    strHeader.Append("Host: %s:%d\r\n", strHost.c_str(), port);
    if (m_bKeepAlive)
    {
        strHeader.Append("Connection: keep-alive\r\n");
    }
    else
    {
        strHeader.Append("Connection: close\r\n");
    }
     
    // 循环添加header
    VEC_HEADER::iterator it(m_vecHeader.begin());
    for(; it != m_vecHeader.end(); ++it)
    {
        HeaderItem &item = *it;
        strHeader.Append("%s: %s\r\n", item.key.c_str(), item.value.c_str());
    }

    // 获取Cookies
    if (!m_vecCookie.empty())
    {
        CMyString strCookie("Cookie: ");
        VEC_PARAM::iterator iter(m_vecCookie.begin());
        for(; iter != m_vecCookie.end(); ++iter)
        {
            ParamItem &item = *iter;
            if (iter != m_vecCookie.begin())
            {
                strCookie += ";";
            }
            strCookie.Append("%s=%s", item.key.c_str(), item.value.c_str());
        }
        strCookie.Append("\r\n");
        strHeader.append(strCookie.c_str());
    }

    // 增加结束行
    strHeader.Append("\r\n");  

    return 0;
}

void CHttpClient::build_param(CMyString &strParam)
{
    strParam.clear();

    VEC_PARAM::iterator it(m_vecParam.begin());
    for(; it != m_vecParam.end(); ++it)
    {
        ParamItem &item = *it;
        if (it == m_vecParam.begin())
        {
            strParam.Append("%s=%s", item.key.c_str(), item.value.c_str());
        }
        else
        {
            strParam.Append("&%s=%s", item.key.c_str(), item.value.c_str());
        }
    }
}

bool CHttpClient::getHeader(const CMyString &key, CMyString &value)
{
    VEC_HEADER::iterator it(m_vecHeader.begin());
    for(; it != m_vecHeader.end(); ++it)
    {
        HeaderItem &item = *it;
        if (item.key == key)
        {
            value = item.value;
            return true;
        }
    }
    return false;
}

void CHttpClient::removeHeader(const CMyString &key)
{
    VEC_HEADER::iterator it(m_vecHeader.begin());
    for(; it != m_vecHeader.end(); ++it)
    {
        HeaderItem &item = *it;
        if (item.key == key)
        {
            m_vecHeader.erase(it);
            break;
        }
    }
}

void CHttpClient::buildContentLength(void)
{
    if (this->IsChunkedMode())
    {
        // 不需要用 Content-Length
        return;
    }

    CMyString strContentLength("%lu", m_data.size());
    this->addHeader("Content-Length", strContentLength, true);
}

bool CHttpClient::IsChunkedMode(void)
{
    // Transfer-Encoding: chunked,
    // Transfer-Encoding: identity

    CMyString strValue;
    if (!this->getHeader("Transfer-Encoding", strValue))
    {
        return false;
    }
    if (strncmp(strValue.c_str(), "chunked", 7)==0)
    {
        return true;
    }
    return false;
}

bool CHttpClient::IsUrlEncodeParam(void)
{
    CMyString strContentType;
    if (!this->getHeader("Content-Type", strContentType))
    {
        return false;
    }
    if (strncmp(strContentType.c_str(), "application/x-www-form-urlencoded", 33 )==0)
    {
        return true;
    }
    return false;
}


bool CHttpClient::IsMultipartMode(CMyString &strBoundary)
{
    // Content-Type: multipart/form-data;boundary=ZnGpDtePMx0KrHh_G0X99Yef9r8JZsRJSXC

    CMyString strValue;
    if (!this->getHeader("Content-Type", strValue))
    {
        return false;
    }
    if (strncmp(strValue.c_str(), "multipart/form-data", 19)==0) // multipart/form-data
    {
        // 解析出 boundary
        size_t pos = strValue.find("boundary=");
        if (pos == std::string::npos)
        {
            // 无boundary, 动态生成一个boundary
            this->BuildBoundary(strBoundary);
            strValue.Format("multipart/form-data;boundary=%s", strBoundary.c_str());
            this->addHeader("Content-Type", strValue, true);
        }
        else
        {
            strBoundary = strValue.substr(pos+9+1); // boundary=<boundary>
        }
        return true;
    }

    return false;
}

void CHttpClient::BuildBoundary(CMyString &strBoundary)
{
    static char s_charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static int32_t s_charlen = 62;

    // 构建64个分隔符，其中前16位为-
    std::string str1(16, '-');
    // ----------------ajdfe1w3o5e6r7o07687iAfBlfmeTroSSFfgjrs9863eihs1 
    for(int32_t i(0); i<48; ++i)
    {
        int32_t index = RandGet()%s_charlen;
        char c = s_charset[index];
        str1 += c;
    }
    strBoundary = str1.c_str();
}


void CHttpClient::addPost(byte_t *pData, size_t cbData, const CMyString &strExtraInfo, bool bLast)
{
    // multipart/form-data
    // chunked
    CMyString strBoundary;
    if (this->IsChunkedMode())
    {
        // chunked 模式存数据
        // 每个 chunked = 1KB
        int32_t size(1024);
        int32_t left(cbData);
        char *pc = (char*)pData;
        CMyString strHeader;
        do {
            if (left>1024) size = 1024;
            else size = left;
            CMyString strHeader("%x\r\n", size);
            m_data.Write((void*)strHeader.c_str(), strHeader.size());
            m_data.Write(pc, size);
            CMyString strTail("\r\n");
            m_data.Write((void*)strTail.c_str(), strTail.size());

            left -= size;
        }while(left>0);

        if (bLast)
        {
            CMyString strEndl("0\r\n\r\n");
            m_data.Write((void*)strEndl.c_str(), strEndl.size());
        }
    }
    else if (this->IsMultipartMode(strBoundary))
    {
        // 以multipart/form data 存数据
        CMyString boundary("--%s\r\n", strBoundary.c_str());
        m_data.Write((void*)boundary.c_str(), boundary.size());
        // strExtraIfo ==>
        // Content-Disposition: form-data;name="desc"\r\n
        // Content-Type: text/plain; charset=UTF-8\r\n
        // Content-Transfer-Encoding: 8bit\r\n
        m_data.Write((void*)strExtraInfo.c_str(), strExtraInfo.size());
        CMyString strTail("\r\n");
        m_data.Write((void*)strTail.c_str(), strTail.size());
        m_data.Write(pData, cbData);
        if (bLast)
        {
            boundary.Format("--%s--\r\n", strBoundary.c_str());
        }
        m_data.Write((void*)boundary.c_str(), boundary.size());
    }
    else if (this->IsUrlEncodeParam())
    {
        CMyString strOut;
        this->parseUrlEncodeParam(pData, cbData, strOut);
        m_data.Write(strOut);
    }
    else
    {
        // 无格式，直接存
        m_data.Write(pData, cbData);
    }
}


int32_t CHttpClient::Get(const CMyString &strHost, const CMyString &strHeader, http::CHttpRespone &resp)
{
    this->setHost(strHost);
    this->setMethod("GET");
    this->addHeader(strHeader, true);
    this->setKeepAlive(false);
    return this->Exec(resp);
}

int32_t CHttpClient::Post(const CMyString &strHost, const CMyString &strHeader, byte_t* pData, size_t cbData, http::CHttpRespone &resp)
{
    this->setHost(strHost);
    this->setMethod("POST");
    this->addHeader(strHeader, true);
    this->addPost(pData, cbData, "", true);
    this->setKeepAlive(false);
    return this->Exec(resp);
}

void CHttpClient::parseUrlEncodeParam(byte_t *pData, size_t cbData, CMyString &strOut)
{
    char *pc = (char*)pData;
    CStrSpliter css;
    CStrSpliter subcss;
    if (!css.Split(pc, '&'))
    {
        // 有可能是只有一行
        if (subcss.Split(pc, '='))
        {
            CMyString str1 = subcss[0];
            CMyString str2 = subcss[1];
            CMyString str3 = CHttpWrapper::encodeURIComponent(str2);
            CMyString strValue("%s=%s", str1.c_str(), str3.c_str());
            strOut = strValue;
        }
        else
        {
            strOut = pc;
        }
        return;
    }

    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString strItem = css[i];
        if (subcss.Split(strItem, '='))
        {
            CMyString str1 = subcss[0];
            CMyString str2 = subcss[1];
            CMyString str3 = CHttpWrapper::encodeURIComponent(str2);
            CMyString strValue("%s=%s", str1.c_str(), str3.c_str());
            if (i!=0)
            {
                strOut.append("&");
            }
            strOut.append(strValue);
        }
        else
        {
            if (i!=0)
            {
                strOut.append("&");
            }
            strOut.append(strItem);
        }
    }
}

}



