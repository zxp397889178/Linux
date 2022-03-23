/*
 * =====================================================================================
 *
 *       Filename:  ClientObj.cpp
 *
 *    Description:  client object of NetServer
 *
 *        Version:  1.0
 *        Created:  2014年12月09日 21时04分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "ClientObj.h"
#include <sys/socket.h>
#include <unistd.h>

CClientObj::CClientObj(uint32_t clientId, int32_t fd, uint32_t ipRemote)
	: m_fd(fd)
	, m_clientId(clientId)
	, m_ipRemote(ipRemote)
	, m_timeCreate(::time(NULL))
	, m_timeLastActive(::time(NULL))
	, m_totalSend(0)  
	, m_totalRecv(0)
{

}

CClientObj::~CClientObj()
{

}

void CClientObj::AddInput(void *pBuf, size_t cbBuf)
{
	m_input.Write(pBuf, cbBuf);
	m_totalRecv += cbBuf;
}

void CClientObj::AddOutput(void *pBuf, size_t cbBuf)
{
	m_output.Write(pBuf, cbBuf);
}

