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

#include "Session.h"
#include "NetWork/NetFunc.h"
#include "../Protocol/protoSystem.h"

static const int32_t s_nHeartBeat 	  = 5*1000;		// 心跳超时设置
static const int32_t s_nMaxHeartCount = 3;			// 心跳检查最大次数
static const int32_t s_nZombieTimeout = 1000;		// zombie connect
static const int32_t s_nAuthTimeout   = 1000;		// authorize timeout


uint32_t CSession::g_nSessionId = 0;

CSession::CSession(enum Actor actor, const CMyString &actorName
	, ISessionMgr *pSessionMgr, int32_t fd, mm::Reactor &reactor
		, uint32_t ip, const CMyString &strIp)
	: m_actor(actor)
	, m_actorName(actorName)  
	, m_fd(fd)
	, m_svrId(0)  
	, m_channelId(0)  
	, m_reactor(reactor)  
	, m_pSessionMgr(pSessionMgr)  
	, m_input()
	, m_output()  
	, m_ip(ip)
	, m_eState(stateInit)  
	, m_eReason(closeUnknown)  
{
	safe_strcpy(m_szIp, sizeof(m_szIp), strIp.c_str());

	m_tActive = ::time(0); 
	m_tAuthorize = 0;

	m_tLastRecv = 0;
	m_tLastSend = 0;
	m_pkgInput = 0;
	m_pkgOutput = 0;
	m_nHeartBeatCount = 0;

	m_pLock = OpenRwLock();
	{
		CGuard guar(m_pLock, false);
		m_nSessionId = (++CSession::g_nSessionId);
	}

	m_timerZombie = 0;
	m_timerAuth = 0;
	m_timerHeartbeat = 0;
}

CSession::~CSession()
{
	if (m_timerZombie != 0) this->cancelTimer(m_timerZombie);
	if (m_timerAuth != 0) this->cancelTimer(m_timerAuth);
	if (m_timerHeartbeat != 0) this->cancelTimer(m_timerHeartbeat);


	if (m_fd > 0)
	{
		close_tcp_socket(m_fd, true);
	}

	this->setEvents(0);
	m_fd = 0;
	SAFE_DELETE(m_pLock);
	char szTime[64] = "";
	TimeToStr(m_tActive, szTime, sizeof(szTime));
	LogMsg("[%s#%u ch#%u]shutdown connection, state %u, reason %u, ip %s, connect at %s",
		m_actorName.c_str(), m_svrId, m_channelId, 
		static_cast<uint32_t>(m_eState), static_cast<uint32_t>(m_eReason), m_szIp, szTime);
}

bool CSession::Init(void)
{
	this->setReactor(&m_reactor);
	this->setEvents(EPOLLIN);
	m_timerZombie = this->setTimer(s_nZombieTimeout);

	LogMsg("new %s session init.", m_actorName.c_str() );
	return true;
}

bool CSession::handleRead(void)
{
	if (m_eState == stateInit)
	{
		m_eState = stateActive;
		this->cancelTimer(m_timerZombie); 
		m_timerZombie = 0;
		m_timerAuth = this->setTimer(s_nAuthTimeout);
	}

	char buf[8192] = "";
	//接收全部数据
	do {
		ssize_t n = recv(m_fd, buf, sizeof(buf), 0);
		if (0 == n)
		{
			//连接已关闭
			m_eReason = closePassive;
			return false;
		}
		else if (-1 == n )
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
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
		// 发送数据，并统计
		m_input.Write((const void*)buf, bufLen);
		//IPMON.OnNetReceive(m_ip, n);
		LogTrace("[%s#%d ch#%u]recv %ld bytes, input pkgIndex %d",
					m_actorName.c_str(), m_svrId, m_channelId, n, m_pkgInput);
	} while(0);

	if (this->process_data() < 0)
	{
		return false;
	}

	return true;
}

