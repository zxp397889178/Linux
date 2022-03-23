/*
 * =====================================================================================
 *
 *       Filename:  ClientObj.h
 *
 *    Description:  ClientObject of NetServer
 *
 *        Version:  1.0
 *        Created:  2014年12月09日 20时57分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_NETWORK_CLIENT_OBJ_H_
#define _MYLIB_NETWORK_CLIENT_OBJ_H_

#include "BaseCode.h"
#include "INetServer.h"
#include <sys/types.h>
#include <sys/time.h>


class CClientObj : public IClientObj
{
public:
	CClientObj(uint32_t clientId, int32_t fd, uint32_t ipRemote);

	virtual void release(void) 		   { delete this; }
	virtual uint32_t getClientId(void) { return m_clientId; }
	virtual uint32_t getIpRemote(void) { return m_ipRemote; }
	virtual int32_t  getSocket(void)   { return m_fd; }

public:
	void AddInput(void *pBuf, size_t cbBuf);
	void AddOutput(void *pBuf, size_t cbBuf);

	CByteStream& getInput(void) { return m_input; }
	CByteStream& getOutput(void) { return m_output; }
	void registerSend(uint32_t bytes) { m_totalSend += bytes; }

	int64_t getTotalSend(void) { return m_totalSend; }
	int64_t getTotalRecv(void) { return m_totalRecv; }

protected:
	~CClientObj();

private:
	int32_t				m_fd;
	uint32_t			m_clientId;
	uint32_t			m_ipRemote;
	time_t				m_timeCreate;
	time_t				m_timeLastActive;

	CByteStream			m_input;
	CByteStream			m_output;

	int64_t				m_totalSend;
	int64_t				m_totalRecv;
};


#endif 	// _MYLIB_NETWORK_CLIENT_OBJ_H_
