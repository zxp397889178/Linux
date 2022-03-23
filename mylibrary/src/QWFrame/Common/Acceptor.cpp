/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 14时26分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "Acceptor.h"
#include "NetWork/NetFunc.h"
#include "../IpTables.h"


CAcceptor::CAcceptor()
	: m_fd(-1)
	, m_port(0)  
	, m_pReactor(NULL)  
	, m_pSessionMgr(NULL)  
	, m_reopenId(0)  
{
}

CAcceptor::~CAcceptor()
{
	this->setEvents(0);

	if (m_fd > 0)
	{
		close_tcp_socket(m_fd, true);
	}
	m_fd = -1;
}

bool CAcceptor::Init(ISessionMgr *pSessionMgr, uint32_t port, mm::Reactor &reactor, uint32_t reopenId, string &strProto)
{
	m_pSessionMgr = pSessionMgr;
	if (NULL == m_pSessionMgr)
	{
		LogFatal("ERR: Session Manager can not be NULL.");
		return false;
	}

	m_port = port;
	m_fd = open_tcp_socket(m_port, true, false, true); 
	if (m_fd <= 0)
	{
		LogFatal("ERR: can not open %u for service.", m_port);
		return false;
	}

	this->setReactor(&reactor);
	this->setEvents(EPOLLIN);
	m_pReactor = &reactor;
	m_reopenId += reopenId;
	LogInit("INIT: begin to listen at port %u, proto[%s]", m_port, strProto.c_str());

	m_strProto = strProto;
	return true;
}

bool CAcceptor::handleRead(void)
{
	struct sockaddr_in addr = {0};
	socklen_t addrLen = sizeof(addr);
	int32_t fd = ::accept(m_fd, (struct sockaddr*)&addr, &addrLen);
	if (-1 == fd)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return true;
		}
		LogErr("ERR:port %u occur error [%d] %s", m_port, errno, strerror(errno));
		return false;
	}
	if (0 == fd)
	{
		return true;
	}

	int32_t flags = fcntl(fd, F_GETFL);
	int32_t ret = fcntl(fd, F_SETFL, flags|O_NONBLOCK);
	if (ret < 0)
	{
		LogFatal("set socket to NON-BLOCK failure.");
	}
	//set_sck_nonblock(fd);

	uint32_t ip = addr.sin_addr.s_addr;
	char szIp[64] = "";
	ip2str(ip, szIp, sizeof(szIp));
	
	/*
	if (!RECHARGE_CONFIG.IsIpValid(ip))
	{
		close_tcp_socket(fd, true);
		LogWarn("WARN: [port %u]ip %s not allowed.", m_port, szIp);
		return true;
	}
	*/

	if (m_strProto == "http")
	{
		if (!m_pSessionMgr->OnAcceptHttpSck(m_port, fd, ip, szIp))
		{
			close_tcp_socket(fd, true);
			LogWarn("WARN: [port %u]ip %s not allow established.", m_port, szIp);
			return true;
		}
	}
	else if (m_strProto == "ws")
	{
		if (!m_pSessionMgr->OnAcceptWebSck(m_port, fd, ip, szIp))
		{
			close_tcp_socket(fd, true);
			LogWarn("WARN: [port %u]ip %s not allow established.", m_port, szIp);
			return true;
		}
	}
	else
	{
		if (!m_pSessionMgr->OnAcceptSck(m_port, fd, ip, szIp))
		{
			close_tcp_socket(fd, true);
			LogWarn("WARN: [port %u]ip %s not allow established.", m_port, szIp);
			return true;
		}
	}
	
	LogDebug("[port %u]ip %s connected.", m_port, szIp);

	return true;
}

bool CAcceptor::handleWrite(void)
{
	LogErr("ERR: port %u only accept socket.", m_port);
	return false;
}

bool CAcceptor::handleTimeout(unsigned int timerId)
{
	if (timerId == m_reopenId)
	{
		this->Reopen();
	}

	return true;
}

bool CAcceptor::handleError(int events, mm::EventHandler *pHandler)
{
	if (pHandler == this)
	{
		LogErr("ERR:occur error, events [%u]", events);
		return false;
	}

	return true;
}

bool CAcceptor::handleClose()
{
	//close_tcp_socket(m_fd, true);
	//m_fd = -1;

	//m_reopenId = this->setTimer(1000);
	//LogFatal("will reopen port %u after 1000ms", m_port);
	LogImpt("handleClose: socket error, reopen port [%d]", m_port);
	this->Reopen();

	return true;
}

bool CAcceptor::confirmDispose(void) 
{ 
	// DO NOT DELETE this EventHandler
	return false; 
}

bool CAcceptor::Reopen(void)
{
	//关闭连接
	close_tcp_socket(m_fd, true);
	nsleep(10);

	m_fd = open_tcp_socket(m_port, true, false, true); 
	if (m_fd <= 0)
	{
		LogFatal("can not open %u for service.", m_port);
		//m_reopenId = this->setTimer(1000);
		return false;
	}

	this->setReactor(m_pReactor);
	this->setEvents(0);
	this->setEvents(EPOLLIN);
	LogImpt("RE-OPEN port %u for service succeed", m_port);

	return true;
}



