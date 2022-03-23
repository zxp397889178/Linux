/*
 * =====================================================================================
 *
 *       Filename:  Client.cpp
 *
 *    Description:  client handler
 *
 *        Version:  1.0
 *        Created:  09/10/2019 09:40:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Client.h"
#include "Config.h"

CClient::CClient(mm::Reactor &reactor, uint32_t id, int32_t fd, uint32_t ip, IWebClientMgr *pClientMgr, uint32_t workerId)
    : mm::EventHandler()
    , m_pClientMgr(pClientMgr)
    , m_clientId(id)
    , m_fd(fd)
    , m_ip(ip)
    , m_workerId(workerId)
    , m_eStatus(statePending)
    , m_timerZombie(0)
    , m_timerCloseWait(0)
    , m_pDataBlock(NULL)
{
    char szIp[64] = "";
    ip2str(ip, szIp, sizeof(szIp));
    m_strIp = szIp;
    this->setReactor(&reactor);
    m_httpReq.setIp(m_strIp);
    m_timerZombie = setTimer(CConfig::Instance().getClientActiveTimeout());
    m_timeBegin = Now();
    m_timeReadBegin = 0;
    m_timeExecBegin = 0;
    m_timeExecEnd = 0;
}

CClient::~CClient()
{
    if (m_timerZombie !=0)
    {
        this->cancelTimer(m_timerZombie);
    }
    if (m_timerCloseWait != 0)
    {
        this->cancelTimer(m_timerCloseWait);
    }
    this->setEvents(0);

    if (m_fd > 0)
    {
        close_tcp_socket(m_fd, true);
    }

    if (NULL != m_pDataBlock)
    {
        free(m_pDataBlock);
    }
}

bool CClient::handleRead(void)
{
    if (m_eStatus == statePending)
    {
        m_eStatus = stateActive;
        this->cancelTimer(m_timerZombie);
        m_timerZombie = 0;
        m_timeReadBegin = Now(true);
    }

    char buf[8192]="";
    int32_t ret(0);
    do 
    {
        ssize_t n = recv(m_fd, buf, sizeof(buf), 0);
        if (0 == n)
        {
            // client has shutdown connect
            m_eStatus = stateClosing;
            LogDebug("client #%u:%u shutdown.", m_workerId, m_clientId);
            return false;
        }
        else if (-1 == n)
        {
            if (EAGAIN==errno||EWOULDBLOCK==errno)
            {
                break;
            }
            m_eStatus = stateClosing;
            LogErr("client #%u:%u shutdown for error: [%d] %s", 
                    m_workerId, m_clientId, errno, strerror(errno));
            return false;
        }
        ret = m_httpReq.read((void*)buf, (size_t)n);
    }while(true);

    m_httpResp.setVersion(m_httpReq.getVersion());
	if (ret != http::CHttpData::stateOK && ret != http::CHttpData::statePending)
	{
        CMyString strData;
        m_httpReq.getBuff().Read(strData);
        LogErr("[HTTP state=%d]%s", ret,strData.c_str());
        std::map< CMyString, CMyString > header;
		CMyString result("{\"Result\":\"-1001\", \"ErrMsg\":\"Invalid data request.\"}");
		this->httpResponse(200, "OK", header, result.c_str(), result.length());
		return true;
	}
	
	//处理所有消息
	ret = this->dealHttpData();
	if (-1 == ret)
	{
		//连接关闭
		m_eStatus = stateClosing;
		LogErr("client %u process data failure.", m_clientId);
		return false;
	}
	this->setEvents(EPOLLIN | EPOLLOUT);
    return true;
}

bool CClient::handleWrite(void)
{
	//发送数据到客户端
	int32_t result(0);
	uint32_t totalSend(0);
	uint32_t sendTimeout(0);
	do {
		if (m_output.empty())
		{
			break;
		}
		size_t nLen = m_output.size();
		CTempBuffer tmpBuf(nLen);
		char *pBuf = tmpBuf.data();
		if (!m_output.Read(pBuf, nLen))
		{
			//从内存中读取失败，重做
			continue;
		}
		
		// 设置发送的超时时间
		int32_t timeCnt = (nLen+1023)/1024;
		sendTimeout = timeCnt * 2000;
		uint32_t curSend(0);
		int32_t ret = send_data_ex(m_fd, pBuf, nLen, curSend, sendTimeout);
		if (ret < 0)
		{
			result = -1;
			//其他错误，有可能是断了
			m_eStatus = stateClosing;	
			LogErr("client %u send data failure, reason %d ", m_clientId, ret);
			break;
		}
		totalSend += curSend;
	}while(!m_output.empty());

	if (m_eStatus == stateClosing || m_eStatus == stateClosed)
	{
		//数据发送后，不能马上关闭SOCK,客户端有可能只收到一半的数据
		//result = -1;
		result = 0;
	}
	else
	{
		this->setEvents(EPOLLIN);
	}
	return (result>=0);
}

bool CClient::handleTimeout(unsigned int timerId)
{
	if (timerId == m_timerZombie)
	{
		LogWarn("client #%u is zombie", m_clientId);
        std::map< CMyString, CMyString > header;
        CMyString strError("connection is zombie");
		this->httpResponse(404, "FAIL", header, strError.c_str(), strError.length());
        return false;
	}
	else if (timerId == m_timerCloseWait)
	{
		LogImpt("[CLOSE_WAIT]catch socket %d close event.", m_fd);
		return false;
	}
	else
	{
		LogErr("unsupport timerId %u", timerId);
	}
    return true;
}

bool CClient::handleClose(void)
{
	// 客户端关闭事件
	LogDebug("handle client %d closed.", m_clientId);
	m_eStatus = stateClosed;

	close_tcp_socket(m_fd, true);
	return true;
}

bool CClient::handleError(int32_t events, mm::EventHandler *pHandler)
{
	if (pHandler == this)
	{
		LogDebug("handle client %u error.", m_clientId);
	}
	return true;
}

void HttpHeaderSet(http::CHttpRespone &resp, STRING_MAP &header)
{
    CMyString strKey;
    strKey = "Data";
    std::map< CMyString, CMyString >::iterator it;
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Data", getGMTDate(time(NULL)) );
    }

    strKey = "Content-Type";
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Content-Type", "text/html");
    }

    strKey = "Server";
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Server", "PayCenter/1.0.2(Linux)");
    }

    //CORS 
    strKey = "Access-Control-Allow-Origin";
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Access-Control-Allow-Origin","*");
    }

    strKey = "Access-Control-Allow-Headers";
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Access-Control-Allow-Headers", "*");
    }

    strKey = "Access-Control-Allow-Methods";
    it = header.find(strKey);
    if (it == header.end())
    {
        resp.setHeader("Access-Control-Allow-Methods", "GET,POST");
    }

    CMyString strVal;
    for(it = header.begin(); it != header.end(); ++it)
    {
        strKey = it->first;
        strVal = it->second;

        resp.setHeader(strKey.c_str(), strVal.c_str());
    }
}
void CClient::httpResponse(uint32_t code, const char *status, 
        std::map<CMyString, CMyString> &header, 
        const char *pData, uint32_t cbData)
{
	m_httpResp.setStatusCode(code);
	m_httpResp.setStatus(status);
    HttpHeaderSet(m_httpResp, header);
    // m_httpResp.setHeader("Content-Length", CMyString("%u", cbData).c_str());
	CByteStream &buff = m_httpResp.getBuff();
	if (pData == NULL || cbData==0)
	{
		m_httpResp.setContentLength( buff.size() );
	}
	else
	{
        m_httpResp.setContentLength(cbData);
        buff.clear();
        buff.Write((void*)pData, cbData);
	}

	CByteStream outBuff;
	m_httpResp.pack(outBuff, true);
	this->httpResponse(-1, outBuff);
}

void CClient::httpResponse(uint32_t cmdId, CByteStream &outBuff)
{
	size_t cbData = outBuff.size();
	CTempBuffer tmpBuf(cbData);
	void *pData = (void*)tmpBuf.data();
	outBuff.Read(pData, cbData);
	m_output.Write(pData, cbData);
	this->setEvents(EPOLLOUT);

	// 注册关闭事件
	m_timerCloseWait = this->setTimer(500);
	m_eStatus = stateClosing;

    // 统计cgi函数执行时间
    int64_t timeEnd = Now();

    LogMsg("[STAT]ip:%s, cgi:%s, cost:%dms (read:%.2lfms, exec:%.2lfms)", 
            m_strIp.c_str(), m_cgiName.c_str(), 
            (int32_t)(timeEnd-m_timeBegin),
            (m_timeExecBegin-m_timeReadBegin)/1000.0,
            (m_timeExecEnd-m_timeExecBegin)/1000.0);
}

void CClient::httpResponse(uint32_t cmdId, uint32_t code, const char* status, const char* data)
{
	m_httpResp.setStatusCode(code);
	m_httpResp.setStatus(status);

    std::map<CMyString, CMyString> ext_header;
    HttpHeaderSet(m_httpResp, ext_header);
	CByteStream &buff = m_httpResp.getBuff();
	if (data == NULL)
	{
		m_httpResp.setContentLength( buff.size() );
	}
	else
	{
		CMyString result;
		result.append(data);
		m_httpResp.setContentLength(result.length());
		buff.clear();
		buff.Write((void*)result.c_str(), result.size());
		LogNone("httpresp:\n%s\n", result.c_str());
	}

	CByteStream outBuff;
	m_httpResp.pack(outBuff, true);
	this->httpResponse(cmdId, outBuff);
	LogDebug("[clientId #%u, cmdId %u] response data [code:%u status:%s, result :%s]",
		m_clientId, cmdId, code, status, data);
}

int32_t CClient::dealHttpData(void)
{
    bool bHeaderEnd = false;
	// 解析出HTTP 包头，并处理
	if (m_httpReq.IsHeaderEnd() && m_httpReq.getUnreadLength()==0 )
	{
        bHeaderEnd = true;
    }
    if (!bHeaderEnd)
    {
        return 0;
    }

    if (m_timeExecBegin == 0)
    {
        m_timeExecBegin = Now(true);
    }

	// 头已读取完毕
    CMyString strUrl;
    bool bFind(true);
	m_cgiName = m_httpReq.getUrl();

    // 优先级1：若是空的cgi，则指向预设的cgi
    if (m_cgiName.empty())
    {
        m_cgiName = CONFIG.getEmptyCgi();
    }

    // 优先级2: 重定向到另一个服务
    CMyString strHost;
    bFind = CONFIG.getTransferInfo(m_cgiName, strHost, strUrl);
    if (bFind)
    {
        m_httpReq.setHeader("9527_transfer_host", strHost.c_str());
        m_httpReq.setHeader("9527_transfer_url", m_cgiName.c_str());
        m_httpReq.setUrl(strUrl);
        LogDebug("transfer [%s] => [%s], exec cgi[%s]", m_cgiName.c_str(), strHost.c_str(), strUrl.c_str());
    }
    else
    {
        // 优先级3: 全局别名
        bFind = CONFIG.getAliaseCgi(m_cgiName, strUrl);
        if (bFind)
        {
            m_httpReq.setUrl(strUrl);
            LogDebug("aliase [%s], real cgi [%s]", m_cgiName.c_str(), strUrl.c_str());
        }
    }
    if (!bFind)
    {
        strUrl = m_cgiName;
    }

    // 插件的别名，以及正常的CGI指向
    std::map< CMyString, CMyString > header;
    uint32_t cbResp = CONFIG.getClientRespBufferSize();
    LogDebug("execute cgi [%s]", strUrl.c_str());
    CTempBuffer tmpbuf(cbResp+1);
    char *pcResp = tmpbuf.data();
	int32_t ret(0);
	try
	{
		ret = m_pClientMgr->ExecuteCGI(strUrl, m_httpReq, pcResp, cbResp, header, this);
        m_timeExecEnd = Now(true);
		if (ret < 0)
		{
			this->httpResponse(404, "FAIL", header, tmpbuf.data(), cbResp);
		}
		else
		{
            // 返回值>=0 均为正确
			this->httpResponse(200, "OK", header, tmpbuf.data(), cbResp);
		}
    }
	catch(...)
	{
        m_timeExecEnd = Now(true);
        CMyString strError("process request failure");
		this->httpResponse(500, "FAIL", header, strError.c_str(), strError.length());
		LogFatal("execute cgi [%s] occur error", strUrl.c_str());
	}
    //LogWarn("PAYDBG>execute cgi OK");
	m_httpReq.clear();

	return 0;
}

void CClient::SendMsg(CByteStream &buff)
{
    m_output.Write(buff);
    this->setEvents(EPOLLOUT);
}

void CClient::SendMsg(const void* pData, size_t cbData)
{
    m_output.Write(pData, cbData);
    this->setEvents(EPOLLOUT);
}

void* CClient::allocateNewBlock(uint32_t cbSize)
{
    if (NULL != m_pDataBlock)
    {
        free(m_pDataBlock);
        m_pDataBlock = NULL;
    }

    m_pDataBlock = calloc(1, cbSize);
    return m_pDataBlock;
}

void CClient::setIp(const CMyString &strIp)
{
    m_strIp = strIp;
    m_ip = str2ip(strIp.c_str());
}
