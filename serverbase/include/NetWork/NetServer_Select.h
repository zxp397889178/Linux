/*
 * =====================================================================================
 *
 *       Filename:  NetServer_Select.h
 *
 *    Description:  net server by select
 *
 *        Version:  1.0
 *        Created:  2014年12月09日 10时18分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_NETSERVER_SELECT_H_
#define _MYLIB_NETWORK_NETSERVER_SELECT_H_

#include "BaseCode.h"
#include "INetServer.h"
#include "ClientObj.h"
#include <map>


class CNetServer_Select : public IServerSocket
{
	typedef std::map<int32_t, CClientObj*>	CLIENT_MAP;		
public:
	CNetServer_Select(IServerSocketEvent *pEvent);
	int32_t Open(uint16_t wPort);

public:
	virtual void release(void);
	virtual bool SendMsg(uint32_t nId, const void *pBuf, size_t cbBuf);
	virtual int32_t Process(void);
	virtual void CloseClient(int32_t nFd);
	virtual void RefuseNewConnect(bool bRefuse);

protected:
	~CNetServer_Select();

private:
	int32_t IssueAccept(void);
	int32_t IssueIO(void);
	int32_t ProcessData(void);

private:
	int						m_fd;		
	uint16_t				m_wPort;
	IServerSocketEvent*		m_pEvent;

	int32_t					m_clientId;
	ILock*					m_pLock;
	CLIENT_MAP				m_mapClient;
};

IServerSocket* OpenSelectSvr(IServerSocketEvent *pHandler, uint16_t wPort);


#endif 	