bool CSession::handleWrite(void)
{
	CByteStream output;
	{
		CGuard guar(m_pLock, false);
		output.Write(m_output);
	}
	//发送数据到客户端
	int32_t result(0);
	while(!output.empty())
	{
		size_t nLen = output.size();
		CTempBuffer buff(nLen);
		if (!output.Read(buff.data(), nLen))
		{
			//从内存中读取失败，重做
			continue;
		}

		// 对输出数据进行转换，主要是加密
		// this->process_output(pBuf, nLen);

		// 发送数据
		int32_t ret = send_data(m_fd, buff.data(), nLen);
		if (ret < 0)
		{
			LogErr("ERR:[%s#%u ch#%u]call send_data(), socket occur error [%d] %s", 
					m_actorName.c_str(), m_svrId, m_channelId, errno, strerror(errno));
			result = -1;
			if (-2 == ret)
				m_eReason = closePassive;
			else 
				m_eReason = closeNetFault;
			break;
		}

		++m_pkgOutput;
		m_tLastSend = Now();
		//IPMON.OnNetSend(m_ip, ret);
		LogTrace("[%s#%u ch#%u]send %d bytes, output pkgIndex %u."
					, m_actorName.c_str(), m_svrId, m_channelId, ret, m_pkgOutput);
		result += ret;
		break;
	}

	if (result < 0)
	{
		// 出错了，直接断开连接
		return false;
	}

	// 处理完毕，移除已发送缓存
	if (result > 0)
	{
		CGuard guard(m_pLock, false);
		m_output.skip(result);
	}

	{
		CGuard guard(m_pLock, false);
		if (m_output.size() > 0)
		{
			this->setEvents(EPOLLIN | EPOLLOUT);
		}
		else
		{
			this->setEvents(EPOLLIN);
		}
	}
	
	
	return (result>=0);
}

bool CSession::handleTimeout(uint32_t timerId)
{
	int32_t result(0);
	if (timerId == m_timerZombie)
	{
		m_eReason = closeZombie;
		LogWarn("Warn: connection is zombie");
		result = -1;
	}
	else if (timerId == m_timerAuth)
	{
		m_eReason = closeAuthTimeout;
		LogWarn("Warn: connection authorize timeout");
		result = -1;
	}
	else if (timerId == m_timerHeartbeat)
	{
		LogTrace("connection try heartbeat");
		if (m_nHeartBeatCount < s_nMaxHeartCount)
		{
			this->OnHeartBeat();
			m_timerHeartbeat = this->setTimer(s_nHeartBeat);
		}
		else
		{
			// 心跳超时
			LogErr("ERR: [%s#%u, ch#%u]heartbeat timeout, times %d.",
				m_actorName.c_str(), m_svrId, m_channelId, m_nHeartBeatCount);
			m_eReason = closeHeartBeat;
			result = -1;
		}
	}
	else
	{
		LogErr("ERR: unkonwn timerId %u when session catched.", timerId);
	}

	return (result >= 0);
}

bool CSession::handleError(int events, mm::EventHandler *pHandler)
{
	return false;
}

bool CSession::handleClose()
{
	return true;
}

bool CSession::confirmDispose(void)
{
	return true;
}

int32_t CSession::process_data(void)
{
	static uint32_t		s_nMaxPkgSize = 16*1024;	// 16KB

	struct Record record;
	struct MSG_HEAD head;
	int32_t ret(0);

	do {
		if (m_input.size() < sizeof(struct Record) + sizeof(struct MSG_HEAD))
		{
			// 包文不完整，继续等待
			break;
		}

		// 预取出数据，用于判断是否完整包
		m_input.SetPeekMode(true);
		m_input.Read(&record, sizeof(record));
		m_input.Read(&head, sizeof(head));
		m_input.SetPeekMode(false);
		record.Cast();
		head.Cast();

		if (head.protoLen > s_nMaxPkgSize)
		{
			// 协议乱了，想办法从record中恢复
			uint32_t buffSize = m_input.size();
			ret = this->adjust_protocol();
			LogFatal("[%s#%u, ch#%u]protocol currpted, protoId [%u], protoLen [%u] "
					", origin buffer size %u, adjust result [%d], after adjust buffer size %u",
					m_actorName.c_str(), m_svrId, m_channelId, 
					head.protoId, head.protoLen, buffSize, ret, m_input.size());
			if (ret < 0) break;
			continue;
		}
		if (m_input.size() < (sizeof(struct Record) + head.protoLen))
		{
			// 包文不完整，继续等待
			LogTrace("===input buffer [%lu], protoLen [%u], record len [%lu]",
					m_input.size(), head.protoLen, sizeof(struct Record));
			break;
		}

		// 包文完整，逐个取出处理
		size_t cbBuf = head.protoLen + sizeof(struct Record);
		CTempBuffer buff(cbBuf);
		m_input.Read((void*)buff.data(), cbBuf);
		//LogErr("begin to handle message, buffer size %u, cmdId %u, state %d, actor %d",
				//cbBuf, record.cmdId, m_eState, m_actor);
		if (m_eState == stateAuthorize)
		{
			if (record.cmdId == protoSysHeartBeat::IDD)
			{
				LogTrace("[%s#%u, ch#%u]recv heartbeat resp.", m_actorName.c_str(), m_svrId, m_channelId);
				m_nHeartBeatCount = 0;
			}
			else
			{
				struct timeval tv;
                gettimeofday(&tv, NULL);
				record.mask = (tv.tv_sec %86400)*1000 + tv.tv_usec/1000;
				m_pSessionMgr->OnRcvMsg(this , record, buff.data(), cbBuf);
			}
			ret = 0;
		}
		else
		{
			char *pData = buff.data() + sizeof(struct Record);
			LogImpt("begin to authorize, data [%p], cmdId %u datalen %u", pData, head.protoId, head.protoLen);
			ret = this->OnAuthorize(record, pData, head.protoLen);
		}
		if (ret < 0)
		{
			break;
		}
	}while(!m_input.empty());

	if (ret < 0)
	{
		// 出错了，不再接收新消息
		this->setEvents(EPOLLOUT);
		return -1;
	}

	{
		CGuard guard(m_pLock, false);
		if (m_output.empty())
		{
			this->setEvents(EPOLLIN);
		}
		else
		{
			this->setEvents(EPOLLIN | EPOLLOUT);
		}
	}
	

	return 1;
}

