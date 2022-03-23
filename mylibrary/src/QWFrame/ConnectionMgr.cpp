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

#include "ConnectionMgr.h"


CConnectionMgr::CConnectionMgr()
{
	m_pLock = OpenRwLock();
}


CConnectionMgr::~CConnectionMgr()
{

}


bool CConnectionMgr::Init()
{
	return false;
}


bool CConnectionMgr::Run()
{
	CGuard guard(m_pLock, false);
	for (CONNECTOR_MAP::iterator iter = m_mapConnector.begin(); iter != m_mapConnector.end(); ++iter)
	{
		CConnectorSet *pConnector = iter->second;
		if (pConnector != NULL)
		{
			pConnector->Maintance();
		}
	}
	return true;
}

void CConnectionMgr::Finish()
{
	for (CONNECTOR_MAP::iterator iter = m_mapConnector.begin(); iter != m_mapConnector.end(); ++iter)
	{
		CConnectorSet *pConnectorSet = iter->second;
		if (pConnectorSet != NULL)
		{
			CConnector *pConnector = pConnectorSet->Find(0);
			if (pConnector != NULL)
			{
				pConnector->handleWrite();
				LogImpt("CConnectionMgr::Finish send data to svr actor %u success.", iter->first);
			}
		}
	}

	for (SESSIONSET_MAP::iterator iter = m_mapSession.begin(); iter != m_mapSession.end(); ++iter)
	{
		CSessionSet *pSessionSet = iter->second;
		if (pSessionSet == NULL)
		{
			continue;
		}

		SESSION_VEC vecSession;
		pSessionSet->QueryAll(vecSession);
		for (SESSION_VEC::iterator iterSession = vecSession.begin(); iterSession != vecSession.end(); ++iterSession)
		{
			CSession *pSession = *iterSession;
			if (pSession == NULL)
			{
				continue;
			}
			pSession->handleWrite();
		}
		LogImpt("CConnectionMgr::Finish send data to session actor %u success.", iter->first);
	}
}


void CConnectionMgr::AddAccept(uint32_t nActor, CAcceptor* pAcceptor)
{

}

void CConnectionMgr::AddPendingSession(uint32_t nActor, CSession* pSession)
{
	CGuard guard(m_pLock, false);
	SESSIONSET_MAP::iterator iter = m_mapSession.find(nActor);
	if (iter == m_mapSession.end())
	{
		CSessionSet *pSessionSet = new CSessionSet((Actor)nActor);
		if (pSessionSet == NULL)
		{
			LogFatal("CConnectionMgr::AddPendingSession new pSessionSet failure.");
			return;
		}
		pSessionSet->AddPending(pSession);
		m_mapSession.insert(make_pair(nActor, pSessionSet));
		return;
	}
	iter->second->AddPending(pSession);
	return;
}

void CConnectionMgr::AddSession(uint32_t nActor, uint32_t svrId, uint32_t channelId, const byte_t *pToken, CSession* pSession)
{
	CGuard guard(m_pLock, false);
	SESSIONSET_MAP::iterator iter = m_mapSession.find(nActor);
	if (iter == m_mapSession.end())
	{
		LogFatal("CConnectionMgr::AddPendingSession not found actor %u.", nActor);
		return;
	}
	iter->second->Add(svrId, channelId, pToken, pSession);
	return;
}


void CConnectionMgr::AddHttpSession(uint32_t sessionId, CSessionHttp* pSession)
{
	CGuard guard(m_pLock, false);
	HTTPSESSION_MAP::iterator iter = m_mapHttpSession.find(sessionId);
	if (iter != m_mapHttpSession.end())
	{
		LogFatal("CConnectionMgr::AddHttpSession sessionid [%u] exist.", sessionId);
		return;
	}
	m_mapHttpSession[sessionId] = pSession;
	return;
}

void CConnectionMgr::AddWebSocketSession(uint32_t sessionId, CSessionWebSocket* pSession)
{
	CGuard guard(m_pLock, false);
	WSSESSION_MAP::iterator iter = m_mapWsSession.find(sessionId);
	if (iter != m_mapWsSession.end())
	{
		LogFatal("CConnectionMgr::AddWebSocketSession sessionid [%u] exist.", sessionId);
		return;
	}
	m_mapWsSession[sessionId] = pSession;
	return;
}


void CConnectionMgr::AddConnector(uint32_t nActor, CConnectorSet* pConnector)
{
	CGuard guard(m_pLock, false);
	m_mapConnector[nActor] = pConnector;
}


void CConnectionMgr::RemoveAccept(CAcceptor* pAcceptor)
{
	
}


void CConnectionMgr::RemoveSession(CSession* pSession)
{
	CGuard guard(m_pLock, false);
	SESSIONSET_MAP::iterator iter = m_mapSession.find(pSession->getActor());
	if (iter == m_mapSession.end())
	{
		LogFatal("CConnectionMgr::RemoveSession not found actor %u.", pSession->getActor());
		return;
	}
	iter->second->Remove(pSession);
	return;
}


void CConnectionMgr::RemoveHttpSession(CSessionHttp* pSessionHttp)
{
	CGuard guard(m_pLock, false);
	HTTPSESSION_MAP::iterator iter = m_mapHttpSession.find(pSessionHttp->getSessionId());
	if (iter != m_mapHttpSession.end())
	{
		//删除 session
		//SAFE_DELETE(pSessionHttp);
		m_mapHttpSession.erase(iter);
	}
	return;
}

