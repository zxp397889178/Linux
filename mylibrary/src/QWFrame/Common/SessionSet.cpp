/*
 * =====================================================================================
 *
 *       Filename:  SessionSet.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月18日 18时41分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "SessionSet.h"
#include <algorithm>

CSessionSet::CSessionSet(enum Actor actor)
	: m_mapSession()
	, m_mapSessionVec()
	, m_actor(actor)  
{
	m_pLock = OpenRwLock();
}

CSessionSet::~CSessionSet()
{
	for(SESSION_MAP::iterator it(m_mapSession.begin());
			it != m_mapSession.end(); ++it)
	{
		CSession *pSession = it->second;
		SAFE_DELETE(pSession);
	}
	m_mapSession.clear();
	m_mapSessionVec.clear();

	SAFE_DELETE(m_pLock);
}

#define MAKE_UINT64(high, low)		(uint64_t)((((uint64_t)high)<<32) | (uint64_t)low)
void CSessionSet::Add(uint32_t svrId, uint32_t channelId, 
		const byte_t* pToken, CSession *pSession)
{
	CGuard guard(m_pLock, false, "add new session");

	uint64_t key = MAKE_UINT64(svrId, channelId);
	SESSION_MAP::iterator it(m_mapSession.find(key));
	if (it != m_mapSession.end())
	{
		// 存在相同的key,删除之
		CSession *pOldSession = it->second;
		if (pOldSession == pSession)
		{
			return;
		}
		LogWarn("WARN:[%s#%u, ch#%u]exist same map, erase old one.",
				pSession->getActorName(), svrId, channelId);
		pOldSession->setEvents(EPOLLHUP);

		m_mapSession.erase(it);
		// 移除映射表
		SESSIONVEC_MAP::iterator iter(m_mapSessionVec.find(svrId));
		if (iter != m_mapSessionVec.end())
		{
			SESSION_VEC &vecSession = iter->second;
			vecSession.erase(
				std::remove(vecSession.begin(), vecSession.end(), pOldSession)
				, vecSession.end());
		}
	}

	// 加入映射表
	m_mapSession[key] = pSession;
	m_mapSessionId[pSession->getSessionId()] = pSession;
	SESSIONVEC_MAP::iterator iter(m_mapSessionVec.find(svrId));
	if (iter == m_mapSessionVec.end())
	{
		SESSION_VEC vecSession;
		vecSession.push_back(pSession);
		m_mapSessionVec.insert(SESSIONVEC_MAP::value_type(svrId, vecSession));
	}
	else
	{
		SESSION_VEC &vecSession = iter->second;
		vecSession.push_back(pSession);
	}

	// 移除等待处理的session
	//this->RemovePending(pSession);
	m_setPending.erase(pSession);
}

CSession* CSessionSet::Find(uint32_t svrId)
{
	READ_GUARD(m_pLock);

	SESSIONVEC_MAP::iterator it(m_mapSessionVec.find(svrId));
	if (it == m_mapSessionVec.end())
	{
		return NULL;
	}
	SESSION_VEC &vecSession = it->second;
	if (vecSession.empty())
	{
		return NULL;
	}

	srand(time(NULL));
	uint32_t index = rand()%vecSession.size();
	return vecSession.at(index);
}

CSession* CSessionSet::Find(uint32_t svrId, uint32_t channelId)
{
	READ_GUARD(m_pLock);

	SESSIONVEC_MAP::iterator it(m_mapSessionVec.find(svrId));
	if (it == m_mapSessionVec.end())
	{
		return NULL;
	}
	SESSION_VEC &vecSession = it->second;
	if (vecSession.empty())
	{
		return NULL;
	}

	for (SESSION_VEC::iterator iter(vecSession.begin());
			iter != vecSession.end(); ++iter)
	{
		CSession *pSession = *iter;
		if (pSession == NULL)
		{
			continue;
		}
		if (pSession->getId() == channelId)
		{
			return pSession;
		}
	}

	// 找不到对应的session, 随意找一个
	LogWarn("svrId %u channelId %u not found session. select random session.", svrId, channelId);
	uint32_t cnt = (uint32_t)vecSession.size();
	if (cnt == 0)
	{
		return NULL;
	}
	uint32_t index = rand() % cnt;
	CSession *pSession = vecSession.at(index);
	return pSession;
}


CSession* CSessionSet::FindBySession(uint32_t sessionId)
{
	READ_GUARD(m_pLock);

	SESSIONID_MAP::iterator it(m_mapSessionId.find(sessionId));
	if (it == m_mapSessionId.end())
	{
		LogWarn("sessionid %u not found session. select random session.", sessionId);
		return NULL;
	}

	CSession *pSession = it->second;	
	return pSession;
}


void CSessionSet::Remove(CSession *pSession)
{
	uint32_t svrId = pSession->getSvrId();
	uint32_t channelId = pSession->getId();
	this->Remove(svrId, channelId);
}

void CSessionSet::Remove(uint32_t svrId, uint32_t channelId)
{
	CGuard guard(m_pLock, false, "remove session");
	
	uint64_t key = MAKE_UINT64(svrId, channelId);
	m_mapSession.erase(key);

	SESSIONVEC_MAP::iterator it(m_mapSessionVec.find(svrId));
	if (it != m_mapSessionVec.end())
	{
		SESSION_VEC &vecSession = it->second;
		for(SESSION_VEC::iterator iter(vecSession.begin()); 
				iter != vecSession.end(); )
		{
			CSession *pSession = *iter;
			if (NULL == pSession)
			{
				iter = vecSession.erase(iter);
				continue;
			}
			if (pSession->getId() == channelId)
			{
				iter = vecSession.erase(iter);

				// 移除等待处理状态的Session
				//this->RemovePending(pSession);
				m_setPending.erase(pSession);
				m_mapSessionId.erase(pSession->getSessionId());
				//delete pSession;
				continue;
			}
			++iter;
		}
	}
}

void CSessionSet::AddPending(CSession *pSession)
{
	CGuard guard(m_pLock, false, "add pending session.");
	m_setPending.insert(pSession);
}

void CSessionSet::QueryEach(SESSION_VEC &vecSession)
{
	READ_GUARD(m_pLock);

	srand(time(NULL));
	SESSIONVEC_MAP::iterator it(m_mapSessionVec.begin());
	for(; it != m_mapSessionVec.end(); ++it)
	{
		SESSION_VEC &tmpSession = it->second;
		if (tmpSession.empty())
		{
			continue;
		}
		uint32_t index = rand() % tmpSession.size();
		vecSession.push_back(tmpSession.at(index));
	}
}

void CSessionSet::QueryAll(SESSION_VEC &vecSession)
{
	READ_GUARD(m_pLock);

	SESSION_MAP::iterator it(m_mapSession.begin());
	for(; it != m_mapSession.end(); ++it)
	{
		vecSession.push_back(it->second);
	}
}

bool CSessionSet::Empty(void)
{
	READ_GUARD(m_pLock);
	if (m_mapSessionVec.empty())
	{
		return true;
	}
	for(SESSIONVEC_MAP::iterator it(m_mapSessionVec.begin());
			it != m_mapSessionVec.end(); ++it)
	{
		SESSION_VEC &vecSession = it->second;
		if (!vecSession.empty())
		{
			return false;
		}
	}

	return true;
}

CSession* CSessionSet::GetRandom(void)
{
	READ_GUARD(m_pLock);
	uint32_t cnt = static_cast<uint32_t>(m_mapSession.size());
	if (cnt == 0) return NULL;
	srand(time(0));
	uint32_t ind = rand()%cnt;
	uint32_t i(0);
	for(SESSION_MAP::iterator it(m_mapSession.begin());
			it != m_mapSession.end(); ++it, ++i)
	{
		if (i == ind)
		{
			CSession *pSession = it->second;
			return pSession;
		}
	}

	return NULL;
}

