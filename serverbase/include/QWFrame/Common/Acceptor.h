/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.h
 *
 *    Description:  accept ProxySession
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 14时26分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */
#ifndef _QW_AUTHSVR_PROXYACCEPTOR_H_
#define _QW_AUTHSVR_PROXYACCEPTOR_H_

#include "BaseCode.h"
#include "MyNet.h"
#include "ISessionMgr.h"

#define SERVICE_REOPEN_EVENT		20000100


class CAcceptor : public mm::EventHandler
{
public:
	CAcceptor();
	~CAcceptor();

	bool Init(ISessionMgr *pSessionMgr, uint32_t port, mm::Reactor &reactor, uint32_t reopenId, string &strProto);

public:
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(unsigned int timerId);
	virtual bool handleError(int events, mm::EventHandler *pHandler);

	virtual int getSocket() const { return m_fd; }
	virtual bool handleClose();
	virtual bool confirmDispose(void);

private:
	bool Reopen(void);

private:
	int32_t				m_fd;
	uint32_t			m_port;
	mm::Reactor*		m_pReactor;

	ISessionMgr*		m_pSessionMgr;
	uint32_t			    m_reopenId;
	string				m_strProto;
};

#endif // _QW_AUTHSVR_PROXYACCEPTOR_H_

