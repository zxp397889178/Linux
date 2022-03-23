/*
* =====================================================================================
*
*       Filename:  Session.cpp
*
*    Description:  for connection session
*
*        Version:  1.0
*        Created:  2015年09月08日 14时23分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
*   Organization:  www.skyunion.net(IGG)
*
* =====================================================================================
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "SessionHttp.h"
#include "NetWork/NetFunc.h"
#include "../Protocol/protoSystem.h"
#include "http/HttpRequest.h"
static const int32_t s_nHeartBeat = 5 * 1000;		// 心跳超时设置
static const int32_t s_nMaxHeartCount = 6;			// 心跳检查最大次数
static const int32_t s_nZombieTimeout = 1000;		// zombie connect
static const int32_t s_nAuthTimeout = 1000;		// authorize timeout


CSessionHttp::CSessionHttp(enum Actor actor, const CMyString &actorName
	, ISessionMgr *pSessionMgr, int32_t fd, mm::Reactor &reactor
	, uint32_t ip, const CMyString &strIp)
	: CSession(actor, actorName, pSessionMgr, fd, reactor, ip, strIp)
{
	m_httpReq.setIp(strIp);
	m_bReceived = false;
	m_timerCloseWait = 0;
}

CSessionHttp::~CSessionHttp()
{
	if (m_timerCloseWait != 0)
	{
		this->cancelTimer(m_timerCloseWait);
	}
	LogDebug2("HTTP",0, "session [%u] closed", m_nSessionId);
}

bool CSessionHttp::handleRead(void)
{
	if (m_eState == stateInit)
	{
		m_eState = stateActive;
		if (m_timerZombie != 0)
		{
			this->cancelTimer(m_timerZombie);
			m_timerZombie = 0;
		}
		//m_timerAuth = this->setTimer(s_nAuthTimeout);
	}
	//LogDebug("begin handle read. received size=[%ld]", m_input.size());

	char buf[8192] = "";
	int32_t ret = 0;
	//接收全部数据
	do {
		memset(buf, 0, sizeof(buf));
		ssize_t n = recv(m_fd, buf, sizeof(buf), 0);
		if (0 == n)
		{
			//连接已关闭
			m_eReason = closePassive;
			return false;
		}
		else if (n < 0)
		{
			if (errno == EINTR)
			{
				LogWarn("EINTR, received buff size=[%ld]", errno, m_input.size());
				continue;
			}

			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				LogWarn("handle read break. errno=[%d], received buff size=[%ld]", errno, m_input.size());
				break;
			}
			LogErr("ERR:[%s#%u ch#%u]call recv(), socket occur error [%d] %s",
				m_actorName.c_str(), m_svrId, m_channelId, errno, strerror(errno));
			m_eReason = closeNetFault;
			return false;
		}

		size_t bufLen((size_t)n);
		++m_pkgInput;
		m_tLastRecv = Now();
		m_nHeartBeatCount = 0;

		// 发送数据，并统计
		m_input.Write((const void*)buf, bufLen);
		//IPMON.OnNetReceive(m_ip, n);
		LogTrace("[%s#%d ch#%u]recv %ld bytes, input pkgIndex %d",
			m_actorName.c_str(), m_svrId, m_channelId, n, m_pkgInput);
	} while (true);

	if (m_bReceived)
	{
		return true;
	}

	size_t cbData = m_input.size();
	CTempBuffer tmpBuf(cbData);
	void *pData = (void*)tmpBuf.data();
	m_input.Read(pData, cbData);
	//LogDebug("tmpBuf=[%s], size=[%ld]", tmpBuf.data(), cbData);
	ret = m_httpReq.read(pData, cbData);

	if (ret == http::CHttpData::statePending)
	{
		return true;
	}
	else if (ret != http::CHttpData::stateOK)
	{
		CMyString result("{\"Result\":-99, \"ErrMsg\":\"Invalid data request.\"}");
		this->errRespone(200, "OK", result.c_str());
		LogErr2("HTTP", 0 , "Invalid data request. ret=[%d]", ret);
		return true;
	}

	if (this->process_data() < 0)
	{
		return false;
	}

	m_bReceived = true;
	return true;
}

bool CSessionHttp::handleWrite(void)
{
	// 注册关闭事件
	if (m_timerCloseWait == 0)
	{
		m_timerCloseWait = this->setTimer(500);
		m_eReason = closePending;
		m_eState = stateClosing;
	}

	return CSession::handleWrite();
}

bool CSessionHttp::handleTimeout(unsigned int timerId)
{
	// TODO: 必须下发客户端的异常状态
	// 另外，不处理心跳，
	if (timerId == m_timerZombie)
	{
		LogWarn("client #%u is zombie", m_nSessionId);
		//m_eReason = closeZombie;
		//result = -1;
		m_timerZombie = 0;
		this->errRespone(404, "FAIL", "connection is zombie");
		return false;
	}
	else if (timerId == m_timerCloseWait)
	{
		LogImpt("[CLOSE_WAIT]session[%u] catch socket %d close event.", m_nSessionId, m_fd);
		m_timerCloseWait = 0;
		return false;
	}
	else
	{
		LogErr("unsupport timerId %u", timerId);
	}

	return false;
}

int32_t CSessionHttp::process_data(void)
{
	// 填充记录头
	m_pSessionMgr->OnRcvHttpMsg(this, m_httpReq);
	//this->setEvents(EPOLLIN | EPOLLOUT);
	return 1;
}

void CSessionHttp::errRespone(uint32_t code, const char *status, const char *data)
{
	m_httpResp.setStatusCode(code);
	m_httpResp.setStatus(status);
	m_httpResp.setHeader("Data", getGMTDate(time(NULL)));
	m_httpResp.setHeader("Content-Type", "text/html");
	m_httpResp.setHeader("Server", "Apache/1.3.12(Unix)");

	CByteStream &buff = m_httpResp.getBuff();
	if (data == NULL)
	{
		m_httpResp.setContentLength(buff.size());
	}
	else
	{
		CMyString result(data);
		m_httpResp.setContentLength(result.length());
		buff.clear();
		buff.Write((void*)result.c_str(), result.size());
	}

	CByteStream outBuff;
	m_httpResp.pack(outBuff, true);
	this->httpRespone(outBuff);
}

void CSessionHttp::httpRespone(CByteStream &outBuff)
{
	size_t cbData = outBuff.size();
	CTempBuffer tmpBuf(cbData);
	void *pData = (void*)tmpBuf.data();
	outBuff.Read(pData, cbData);

	this->SendMsg(pData, cbData);

}

void CSessionHttp::httpRespone(uint32_t code, CMyString &strStatus, CMyString &strData, http::CHttpData::NodeVector &vecHead)
{
	m_httpResp.setStatusCode(code);
	m_httpResp.setStatus(strStatus.c_str());
	m_httpResp.setHeader("Date", getGMTDate(time(NULL)));
	m_httpResp.setHeader("Content-Type", "text/html");
	m_httpResp.setHeader("Server", "Apache/1.3.12(Uinx)");

	// 自定义头
	for (uint32_t i = 0; i < vecHead.size(); ++i)
	{
		m_httpResp.setHeader(vecHead[i].name, vecHead[i].value);
	}

	CByteStream &buff = m_httpResp.getBuff();
	if (strData.length() == 0)
	{
		m_httpResp.setContentLength(buff.size());
	}
	else
	{
		//CMyString result(data);
		m_httpResp.setContentLength(strData.length());
		buff.clear();
		buff.Write((void*)strData.c_str(), strData.size());
	}

	CByteStream outBuff;
	m_httpResp.pack(outBuff, true);
	this->httpRespone(outBuff);

	return;

}

