/*
 * =====================================================================================
 *
 *       Filename:  ConnectorSet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月13日 10时18分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include <algorithm>
#include "ConnectorSet.h"
#include "IConnectorMgr.h"
#include "../QWFrame.h"
CConnectorSet::CConnectorSet()
	: m_vecConnector()
	, m_mapConnector()
	, m_pLock(OpenRwLock())
	, m_lastChannelId(1)  
{
}

CConnectorSet::~CConnectorSet()
{
	{
		CGuard guard(m_pLock, false);
		m_vecConnector.clear();
		m_mapConnector.clear();
	}

	SAFE_DELETE(m_pLock);
}

CConnector* CConnectorSet::Find(uint32_t channelId)
{
	if (0 == channelId)
	{
		CGuard guard(m_pLock, true);
		if (m_vecConnector.empty()) return NULL;
		srand(time(0));
		int32_t index = rand() % m_vecConnector.size();
		return m_vecConnector.at(index);
	}

	CGuard gaurd(m_pLock, true);
	CONNECTOR_MAP::iterator it(m_mapConnector.find(channelId));
	if (it != m_mapConnector.end())
	{
		return it->second;
	}
	else
	{
		return this->Find(0);
	}
}

bool CConnectorSet::SendMsg(uint32_t channelId, void *pBuf, size_t cbBuf)
{
	CConnector *pConn = this->Find(channelId);
	if (NULL == pConn)
	{
		return false;
	}
	pConn->SendMsg(pBuf, cbBuf);
	return true;
}

bool CConnectorSet::SendMsg(void *pBuf, size_t cbBuf)
{
	return this->SendMsg(0, pBuf, cbBuf);
}

uint32_t CConnectorSet::Add(CConnector *pConn)
{
	CGuard guard(m_pLock, false);
	CONNECTOR_VEC::iterator it = std::find(
			m_vecConnector.begin(), m_vecConnector.end(), pConn);
	if (it != m_vecConnector.end())
	{
		return pConn->getId();
	}
	//pConn->setId(m_lastChannelId++);
	m_vecConnector.push_back(pConn);
	m_mapConnector[pConn->getId()] = pConn;
	return pConn->getId();
}

void CConnectorSet::Remove(uint32_t channelId)
{
	CGuard guard(m_pLock, false);
	CONNECTOR_MAP::iterator it (m_mapConnector.find(channelId));
	if (it == m_mapConnector.end())
	{
		return;
	}
	CConnector *pConn = it->second;
	m_mapConnector.erase(it);
	m_vecConnector.erase(
			std::remove(m_vecConnector.begin(), m_vecConnector.end(), pConn)
			, m_vecConnector.end());
}

void CConnectorSet::Remove(CConnector *pConn)
{
	uint32_t channelId = pConn->getId();

	CGuard guard(m_pLock, false);
	m_vecConnector.erase(
			std::remove(m_vecConnector.begin(), m_vecConnector.end(), pConn)
			, m_vecConnector.end());
	m_mapConnector.erase(channelId);	

	return;
}

bool CConnectorSet::Init(uint32_t svrId, uint32_t count,
			enum Actor actorSelf, enum Actor actor, const CMyString &actorName,
			IConnectorMgr *pMgr, mm::Reactor *pReactor,
			const CMyString &strHost, uint32_t port)
{
	m_svrId 	= svrId;
	m_count 	= count;
	m_actor 	= actor;
	m_actorName = actorName;
	m_pMgr 		= pMgr;
	m_pReactor 	= pReactor;
	m_strHost 	= strHost;
	m_port 		= port;
	m_actorSelf = actorSelf;

	int32_t ret = this->Maintance();
	if (ret < 0) return false;
	return true;
}

int32_t CConnectorSet::Maintance()
{
	if (m_vecConnector.size() >= m_count)
	{
		return 0;
	}

	int32_t count = m_count - m_vecConnector.size();

	for(int32_t index(0); index<count; ++index)
	{
		CConnector *pConn = new CConnector(m_svrId, m_lastChannelId++,
			m_actorSelf, m_actor, m_actorName,
			m_pMgr,
			m_strHost, m_port,
			*m_pReactor);
		if (NULL == pConn)
		{
			return -1;
		}
		int32_t ret = pConn->Init();
		if (ret < 0)
		{
			SAFE_DELETE(pConn);
			return -1;
		}
		this->Add(pConn);
	}

	return (int32_t)m_vecConnector.size();
}
