/*
 * =====================================================================================
 *
 *       Filename:  SessionSet.h
 *
 *    Description:  session set
 *
 *        Version:  1.0
 *        Created:  2015年09月18日 18时39分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_GAMESVR_SESSION_SET_H_
#define _QW_CRAZYFISH_GAMESVR_SESSION_SET_H_

#include <vector>
#include <map>
#include <set>
#include "BaseCode.h"
#include "Session.h"

typedef std::vector<CSession*>				SESSION_VEC;
class CSessionSet
{
	typedef std::map<uint64_t, CSession*>		SESSION_MAP;	// uint64_t(svrId, channelId) <--> CSession*
	typedef std::map<uint32_t, SESSION_VEC>		SESSIONVEC_MAP; // svrId <--> SESSION_VEC
	typedef std::map<uint32_t, CSession*>		SESSIONID_MAP;	// sessionid <--> CSession*

	typedef std::set<CSession*>					SESSION_SET;
public:
	explicit CSessionSet(enum Actor actor);
	~CSessionSet();

	// 判断是否没有任何连接
	bool Empty(void);
	// 获得随机的连接
	CSession* GetRandom(void);

	void AddPending(CSession *pSession);

	void Add(uint32_t svrId, uint32_t channelId, const byte_t* pToken,
			CSession *pSession);
	void Remove(CSession *pSession);
	void Remove(uint32_t svrId, uint32_t channelId);

	CSession* Find(uint32_t svrId);
	CSession* Find(uint32_t svrId, uint32_t channelId);
	CSession* FindBySession(uint32_t sessionId);

	// 从每个svrId中获取一个随机的连接
	void QueryEach(SESSION_VEC &vecSession);
	// 取出全部连接
	void QueryAll(SESSION_VEC &vecSession);

private:
	SESSION_MAP		m_mapSession;
	SESSIONVEC_MAP	m_mapSessionVec;
	SESSION_SET		m_setPending;

	SESSIONID_MAP	m_mapSessionId;

	enum Actor		m_actor;
	ILock*			m_pLock;
};


#endif // _QW_CRAZYFISH_GAMESVR_SESSION_SET_H_


