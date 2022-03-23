/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "QWFrame.h"
#include "NetworkHandle.h"

#include "QWFrameConfig.h"

#include "Common/Acceptor.h"
#include "Common/Session.h"
#include "Common/SessionHttp.h"
#include "Common/SessionSet.h"
#include "Common/Connector.h"
#include "Common/ConnectorSet.h"
#include "Common/SessionWebSocket.h"

CNetworkHandle::CNetworkHandle()
: m_pFrame(NULL)
{
	m_timerLoop = 0;
}

CNetworkHandle::~CNetworkHandle()
{

}

bool CNetworkHandle::Init(CQWFrame* pFrame)
{
	if (NULL == pFrame)
	{
		return false;
	}
	m_pFrame = pFrame;

	//网络连接管理
	m_objConnectMgr.Init();
	if (this->InitNetworkConnection() != 0)
	{
		return false;
	}
	
	//设置网络错误处理句柄
	this->setReactor(&m_reactor);
	m_reactor.addErrorHandler(this);

	return true;
}

void CNetworkHandle::Run()
{
	m_timerLoop = this->setTimer(5000);

	try
	{
		m_reactor.run();
	}
	catch (...)
	{
		LogFatal("gamesvr rum error.");
	}
}


/**********************************网络回调**************************************/
bool CNetworkHandle::handleRead()
{
	return true;
}

bool CNetworkHandle::handleWrite()
{
	return true;
}

bool CNetworkHandle::handleTimeout(unsigned int timerId)
{
	if (timerId == m_timerLoop)
	{
		m_objConnectMgr.Run();
		m_timerLoop = this->setTimer(5000);
	}
	else
	{
		LogErr("unknown timerId %u when catch timeout.", timerId);
	}

	return true;
}

bool CNetworkHandle::handleClose()
{
	return true;
}

bool CNetworkHandle::confirmDispose()
{
	return true;
}

bool CNetworkHandle::handleError(int events, EventHandler* handler)
{
	//处理连接断开等
	if (handler == this)
	{
		LogErr("ERR: acceptor occur error, closed.");
		return false;
	}
	LogMsg2("NETWORK", 0, "socket handle error.events[%d],", events);

	CSessionHttp *pSessionHttp = dynamic_cast<CSessionHttp*>(handler);
	if (pSessionHttp != NULL)
	{
		m_objConnectMgr.RemoveHttpSession(pSessionHttp);
		return true;
	}

	CSessionWebSocket *pSessionWs = dynamic_cast<CSessionWebSocket*>(handler);
	if (pSessionWs != NULL)
	{
		// 连接断开消息抛到上层让有需要的程序自行处理
		LogicReq req;
		req.actor = pSessionWs->getActor();
		req.svrId = pSessionWs->getSvrId();
		req.channelId = pSessionWs->getId();

		struct protoSysSessionShutdown protoShutdown;
		protoShutdown.svrId = pSessionWs->getSessionId();
		req.cbData = protoShutdown.getProtoLen();
		req.pData = calloc(sizeof(char), req.cbData + 8);
		protoShutdown.Serialize(req.pData, req.cbData);
		m_pFrame->GetLogicHandle().PushLogicReq(req);

		m_objConnectMgr.RemoveWebSocketSession(pSessionWs);
		return true;
	}


	CSession *pSession = dynamic_cast<CSession*>(handler);
	if (NULL != pSession)
	{
		// 连接断开消息抛到上层让有需要的程序自行处理
		LogicReq req;
		req.actor = pSession->getActor();
		req.svrId = pSession->getSvrId();
		req.channelId = pSession->getId();
		
		struct protoSysSessionShutdown protoShutdown;
		protoShutdown.actor = pSession->getActor();
		protoShutdown.svrId = pSession->getSvrId();
		protoShutdown.channelId = pSession->getId();
		req.cbData = protoShutdown.getProtoLen();
		req.pData = calloc(sizeof(char), req.cbData + 8);
		protoShutdown.Serialize(req.pData, req.cbData);
		m_pFrame->GetLogicHandle().PushLogicReq(req);

		m_objConnectMgr.RemoveSession(pSession);
		return true;
	}

	CConnector *pConn = dynamic_cast<CConnector*>(handler);
	if (NULL != pConn)
	{
		m_objConnectMgr.RemoveConnector(pConn);
		return true;
	}


	LogErr("ERR: unknown error, events %d, handler %p", events, handler);

	return true;
}

