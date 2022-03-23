/*
 * =====================================================================================
 *
 *       Filename:  GameHandler.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月02日 11时28分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include <errno.h>
#include <string.h>
#include "Connector.h"
#include "NetWork/NetFunc.h"



CConnector::CConnector(uint32_t svrId, uint32_t channelId
			, enum Actor actorSelf, enum Actor actor, const CMyString &actorName
			, IConnectorMgr *pConnectorMgr
			, const CMyString &strHost, uint32_t port
			, mm::Reactor &reactor)
	: m_actor(actor)
	, m_actorName(actorName)
	, m_pConnectorMgr(pConnectorMgr)
	, m_reactor(reactor)  
	, m_strHost(strHost)
	, m_port(port)
	, m_fd(-1)
	, m_sendBuf()
	, m_recvBuf()
	, m_svrId(svrId)
	, m_channelId(channelId)  
	, m_sendBytes(0) 
	, m_recvBytes(0)
{
	m_pLock = OpenRwLock();
	m_actorSelf = actorSelf;
}

CConnector::~CConnector()
{
	if (m_fd > 0)
	{
		close_tcp_socket(m_fd);
		m_fd = 0;
	}
	SAFE_DELETE(m_pLock);
}

int32_t CConnector::Init(void)
{
	static int32_t s_sndbuf = 4*1024*1024;
	static int32_t s_rcvbuf = 4*1024*1024;

	m_fd = create_tcp_socket(true, false, false, s_sndbuf, s_rcvbuf);
	if (m_fd <= 0)
	{
		LogFatal("ERR: [%s#%u, ch#%u]can not create tcp socket.",
				m_actorName.c_str(), m_svrId, m_channelId);
		return -1;
	}

	int32_t ret = open_tcp_connect(m_fd, m_strHost.c_str(), m_port, 2000);
	if (ret < 0)
	{
		LogFatal("ERR: [%s#%u, ch#%u]can not connect to %s:%u",
				m_actorName.c_str(), m_svrId, m_channelId, m_strHost.c_str(), m_port);
		close_tcp_socket(m_fd);
		m_fd = 0;
		return -2;
	}

	this->setReactor(&m_reactor);

	// send authorize data
#if 0
	ret = m_pConnectorMgr->OnAuthorize(this);
#else
	ret = this->Authorize();
#endif
	if (ret< 0)
	{
		LogFatal("ERR: [%s#%u, ch#%u]do not authorize self.",
				m_actorName.c_str(), m_svrId, m_channelId);
		return -1;
	}

	LogImpt("[%s#%u, ch#%u]connect to %s:%u succeed.", 
			m_actorName.c_str(), m_svrId, m_channelId, 
			m_strHost.c_str(), m_port);

	return 0;
}

bool CConnector::handleRead(void)
{
	char buf[8192] = "";
	//接收全部数据
	do {
		ssize_t n = recv(m_fd, buf, sizeof(buf), 0);
		if (0 == n)
		{
			//连接已关闭
			//客户端关闭连接
			LogFatal("ERR:[%s#%u, ch#%u]remote shutdown connection.",
					m_actorName.c_str(), m_svrId, m_channelId);
			return false;
		}
		else if (-1 == n )
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				LogWarn2("CONNECT", 0, "[%s#%u, ch#%u]handle read break. errno=[%d], received buff size=[%ld]", m_actorName.c_str(), m_svrId, m_channelId, errno, m_recvBuf.size());
				break;
			}
			LogFatal("ERR:[%s#%u, ch#%u]socket occur error %d, %s",
					m_actorName.c_str(), m_svrId, m_channelId, errno, strerror(errno));
			return false;
		}

		//IPMON.OnGameRecv((int32_t)n);

		m_recvBytes += n;
		m_recvBuf.Write((const void*)buf, n);
		LogTrace("[%s#%d, ch#%u]recv %ld bytes",
				m_actorName.c_str(), m_svrId, m_channelId, n); 
	} while(0);

	// callback, 处理所有消息
	int32_t ret = this->ProcessData();
	if (ret < 0)
	{
		return false;
	}
	this->setEvents(EPOLLIN | EPOLLOUT);

	return true;
}

void CConnector::SendMsg(void *pBuf, size_t cbBuf)
{
	{
		CGuard guard(m_pLock, false);
		m_sendBuf.Write(pBuf, cbBuf);
	}
	this->setEvents(EPOLLOUT);
}

bool CConnector::handleWrite(void)
{
	CByteStream buffer;
	{
		CGuard guard(m_pLock, false);
		buffer.Write(m_sendBuf);
	}

	//send data to gamesvr
	int32_t total_send(0);
	int32_t result(0);
	do {
		if (buffer.empty())
		{
			break;
		}
		size_t nLen = buffer.size();
		CTempBuffer buff(nLen);
		if (!buffer.Read(buff.data(), nLen))
		{
			//从内存中读取失败，重做
			continue;
		}

		// 发送数据
		int32_t ret = send_data(m_fd, buff.data(), nLen);
		if (ret < 0)
		{
			result = -1;
			//network error, maybe shutdown, reconnect it
			if (-2 == ret)
			{
				LogFatal("ERR: [%s#%u, ch#%u]remote shutdown connection",
						m_actorName.c_str(), m_svrId, m_channelId);
			}
			else 
			{
				LogFatal("ERR: [%s#%u, ch#%u]occur network error %d, %s", 
						m_actorName.c_str(), m_svrId, m_channelId, errno, strerror(errno));
			}
			break;
		}

		//IPMON.OnGameSend((int32_t)ret);

		m_sendBytes += ret;
		total_send += ret;
		LogTrace("[%s#%u, ch#%u]send %d bytes.",
				m_actorName.c_str(), m_svrId, m_channelId, ret);
		result = ret;
	}while(0);

	if (total_send > 0)
	{
		CGuard guard(m_pLock, false);
		m_sendBuf.skip(total_send);
	}

	if (m_sendBuf.size() > 0)
	{
		this->setEvents(EPOLLIN | EPOLLOUT);
	}
	else
	{
		this->setEvents(EPOLLIN);
	}
	
	return (result >= 0);
}

bool CConnector::handleTimeout(uint32_t timerId)
{
	return true;
}

bool CConnector::handleClose()
{
	// handle reconnect
	close_tcp_socket(m_fd, true);
	m_fd = 0;
	
	m_sendBuf.clear();
	m_recvBuf.clear();
	m_pConnectorMgr->OnConnectorShutdown(this);
	LogErr("[%s#%u, ch#%u]handle connector close.",
			m_actorName.c_str(), m_svrId, m_channelId);

	return true;
}

bool CConnector::handleError(int events, mm::EventHandler *pHandler)
{
	if (pHandler == this)
	{
		LogDebug("[%s#%u, ch#%u]handle error, events %u.",
					m_actorName.c_str(), m_svrId, m_channelId, events);
	}
	return true;
}

bool CConnector::confirmDispose(void)
{
	return true;
}

int32_t CConnector::ProcessData(void)
{
	static uint32_t s_nMaxPkgSize = 64*1024;
	static uint32_t s_nRecordLen = sizeof(struct Record)+sizeof(MSG_HEAD);

	int32_t result(0);
	do {
		if (m_recvBuf.empty())
		{
			break;
		}
	
		if (m_recvBuf.size() < s_nRecordLen)
		{
			break;
		}
		struct Record record;
		struct MSG_HEAD head;
		m_recvBuf.SetPeekMode(true);
		m_recvBuf.Read((void*)&record, sizeof(record));
		m_recvBuf.Read((void*)&head, sizeof(head));
		m_recvBuf.SetPeekMode(false);
		record.Cast();
		head.Cast();
	
		if (head.protoLen > s_nMaxPkgSize)
		{
			LogFatal("ERR: [%s#%u, ch#%u]recv package size overflow,"
					"buffer size %u, protoId %u, protoLen %u",
					m_actorName.c_str(), m_svrId, m_channelId,
					m_recvBuf.size(), head.protoId, head.protoLen);
			result = -1;
			break;
		}
		uint32_t cbBuf = head.protoLen + sizeof(struct Record);
		if (m_recvBuf.size() < cbBuf)
		{
			break;
		}
	
		CTempBuffer buff(cbBuf);
		m_recvBuf.Read(buff.data(), cbBuf);
		int32_t ret(0);
		if (head.protoId == protoSysHeartBeat::IDD)
		{
			// 心跳协议，直接返回
			this->SendMsg(buff.data(), cbBuf);		
			continue;
		}

		try{
			ret = m_pConnectorMgr->OnConnectorRespone(
				this, record, buff.data(), cbBuf);
		}catch(...){
			LogFatal("ERR: occur error when connector respone data.");
			result = -1;
			break;
		}
		if (ret < 0)
		{
			LogFatal("ERR: exec connector respone data failure, ret=%d", ret);
			result = -1;
			break;
		}
	}while(true);

	return result;
}

int32_t CConnector::Authorize(void)
{
	protoSysAuthorize auth(m_actorSelf, m_svrId, m_channelId);
	//NOTE: token NOT used now
	
	char buffer[1024] = "";
	char *pData = (char*)&buffer[sizeof(struct Record)];
	size_t cbData = auth.getProtoLen();
	int32_t ret = auth.Serialize(pData, cbData);
	if (ret < 0)
	{
		LogFatal("[%s#%u, ch#%u]serialize protoAuthorize failure.",
				m_actorName.c_str(), m_svrId, m_channelId);
		return -1;
	}

	struct Record *pRecord = (struct Record*)&buffer[0];
	size_t cbBuf = sizeof(struct Record)+cbData;
	memcpy(pRecord->synccode, g_cSyncCode, sizeof(pRecord->synccode));
	pRecord->recordLen 	= cbBuf;
	pRecord->cmdId 		= protoSysAuthorize::IDD;
	pRecord->uin 		= 0;
	pRecord->sessionId 	= 0;
	pRecord->clientIp 	= 0;
	pRecord->flag 		= Record::FRM_ALL;
	pRecord->mask 		= Record::INNER_PROTOCOL;
	pRecord->Cast();

	this->SendMsg((void*)&buffer[0], cbBuf);
	LogDebug("begin to authorize connector");

	return 0;
}