void CConnectionMgr::RemoveWebSocketSession(CSessionWebSocket* pSession)
{
	CGuard guard(m_pLock, false);
	WSSESSION_MAP::iterator iter = m_mapWsSession.find(pSession->getSessionId());
	if (iter != m_mapWsSession.end())
	{
		//删除 session
		//SAFE_DELETE(pSessionHttp);
		m_mapWsSession.erase(iter);
	}
	return;
}


void CConnectionMgr::RemoveConnector(CConnector* pConnector)
{
	CGuard guard(m_pLock, false);
	CONNECTOR_MAP::iterator iter = m_mapConnector.find(pConnector->getActor());
	if (iter == m_mapConnector.end())
	{
		LogErr("CConnectionMgr::RemoveConnector not found actor %u.", pConnector->getActor());
		return;
	}
	iter->second->Remove(pConnector);
	return;
}


bool CConnectionMgr::SendMsgToSession(Actor actor, uint32_t svrId, uint32_t channelId, void *pData, uint32_t cbData)
{
	CGuard guard(m_pLock, false);
	SESSIONSET_MAP::iterator iter = m_mapSession.find(actor);
	if (iter == m_mapSession.end())
	{
		LogErr("CConnectionMgr::SendMsgToSession not found actor %u.", actor);
		return false;
	}
	CSessionSet *pSessionSet = iter->second;
	CSession *pSession = NULL;
	if (channelId == 0)
	{
		pSession = pSessionSet->Find(svrId);
	}
	else
	{
		pSession = pSessionSet->Find(svrId, channelId);
	}

	if (pSession == NULL)
	{
		LogErr("CConnectionMgr::SendMsgToSession not found session svrid %u,channel id %u", svrId, channelId);
		return false;
	}

	pSession->SendMsg(pData, cbData);
	return true;
}


bool CConnectionMgr::SendMsgToSvr(Actor actor, void *pData, uint32_t cbData)
{
	CGuard guard(m_pLock, false);
	CONNECTOR_MAP::iterator iter = m_mapConnector.find(actor);
	if (iter == m_mapConnector.end())
	{
		LogErr("CConnectionMgr::SendMsgToSvr actor %u not found.", actor);
		return false;
	}

	CConnectorSet *pConnectorSet = iter->second;
	return pConnectorSet->SendMsg(pData, cbData);
}


bool CConnectionMgr::SendBroadcastMsg(Actor actor, void *pData, uint32_t cbData)
{
	CGuard guard(m_pLock, false);
	SESSIONSET_MAP::iterator iter = m_mapSession.find(actor);
	if (iter == m_mapSession.end())
	{
		LogErr("CConnectionMgr::SendBroadcastMsg not found actor %u.", actor);
		return false;
	}
	CSessionSet *pSessionSet = iter->second;

	SESSION_VEC vecSession;
	pSessionSet->QueryAll(vecSession);
	for (SESSION_VEC::iterator it = vecSession.begin(); it != vecSession.end(); ++it)
	{
		CSession *pSession = *it;
		if (pSession == NULL)
		{
			continue;
		}
		pSession->SendMsg(pData, cbData);
	}
	return true;
}


bool CConnectionMgr::SendHttpMsg(uint32_t sessionId, HttpRespData &respData)
{
	CGuard guard(m_pLock, false);
	HTTPSESSION_MAP::iterator iter = m_mapHttpSession.find(sessionId);
	if (iter == m_mapHttpSession.end())
	{
		LogErr2("HTTP", 0, "session id [%u] not found when resp msg.", sessionId);
		return false;
	}

	CSessionHttp *pSession = iter->second;
	if (pSession == NULL)
	{
		LogErr2("HTTP", 0, "session id [%u] obj is null.", sessionId);
		return false;
	}

	pSession->httpRespone(respData.code, respData.strStatus, respData.strResp, respData.vecHead);

	return true;
}


bool CConnectionMgr::SendWsMsg(uint32_t sessionId, void *pData, uint32_t cbData)
{
	CGuard guard(m_pLock, false);
	WSSESSION_MAP::iterator iter = m_mapWsSession.find(sessionId);
	if (iter == m_mapWsSession.end())
	{
		LogErr2("WS", 0, "session id [%u] not found when resp msg.", sessionId);
		return false;
	}

	CSessionWebSocket *pSession = iter->second;
	if (pSession == NULL)
	{
		LogErr2("WS", 0, "session id [%u] obj is null.", sessionId);
		return false;
	}

	pSession->SendMsg(pData, cbData);

	return true;
}

bool CConnectionMgr::CloseWsSession(uint32_t sessionId)
{
	CGuard guard(m_pLock, false);
	WSSESSION_MAP::iterator iter = m_mapWsSession.find(sessionId);
	if (iter == m_mapWsSession.end())
	{
		LogErr2("WS", 0, "session id [%u] not found when close session.", sessionId);
		return false;
	}

	CSessionWebSocket *pSession = iter->second;
	if (pSession == NULL)
	{
		LogErr2("HTTP", 0, "session id [%u] obj is null.", sessionId);
		return false;
	}

	pSession->OnClose();

	return true;
}