//session mgr*************************************************************************
bool CNetworkHandle::OnAcceptSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp)
{
	ListenCfg *pCfgListen = m_pFrame->GetFrameConfig().GetListenCfg(port);
	if (pCfgListen == NULL)
	{
		LogErr("CNetworkHandle::OnAcceptSck port %u not found in config. fd[%u], ip[%s]", port, fd, pszIp);
		return false;
	}

	CSession *pSession = new CSession((Actor)pCfgListen->nActorId, pCfgListen->strName.c_str(), this, fd, m_reactor, ip, pszIp);
	if (NULL == pSession)
	{
		LogFatal("CNetworkHandle::OnAcceptSck not enough memory to allocate CSession.");
		return false;
	}
	if (!pSession->Init())
	{
		SAFE_DELETE(pSession);
		return false;
	}

	m_objConnectMgr.AddPendingSession(pCfgListen->nActorId, pSession);
	LogMsg("CNetworkHandle::OnAcceptSck new session established.port[%u],fd[%u],ip[%s]", port, fd, pszIp);
	return true;
}

//session mgr*************************************************************************
bool CNetworkHandle::OnAcceptHttpSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp)
{
	ListenCfg *pCfgListen = m_pFrame->GetFrameConfig().GetListenCfg(port);
	if (pCfgListen == NULL)
	{
		LogErr("CNetworkHandle::OnAcceptSck port %u not found in config. fd[%u], ip[%s]", port, fd, pszIp);
		return false;
	}

	CSessionHttp *pSessionHttp = new CSessionHttp((Actor)pCfgListen->nActorId, pCfgListen->strName.c_str(), this, fd, m_reactor, ip, pszIp);
	if (NULL == pSessionHttp)
	{
		LogFatal("CNetworkHandle::OnAcceptSck not enough memory to allocate CSession.");
		return false;
	}
	if (!pSessionHttp->Init())
	{
		SAFE_DELETE(pSessionHttp);
		return false;
	}

	m_objConnectMgr.AddHttpSession(pSessionHttp->getSessionId(), pSessionHttp);
	LogMsg2("NETWORK:HTTP",0, "new http session established.port[%u],fd[%u],ip[%s]", port, fd, pszIp);
	return true;
}

int32_t	CNetworkHandle::OnSessionAuthorize(enum Actor actor, struct Record &record, void *pBuf, uint32_t cbBuf, CSession *pSession)
{
	LogMsg("CNetworkHandle::OnSessionAuthorize actor %d, cbBuf %u", actor, cbBuf);
	struct protoSysAuthorize auth_data;
	int32_t ret = auth_data.Deserialize(pBuf, cbBuf);
	if (ret < 0)
	{
		// protocol decode fault 
		LogFatal("[%s] actor %u Auth Fault: deserialize protoAuthorize failure.",
			pSession->getActorName(), static_cast<uint32_t>(actor));
		return -1;
	}

	uint32_t svrId = auth_data.serverId;
	uint32_t channelId = auth_data.channelId;
	const byte_t *pToken = auth_data.key;
	pSession->SetChannel(svrId, channelId);

	m_objConnectMgr.AddSession(actor, svrId, channelId, pToken, pSession);

	// 将认证抛到上层让有需要的程序自行处理
	cbBuf += sizeof(Record);
	char *pData = (char*)pBuf - sizeof(Record);
	this->OnRcvMsg(pSession, record, pData, cbBuf);

	return 0;
}

void CNetworkHandle::OnRcvMsg(CSession *pSession, struct Record &record, void *pBuf, uint32_t cbBuf)
{
	enum Actor actor = pSession->getActor();
	uint32_t svrId = pSession->getSvrId();
	uint32_t channelId = pSession->getId();

	char *pData = (char*)pBuf + sizeof(struct Record);
	uint32_t cbData = cbBuf - sizeof(struct Record);
	struct MSG_HEAD head;
	memcpy(&head, pData, sizeof(struct MSG_HEAD));
	head.Cast();
	switch (head.protoId)
	{
	default:
		{
			// deliver msg to each processor:
			LogicReq req;
			req.actor = actor;
			req.svrId = svrId;
			req.channelId = channelId;
			req.record = record;
			req.pData = calloc(sizeof(char), cbData + 8);
			memcpy(req.pData, pData, cbData);
			req.cbData = cbData;
			m_pFrame->GetLogicHandle().PushLogicReq(req);
		}
		break;
	}
	return;
}


void CNetworkHandle::OnRcvHttpMsg(CSessionHttp *pSession, http::CHttpRequest& pReq)
{
	HttpLogicReq req;
	req.nSessionId = pSession->getSessionId();
	req.httpData = pReq;

	m_pFrame->GetLogicHandle().PushHttpLogicReq(req);
	return;
}