void CSession::SendMsg(void *pBuf, size_t cbBuf)
{
	CGuard guard(m_pLock, false);
	m_output.Write(pBuf, cbBuf);

	this->setEvents(EPOLLOUT);
}

int32_t CSession::OnAuthorize(struct Record &record, void *pBuf, uint32_t cbBuf)
{
	int32_t ret = m_pSessionMgr->OnSessionAuthorize(m_actor, record, pBuf, cbBuf, this);
	if (ret < 0)
	{
		LogErr("%s auth fault.", m_actorName.c_str());
		m_eReason = closeAuthFault;
		return -1;
	}

	//protoSysAuthorizeResp resp(m_actor, m_svrId, m_channelId);
	//resp.result = 1;
	//uint32_t protoLen = resp.getProtoLen();
	//CServerMsgBuffer tmpBuf(protoLen);
	//tmpBuf.BuildRecord(protoLen, protoSysAuthorizeResp::IDD, 0, 0, 0);
	//ret = resp.Serialize(tmpBuf.getProtoData(), tmpBuf.getProtoLen());
	//if (ret < 0)
	//{
	//	LogErr("serialize protoServerAuthorizeResp failure, ret=%d", ret);
	//	return 0;
	//}
	//this->SendMsg(tmpBuf.getBuffer(), tmpBuf.getBufferLen());

	m_eState = stateAuthorize;
	this->cancelTimer(m_timerAuth);
	m_timerHeartbeat = this->setTimer(s_nHeartBeat);
	LogImpt("[%s#%u, ch#%u]authorized ok",
			m_actorName.c_str(), m_svrId, m_channelId);
	return 0;	
}

int32_t CSession::adjust_protocol(void)
{
	// 自动校正protocol，依据Record中的校验码，
	// 校验成功，则跳过指定的字节；返回-1表示校验失败
	// TODO
	
	return -1;
}

void CSession::OnHeartBeat(void)
{
	//if (Now() < m_tLastRecvHeartBeat + s_nHeartBeat)
	//{
	//	// 时间未到，不予处理心跳
	//	return;
	//}

	char szHeart[512] = "";
	struct protoSysHeartBeat heartbeat(m_actor);
	size_t protoLen = heartbeat.getProtoLen();
	void *pBuf= (void*)&szHeart[sizeof(Record)];
	int32_t ret = heartbeat.Serialize(pBuf, protoLen);
	if (ret < 0)
	{
		LogErr("ERR: [%S#%u, ch#%u]packed protoHeartBeat failure.",
			m_actorName.c_str(), m_svrId, m_channelId);
		return;
	}

	struct Record *pRecord = (struct Record*)&szHeart[0];
	memcpy(&pRecord->synccode[0], &g_cSyncCode[0], sizeof(pRecord->synccode));
	size_t bufLen = sizeof(struct Record)+protoLen;
	pRecord->recordLen 	= bufLen;
	pRecord->cmdId 		= protoSysHeartBeat::IDD;
	pRecord->uin 		= 0;	// NO USE
	pRecord->sessionId 	= 0;	// NO USE
	pRecord->clientIp 	= 0;	// NO USE
	pRecord->flag 		= Record::FRM_ALL;
	pRecord->mask 		= Record::INNER_PROTOCOL;
	pRecord->Cast();

	this->SendMsg((void*)&szHeart[0], bufLen);
	++m_nHeartBeatCount;
	LogTrace("[%s#%u, ch#%u]request heartbeat, times %d.",
			m_actorName.c_str(), m_svrId, m_channelId, m_nHeartBeatCount);
}

void CSession::SetChannel(uint32_t svrId, uint32_t channelId)
{
	m_svrId = svrId;
	m_channelId = channelId;
}



