/*
 * =====================================================================================
 *
 *       Filename:  AuthSession.h
 *
 *    Description:  for server connection
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 14时22分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_AUTHSVR_SESSION_H_
#define _QW_AUTHSVR_SESSION_H_

#include "BaseCode.h"
#include "MyNet.h"
#include "ISessionMgr.h"
#include "../Protocol/protoSysDef.h"

class CSession : public mm::EventHandler
{
public:
	CSession(enum Actor actor, const CMyString &actorName
			, ISessionMgr *pSessionMgr
			, int32_t fd, mm::Reactor &reactor
			, uint32_t ip, const CMyString &strIp);
	virtual ~CSession();

	bool Init(void);
	virtual void SendMsg(void *pBuf, size_t cbBuf);

	inline const enum Actor& getActor(void) { return m_actor; }
	inline const char* getActorName(void) 	{ return m_actorName.c_str(); }
	inline uint32_t    getSvrId(void)    	{ return m_svrId; }
	inline uint32_t    getId(void) 			{ return m_channelId; }
	inline const char* getIp(void)		  	{ return m_szIp; }
	inline uint32_t    getSessionId(void) { return m_nSessionId; }

public:
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(uint32_t timerId);
	virtual bool handleError(int events, mm::EventHandler *pHandler);

	virtual int32_t getSocket() const { return m_fd; }
	virtual bool handleClose();
	virtual bool confirmDispose(void);

public:
	void SetChannel(uint32_t svrId, uint32_t channelId);

private:
	int32_t process_data(void);
	int32_t adjust_protocol(void);

	int32_t OnAuthorize(struct Record &record, void *pBuf, uint32_t cbBuf);
	void OnHeartBeat(void);

protected:
	enum Actor			m_actor;
	CMyString			m_actorName;
	int32_t				m_fd;
	uint32_t			m_svrId;
	uint32_t			m_channelId;
	mm::Reactor&		m_reactor;
	ISessionMgr*		m_pSessionMgr;
	CByteStream			m_input;
	CByteStream			m_output;
	ILock*				m_pLock;

protected:
	uint32_t			m_ip;
	char				m_szIp[16];
	time_t				m_tActive;
	time_t				m_tAuthorize;
	enum State			m_eState;
	enum CloseReason	m_eReason;

	int32_t				m_pkgInput;		// recv的次数
	int32_t				m_pkgOutput;	// send的次数
	uint64_t			m_tLastRecv;
	uint64_t			m_tLastSend;
	int32_t				m_nHeartBeatCount;	// 连续下发心跳包次数

protected:
	uint32_t			m_timerZombie;
	uint32_t			m_timerAuth;
	uint32_t			m_timerHeartbeat;

	uint32_t			m_nSessionId;

public:
	static uint32_t  g_nSessionId;

};

#endif // _QW_AUTHSVR_SESSION_H_