bool CNetworkHandle::OnAcceptWebSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp)
{
	ListenCfg *pCfgListen = m_pFrame->GetFrameConfig().GetListenCfg(port);
	if (pCfgListen == NULL)
	{
		LogErr("CNetworkHandle::OnAcceptSck port %u not found in config. fd[%u], ip[%s]", port, fd, pszIp);
		return false;
	}

	CSessionWebSocket *pSession = new CSessionWebSocket((Actor)pCfgListen->nActorId, pCfgListen->strName.c_str(), this, fd, m_reactor, ip, pszIp);
	if (NULL == pSession)
	{
		LogFatal("CNetworkHandle::OnAcceptSck not enough memory to allocate CSession.");
		return false;
	}
	if (!pSession->Init())
	{
		SAFE_DELETE(pSession);
		return false;
	}

	m_objConnectMgr.AddWebSocketSession(pSession->getSessionId(), pSession);
	LogMsg2("NETWORK:HTTP", 0, "new http session established.port[%u],fd[%u],ip[%s]", port, fd, pszIp);
	return true;
}

void CNetworkHandle::OnRcvWebSocketMsg(CSession *pSession, uint32_t msgId, void *pBuf, uint32_t cbBuf)
{
	LogDebug("OnRcvWebSocketMsg, sessionid[%u], msgid[%u], data len[%u]", pSession->getSessionId(), msgId, cbBuf);
	WsLogicReq req;
	req.nSessionId = pSession->getSessionId();
	req.actor = pSession->getActor();
	req.svrId = pSession->getSvrId();
	req.channelId = pSession->getId();
	req.msgId = msgId;
	req.pData = calloc(sizeof(char), cbBuf + 8);
	memcpy(req.pData, pBuf, cbBuf);
	req.cbData = cbBuf;
	m_pFrame->GetLogicHandle().PushWsReq(req);

	return;
}

//connector mgr***********************************************************************
int32_t	CNetworkHandle::OnConnectorRespone(CConnector *pConn, struct Record &record, void *pBuf, uint32_t cbBuf)
{
	char *pData = (char *)pBuf + sizeof(struct Record);
	uint32_t cbData = cbBuf - sizeof(struct Record);
	if (record.cmdId == PROTO_SYS_HEARTBEAT_RESP)
	{
		pConn->SendMsg(pBuf, cbBuf);
		return 0;
	}

	LogicReq req;
	req.svrId = pConn->getSvrId();
	req.actor = pConn->getActor();
	req.channelId = 0;
	req.record = record;
	req.pData = calloc(sizeof(char), cbData + 8);
	memcpy(req.pData, pData, cbData);
	req.cbData = cbData;

	m_pFrame->GetLogicHandle().PushLogicReq(req);
	return 0;
}

void CNetworkHandle::OnConnectorShutdown(CConnector *pConn)
{
	return;
}

/**********************************业务处理**************************************/
int32_t CNetworkHandle::InitNetworkConnection()
{
	CQWFrameConfig& frameConfig = m_pFrame->GetFrameConfig();

	//帧听
	CQWFrameConfig::LISTEN_CFG_LIST& listenCfg = frameConfig.GetListenConfig();
	CQWFrameConfig::LISTEN_CFG_LIST::iterator itListen = listenCfg.begin();
	for (; itListen != listenCfg.end(); ++itListen)
	{
		ListenCfg& listen = itListen->second;

		bool bHttp = false;
		if (listen.strProto == "http")
		{
			bHttp = true;
		}

		CAcceptor* pAcceptor = new CAcceptor;
		if (pAcceptor->Init(this, listen.nPort, m_reactor, 1, listen.strProto))
		{
			m_objConnectMgr.AddAccept(listen.nActorId, pAcceptor);
		}
		else
		{
			delete pAcceptor;
			return -1;
		}
	}
	
	//连接
	CQWFrameConfig::CONNECTION_CFG_LIST&	connCfg = frameConfig.GetConnectionConfig();
	CQWFrameConfig::CONNECTION_CFG_LIST::iterator itConn = connCfg.begin();

	for (; itConn != connCfg.end(); ++itConn)
	{
		ConnectionCfg& conn = itConn->second;
		CConnectorSet* pConnectorSet = new CConnectorSet;

		bool bInit = pConnectorSet->Init(conn.nId,
										conn.nCount,
										(Actor)frameConfig.GetActorId(),
										(Actor) conn.nActorId,
										conn.strName,
										this, 
										&m_reactor,
										conn.strHost,
										conn.nPort);

		if (bInit)
		{
			m_objConnectMgr.AddConnector(conn.nActorId, pConnectorSet);
		}
		else
		{
			delete pConnectorSet;
		}
	}

	return 0;
}



