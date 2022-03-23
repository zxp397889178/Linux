/*
 * =====================================================================================
 *
 *       Filename:  NetServer_Select.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月09日 10时18分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "NetServer_Async.h"
#include <unistd.h>			
#include <sys/types.h>
#include <sys/socket.h>		
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "NetFunc.h"
#include "BaseCode.h"


CNetServer_Async::CNetServer_Async(IServerSocketEvent *pEvent)
	: m_fd(-1)
	, m_wPort(0)
	, m_pEvent(pEvent)
	, m_clientId(0)
	, m_pLock(NULL)
{
	m_pLock = OpenRwLock();
	assert(m_pEvent != NULL);
	signal(SIGPIPE, SIG_IGN);
}

CNetServer_Async::~CNetServer_Async()
{
	
	SAFE_RELEASE(m_pEvent);

	
	m_pLock->Lock(false);
	for(CLIENT_MAP::iterator it(m_mapClient.begin());
			it != m_mapClient.end(); ++it)
	{
		int32_t fd = it->first;
		CClientObj *pObj = it->second;
		if (NULL != pObj)
		{
			LogTrace("NET:client %u total send %ld bytes, total recv %ld bytes.",
					fd, pObj->getTotalSend(), pObj->getTotalRecv());
		}
		close_tcp_socket(fd, true);
		SAFE_RELEASE(pObj);
	}
	m_mapClient.clear();
	m_pLock->Unlock();

	
	close_tcp_socket(m_fd, true);

	
	SAFE_DELETE(m_pLock);
}

int32_t CNetServer_Async::Open(uint16_t wPort)
{
	int32_t ret = open_tcp_socket(wPort, true, false, true);
	if (-1 == ret)
	{
		return -1;
	}
	m_fd = ret;
	return 0;
}

void CNetServer_Async::release(void)
{
	delete this;
}

bool CNetServer_Async::SendMsg(uint32_t nId, const void *pBuf, size_t cbBuf)
{
	CGuard guard(m_pLock, false);
	CLIENT_MAP::iterator it(m_mapClient.find(nId));
	if (it == m_mapClient.end())
	{
		return false;
	}

	CClientObj *pClient = it->second;
	if (NULL==pClient)
	{
		return false;
	}

	pClient->AddOutput(const_cast<void*>(pBuf), cbBuf);

	return true;
}


int32_t CNetServer_Async::Process(void)
{
	int32_t ret(-1);
	int32_t result(0);

	

	int32_t accept_count(5);
	while(accept_count > 0)
	{
		ret = this->IssueAccept();
		if (ret <= 0) break;
		++result;
		--accept_count;
	}

	if (this->IssueIO() > 0)
	{
		++result;
	}

	if (this->ProcessData())
	{
		++result;
	}
	
	

	return result;
}

int32_t CNetServer_Async::ProcessData(void)
{
	int32_t result(0);

	CLIENT_MAP mapClient;
	m_pLock->Lock(true);
	mapClient = m_mapClient;
	m_pLock->Unlock();

	for(CLIENT_MAP::iterator it(mapClient.begin()); 
			it != mapClient.end(); ++it)
	{
		int32_t fd = it->first;
		CClientObj *pObj = it->second;
		if (NULL == pObj)
		{
			this->CloseClient(fd);
			continue;	
		}

		CByteStream &buff = pObj->getInput();
		if (!buff.empty())
		{
			++result;
			if (NULL != m_pEvent)
			{
				size_t cbData = buff.size();
				void *pData = calloc(sizeof(char), cbData+10);
				buff.SetPeekMode(true);
				buff.Read(pData, cbData);
				buff.SetPeekMode(false);
				int32_t ret = m_pEvent->OnRcvMsg(fd, pData, cbData);
				free(pData);
				if (ret < 0)
				{
					this->CloseClient(fd);
					continue;
				}
				else if (ret > 0)
				{
					buff.skip(ret);
				}
			}
			else
			{
				buff.clear();
			}
		}
	}

	return result;
}

void CNetServer_Async::CloseClient(int32_t nFd)
{
	int32_t fd(0);
	m_pLock->Lock(false);
	CLIENT_MAP::iterator it(m_mapClient.find(nFd));
	if (it != m_mapClient.end())
	{
		fd = it->first;
		CClientObj *pObj = it->second;
		if (NULL != pObj)
		{
			LogTrace("NET:client %u total send %ld bytes, total recv %ld bytes.",
					fd, pObj->getTotalSend(), pObj->getTotalRecv());
		}
		SAFE_RELEASE(pObj);
		m_mapClient.erase(it);
	}
	m_pLock->Unlock();
	close_tcp_socket(nFd, true);
}

void CNetServer_Async::RefuseNewConnect(bool bRefuse)
{
	
}

int32_t CNetServer_Async::IssueAccept(void)
{
	static int32_t s_sndbuf = 4*1024*1024;
	static int32_t s_rcvbuf = 4*1024*1024;

	int32_t result(0);
	struct sockaddr_in remote_addr;
	socklen_t socklen = (socklen_t)sizeof(remote_addr);
	int32_t fd = accept(m_fd, (struct sockaddr*)&remote_addr, &socklen);
	if (-1 == fd)
	{
		int32_t err = WSAGetLastError(m_fd);
		if (EAGAIN != err)
		{
			result = -1;
			LogFatal("accept socket occur error %d, %s", errno, strerror(errno));
		}
		return result;
	}

#ifdef OS_WIN32
	
	setsockopt(fd, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_fd, sizeof(m_fd));
#endif

	
	uint32_t ipRemote = remote_addr.sin_addr.s_addr;
	if (NULL != m_pEvent)
	{
		if (!m_pEvent->OnAcceptSck(ipRemote))
		{
			close_tcp_socket(fd, true);
			return 0;
		}
	}

	
	set_sck_nonblock(fd);
	set_sock_rcvbuf(fd, s_rcvbuf);
	set_sock_sndbuf(fd, s_sndbuf);

	uint32_t clientId = ++m_clientId;
	
	CClientObj *pClient = new CClientObj(clientId, fd, ipRemote);
	if (NULL == pClient)
	{
		close_tcp_socket(fd, true);
		return 0;
	}
			
	result = 1;
	
	m_pLock->Lock(false);
	m_mapClient[fd] = pClient;
	m_pLock->Unlock();

	if (NULL != m_pEvent)
	{
		m_pEvent->OnEstablishSck(fd, ipRemote);
	}
	return result;
}

int32_t CNetServer_Async::IssueIO(void)
{
	int32_t result(0);

	m_pLock->Lock(true);
	CLIENT_MAP mapClient = m_mapClient;
	m_pLock->Unlock();

	char buffer[10240] = "";
	for(CLIENT_MAP::iterator it(mapClient.begin());
			it != mapClient.end(); ++it) 
	{
		int32_t fd = it->first;
		CClientObj* pClient = it->second;
		if (NULL == pClient)
		{
			continue;
		}

		
		uint32_t ipRemote = pClient->getIpRemote();

		
		int32_t max_rcv_times(1);
		int32_t ret(0);
		do {
			ret = recv(fd, buffer, sizeof(buffer), 0);
			if (ret>0)
			{
				
				pClient->AddInput(buffer, ret);
				++result;
			}
			else if (ret==0)
			{
				LogErr("NETWARN: remote host shutdown connection, error %d [%s]", errno, strerror(errno));
				if (NULL != m_pEvent)
				{
					m_pEvent->OnCloseSck(fd, ipRemote, true);
				}
				max_rcv_times = -1;
				this->CloseClient(fd);
				break;
			}
			else if (-1==ret)
			{
				int err = WSAGetLastError(fd);
				if (err==EAGAIN || err == EWOULDBLOCK)
				{
					break;
				}

				LogErr("NETERR: occur error when recv, error: %d, %s", err, strerror(err));
				if (NULL != m_pEvent)
				{
					m_pEvent->OnCloseSck(fd, ipRemote, true);
				}
				max_rcv_times = -1;
				this->CloseClient(fd);
				break;
			}
			--max_rcv_times;
		}while(ret>0 && max_rcv_times>0);

		
		m_pLock->Lock(false);
		CByteStream buff = pClient->getOutput();
		m_pLock->Unlock();

		if (!buff.empty())
		{
			size_t cbBuf = buff.size();
		    void *pBuf = calloc(sizeof(char), cbBuf+10);

			buff.SetPeekMode(true);
			buff.Read(pBuf, cbBuf);
			buff.SetPeekMode(false);
			++result;

			int32_t retry_count(3);
			do {
				
				ret = send_data(fd, pBuf, cbBuf, 3, (uint32_t)-1);
				if (ret<=0)
				{
					switch(ret)
					{
					case 0:
					case -5:
					case -6:
					case -9:
						LogWarn("NETWARN: occur error when send, error: %d, %s", errno, strerror(errno));
						--retry_count;
						nsleep(500);
						continue;
						break;
					case -2:
						LogErr("NETERR: occur error when send, error: %d, %s", errno, strerror(errno));
						if (NULL!=m_pEvent)
						{
							m_pEvent->OnCloseSck(fd, ipRemote, true);
						}
						retry_count = -1;
						this->CloseClient(fd);
						break;
					case -3:
					default:
						LogErr("NETERR: occur error when send, error: %d, %s", errno, strerror(errno));
						if (NULL!=m_pEvent)
						{
							m_pEvent->OnCloseSck(fd, ipRemote, false);
						}
						retry_count = -1;
						this->CloseClient(fd);
						break;
					}
				} 
				else
				{
					break;
				}
			}while(retry_count>0);
			if (ret > 0)
			{
				m_pLock->Lock(false);
				CByteStream &buff2 = pClient->getOutput();
				buff2.skip(ret);
				pClient->registerSend(ret);
				m_pLock->Unlock();
			}
			free(pBuf);
		} 
	} 

	return result;
}



IServerSocket* OpenNetSvr(IServerSocketEvent *pHandler, uint16_t wPort)
{
	CNetServer_Async *pServer = new CNetServer_Async(pHandler);
	if (NULL == pServer)
	{
		return NULL;
	}
	pHandler->BindSocket(pServer);
	int ret = pServer->Open(wPort);
	if (ret != 0)
	{
		SAFE_RELEASE(pServer);
		return NULL;
	}

	return pServer;
}

