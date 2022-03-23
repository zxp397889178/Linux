/*
 * =====================================================================================
 *
 *       Filename:  NetClient.cpp
 *
 *    Description:  net client object
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 13时58分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "NetClient.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//#include "ByteStream.h"
#include "NetFunc.h"

///////////////////////////////////////////////////////////////////////////////
IClientSocketEvent::~IClientSocketEvent()
{

}

IClientSocket::~IClientSocket()
{

}

///////////////////////////////////////////////////////////////////////////
//CNetClient
CNetClient::CNetClient(IClientSocketEvent *pEvent)
	: m_pEvent(pEvent)
	, m_strHost()
	, m_wPort(0)
	, m_bIsActive(false)
	, m_fd(-1)
	, m_cbSendBuf(-1)
	, m_cbRecvBuf(-1)
{

}

CNetClient::~CNetClient()
{
	this->internal_close();

	SAFE_RELEASE(m_pEvent);
}

bool CNetClient::Open(const char *pszHost, uint16_t wPort
		, uint32_t dwConnectTimeout)
{
	std::vector<uint32_t> arIp;
	int32_t ret = host_to_ip(pszHost, arIp);
	if (-1 == ret)
	{
		return false;
	}

	uint32_t ip = arIp[0];
	ret = this->Connect(ip, NETORDER(wPort), dwConnectTimeout);
	if (0 != ret)
	{
		this->internal_close();
		return false;
	}

	// 连接后处理
    m_bIsActive = true;
	m_strHost = pszHost;
	m_wPort = wPort;
	if (NULL != m_pEvent)
	{
		if( !m_pEvent->OnEstablished(pszHost, wPort) )
		{
			this->internal_close();
			return false;
		}
	}
	
	return true;
}

int32_t CNetClient::Connect(uint32_t dwIp, uint16_t wPort,
		uint32_t dwTimeout)
{
	m_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == m_fd)
	{
		return -1;
	}
	
	// 设置非阻寒
	int32_t flags = fcntl(m_fd, F_GETFL, 0);
	int32_t ret = fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
	if (-1 == ret)
	{
		this->PrintError(m_fd, "set non-block fail");
		return -1;
	}

	// 设置发送和接收缓存,失败也不断开
	int32_t opt_rcvbuf(RCV_BUFFER_SIZE);
	/* 
	ret = setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, (const char*)&opt_rcvbuf, sizeof(opt_rcvbuf));
	if (-1 == ret)
	{
		this->PrintError(m_fd, "set receive buffer size fail");	
	}
	*/
	int32_t opt_sndbuf(SND_BUFFER_SIZE);
	/* 
	ret = setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, (const char*)&opt_sndbuf, sizeof(opt_sndbuf));
	if (-1 == ret)
	{
		this->PrintError(m_fd, "set send buffer size fail");
	}
	*/

	//获取发送和接收缓冲区大小
	socklen_t len_rcvbuf = sizeof(opt_rcvbuf);
	ret = getsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, (void*)&opt_rcvbuf, &len_rcvbuf);
	socklen_t len_sndbuf = sizeof(opt_sndbuf);
	ret = getsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, (void*)&opt_sndbuf, &len_sndbuf);
	m_cbSendBuf = (int)opt_sndbuf;
	m_cbRecvBuf = (int)opt_rcvbuf;
	
	//设置linger
	struct linger no_linger = {0, 0};
	ret = setsockopt(m_fd, SOL_SOCKET, SO_LINGER, (const void*)&no_linger, sizeof(no_linger));
	if (-1 == ret)
	{
		this->PrintError(m_fd, "set not linger fail");
	}

	// 连接远端服务器
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = dwIp;
	addr.sin_port = wPort;
	ret = connect(m_fd, (struct sockaddr*)&addr, sizeof(addr));
	if (-1 == ret)
	{
		int32_t val = WSAGetLastError(m_fd);
		if (-1 == val)
		{
			fprintf(stderr, "connect fail, but can not obtain error info. [%d] %s\n",
					errno, strerror(errno));
			return -1;
		}
		else if (val != EINPROGRESS)
		{
			fprintf(stderr, "connect fail, [%d]%s\n", val, strerror(val));
			return -1;
		}
	}
	
	// 等待连接超时
	if(!this->IsConnected(dwTimeout)) return -1;

	return 0;
}

bool CNetClient::IsConnected(uint32_t dwTimeout)
{
	//int32_t ret = this->CheckSocket_BySelect(m_fd, SOCKET_CHECK_WRITE, dwTimeout);
	//return (ret==1);
	int32_t result(-1);
	fd_set wr_events;
	fd_set ex_events;
	fd_set rd_events;
	FD_ZERO(&wr_events);
	FD_ZERO(&ex_events);
	FD_ZERO(&rd_events);
	
	FD_SET(m_fd, &wr_events);
	FD_SET(m_fd, &rd_events);
	FD_SET(m_fd, &ex_events);
	long nSeconds = (long)(dwTimeout/1000);
	long nMicroSec = (long)((dwTimeout%1000)*1000);
	struct timeval timeout = { nSeconds, nMicroSec };
	bool correct=false;
	int32_t ret(0);
	ret = select((int)m_fd+1, &rd_events, &wr_events, &ex_events, &timeout);
	switch(ret)
	{
	case -1:
		fprintf(stderr, "ERR: connect failure, [%d]%s\n", errno, strerror(errno));
		break;
	case 0:
		fprintf(stderr, "ERR: connect timeout\n");
		result = 0;
		break;
	default:
		if(FD_ISSET(m_fd, &ex_events))
		{
			fprintf(stderr, "ERR: connect to remote host failure when some error occur.\n");
			break;
		}
		if(FD_ISSET(m_fd, &rd_events)) correct=true;
		if(FD_ISSET(m_fd, &wr_events))
		{
			if(correct)
			{
				int32_t val = WSAGetLastError(m_fd);
				fprintf(stderr, "connect ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));
				result = -1;
			}
			else result = 1;
		}
		break;
	} // end of switch
	
	return (result==1);
}

bool CNetClient::CanWrite(uint32_t dwTimeout)
{
	int32_t ret = this->CheckSocket_BySelect(m_fd, SOCKET_CHECK_WRITE, dwTimeout);
	return (ret!=-1);
}

int32_t CNetClient::CanRead(uint32_t dwTimeout)
{
	int32_t ret = this->CheckSocket_BySelect(m_fd, SOCKET_CHECK_READ, dwTimeout);
	return ret;
}

int32_t CNetClient::CheckSocket_BySelect(int32_t sockfd, int32_t flag, uint32_t dwTimeout)
{
	int32_t result(-1);
	fd_set wr_events;
	fd_set ex_events;
	fd_set rd_events;
	FD_ZERO(&wr_events);
	FD_ZERO(&ex_events);
	FD_ZERO(&rd_events);
	if (SOCKET_CHECK_WRITE==flag)
	{
		FD_SET(m_fd, &wr_events);
	}
	else if (SOCKET_CHECK_READ==flag)
	{
		FD_SET(m_fd, &rd_events);
	}
	FD_SET(m_fd, &ex_events);
	long nSeconds = (long)(dwTimeout/1000);
	long nMicroSec = (long)((dwTimeout%1000)*1000);
	struct timeval timeout = { nSeconds, nMicroSec };
	int32_t ret(0);
	if (SOCKET_CHECK_WRITE==flag)
	{
		ret = select((int)sockfd+1, NULL, &wr_events, &ex_events, &timeout);
	}
	else if (SOCKET_CHECK_READ==flag)
	{	
		ret = select((int)sockfd+1, &rd_events, NULL, &ex_events, &timeout);
	}
	switch(ret)
	{
	case -1:
		fprintf(stderr, "ERR: connect failure, [%d]%s\n", errno, strerror(errno));
		break;
	case 0:
		//fprintf(stderr, "ERR: connect timeout\n");
		result = 0;
		break;
	default:
		if(FD_ISSET(sockfd, &ex_events))
		{
			fprintf(stderr, "ERR: connect to remote host failure when some error occur.\n");
			break;
		}
		if (SOCKET_CHECK_WRITE==flag)
		{
			if(FD_ISSET(sockfd, &wr_events)) return 1;
			else {
				int32_t val = WSAGetLastError(sockfd);
				fprintf(stderr, "write ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));
				result = -1;
			}
		}
		else if (SOCKET_CHECK_READ==flag)
		{
			if (FD_ISSET(sockfd, &rd_events)) return 1;
			else {
				int32_t val = WSAGetLastError(sockfd);
				fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));	
				result = -1;
			}
		}
		break;
	} // end of switch
	
	return result;
}

int32_t CNetClient::CheckSocket_ByPoll(int32_t sockfd, int32_t flag, uint32_t dwTimeout)
{
	int32_t result(-1);
	struct pollfd pfd = {0};
	pfd.fd = sockfd;
	if (SOCKET_CHECK_WRITE==flag)
	{
		pfd.events = POLLOUT;
	}
	else if (SOCKET_CHECK_READ==flag)
	{
		pfd.events = POLLIN;
	}
	errno = 0;
	int32_t ret = poll(&pfd, 1, dwTimeout);
	switch(ret)
	{
	case -1:
		fprintf(stderr, "ERR: connect failure, [%d]%s\n", errno, strerror(errno));
		break;
	case 0:
		fprintf(stderr, "WRN: connect timeout.\n");
		result = 0;
		break;
	default:
		if ( HAS_PROP(pfd.revents, POLLERR)  
			|| HAS_PROP(pfd.revents, POLLHUP)) 
//			|| HAS_PROP(pfd.revents, POLLRDHUP) )
		{
			int32_t val = WSAGetLastError(sockfd);
			if (-1==val)
			{
				fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", errno, strerror(errno));
			}
			else if (val>0)
			{
				fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));
			}
			else
			{
				fprintf(stderr, "ERR: connect failure but can NOT detect the real error.\n");
			}
			break;
		}

		// connect succeed.
		if (SOCKET_CHECK_WRITE==flag)
		{
			if ( HAS_PROP(pfd.revents, POLLOUT) )
			{
				int32_t val = WSAGetLastError(sockfd);
				if (-1==val)
				{
					fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", errno, strerror(errno));
				}
				else if (val>0)
				{
					fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));
				}
				else
				{
					result = 1;
				}
				break;
			}
		}
		else if (SOCKET_CHECK_READ==flag)
		{
			if ( HAS_PROP(pfd.revents, POLLIN) )
			{
				int32_t val = WSAGetLastError(sockfd);
				if (-1==val)
				{
					fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", errno, strerror(errno));
				}
				else if (val>0)
				{
					fprintf(stderr, "ERR: connect to remote host failure, [%d]%s\n", val, strerror(val));
				}
				else
				{
					result = 1;
				}
				break;
			}
		}
		break;
	}
	return result;
}


void CNetClient::PrintError(int32_t sockfd, const char *ext_msg)
{
	int32_t ret = WSAGetLastError(sockfd);
	if (-1 == ret)
	{
		fprintf(stderr, "ERR: %s : [%d]%s\n", ext_msg, errno, strerror(errno));
	}
	else if (ret>0)
	{
		fprintf(stderr, "ERR: %s : [%d]%s\n", ext_msg, ret, strerror(ret));
	}
}

int32_t CNetClient::SendMsg(const void *pBuf, size_t &cbLen)
{
	static const int32_t s_nMaxLoopCount = 10;

	if (!this->IsActive())
	{
		return -1;
	}

	char *pc = (char*)(const_cast<void*>(pBuf));
	int32_t cbLeft((int)cbLen);
	cbLen = 0;
	MY_SOCK_ERROR result(ERR_OK);
	int32_t nLoopCount(0);	// 用于监测客户端网络的健康状态，若超出一定限制，则认为不适合再发送数据
	while(cbLeft>0 && nLoopCount<s_nMaxLoopCount)
	{
		int32_t cbNeedSend = std::min(m_cbSendBuf, cbLeft);	// 确保不会超出发送缓冲区
		ssize_t nSend = send(m_fd, (const void*)pc, (size_t)cbNeedSend, 0);
		if (-1 == nSend)
		{
			result = ERR_OTHERS;
			printf("ERR: send msg failure, [%d]%s\n", errno, strerror(errno));
			int32_t err = this->GetLastError();
			switch(err)
			{
			case EAGAIN:
			//case EWOULDBLOCK:
				//  The socket is marked non-blocking and the requested operation would block.
				if (this->CanWrite(500))
				{
					++nLoopCount;
					continue;
				}
				break;
			case EINTR:
				// A signal occurred before any data was transmitted.
				// can write, continue;
				fprintf(stderr, "WRN: receive a signal when send mesage.\n");
				++nLoopCount;
				continue;
				break;
			case ECONNRESET:
				// Connection reset by peer.
				if (m_pEvent != NULL)
				{
					m_pEvent->OnClose(true);
				}
				this->internal_close();
				fprintf(stderr, "WRN: remote host has close this socket.\n");
				result = ERR_CLOSED;
				break;
			case EPIPE:
				// The local end has been shut down on a connection oriented socket. 
				// In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.
				if (m_pEvent != NULL)
				{
					m_pEvent->OnClose(false);
				}
				this->internal_close();
				fprintf(stderr, "WRN: client has shutdown this socket.\n");
				result = ERR_SHUTDOWN;
				break;
/* 				
			case EMSGSIZE:
				// The socket type requires that message be sent atomically, 
				// and the size of the message to be sent made this impossible.
				break;
			case ENOBUFS:
				// The output queue for a network interface was full. This generally 
				// indicates that the interface has stopped sending, but may be 
				// caused by transient congestion. (Normally, this does not occur 
				// in Linux. Packets are just silently dropped when a device queue overflows.)
				break;
			case ENOMEM:
				// No memory available.
				break;
*/				
			default:
				fprintf(stderr, "ERR: send msg failure, [%d]%s\n", errno, strerror(errno));
				break;
			}
			break;
		}
		else if (0==nSend)
		{
			// not send any data, maybe error
			fprintf(stderr, "WRN: send ZEOR data, maybe failure.\n");
			continue;
		}
		else
		{
			pc += nSend;
			cbLeft -= nSend;
			cbLen += nSend;
		}
	}

	if (nLoopCount >= s_nMaxLoopCount)
	{
		fprintf(stderr, "WRN: catch system intr or in block-mode %d times, break\n", nLoopCount);
		result = ERR_SYSTEM;
	}
	if (cbLen > 0)
	{
		result = ERR_OK;
	}

	switch(result)
	{
	case ERR_OK:
		return cbLen;
		break;
	case ERR_CLOSED:
	case ERR_SHUTDOWN:
		return 0;
		break;
	default:
		return -2;
		break;
	}
}

void CNetClient::Close(void)
{
	if (!this->IsActive())
	{
		return;
	}

	m_bIsActive = false;
	if (NULL != m_pEvent)
	{
		m_pEvent->OnClose(false);
	}

	this->internal_close();
}


void CNetClient::internal_close(void)
{
	m_bIsActive = false;
	if (-1 != m_fd)
	{
		shutdown(m_fd, SHUT_RDWR);
		close(m_fd);
		m_fd = -1;
	}
}

bool CNetClient::GetHost(char *szHost, size_t cbHost, uint16_t &wPort)
{
	if (cbHost >= m_strHost.length())
	{
		return false;
	}
	strcpy(szHost, m_strHost.c_str());
	wPort = m_wPort;
	return true;
}

int32_t CNetClient::RecvMsg(void *pBuf, size_t &cbLen)
{
	static const int32_t s_nMaxLoopCount(10);

	int32_t ret = this->CanRead(0);
	if (-1==ret)
	{
		// error occur
		return -1;
	}
	else if (0==ret)
	{
		// timeout, not data avaiable
		cbLen = 0;
		return 1;
	}

	enum MY_SOCK_ERROR result(ERR_OK);
	int32_t cbLeft((int)cbLen);
	int32_t nLoopCount(0);
	char *pc = (char*)pBuf;
	cbLen = 0;
	while(cbLeft>0 && nLoopCount<s_nMaxLoopCount)
	{
		size_t cbNeedRecv = std::min(cbLeft, m_cbRecvBuf);
		ssize_t nRecv = recv(m_fd, (void*)pc, cbNeedRecv, 0);
		if (-1 == nRecv)
		{
			int32_t err = this->GetLastError();
			switch(err)
			{
			case EAGAIN:
				// The socket is marked non-blocking and the receive operation 
				// would block, or a receive timeout had been set and the 
				// timeout expired before data was received.
				result = ERR_NO_MORE_DATA;
				break;
			case ECONNREFUSED:
				// A remote host refused to allow the network connection 
				// (typically because it is not running the requested service).
				result = ERR_CLOSED;
				break;
			case EINTR:
				// The receive was interrupted by delivery of a signal 
				// before any data were available
				++nLoopCount;
				continue;
				break;
			default:
				result = ERR_OTHERS;
				break;
			}
			break;
		}
		else if (0==nRecv)
		{
			//remote closed
			result = ERR_CLOSED;
			break;
		}
		else
		{
			pc += nRecv;
			cbLeft -= nRecv;
			cbLen += nRecv;
		}
	}

	if (nLoopCount>=s_nMaxLoopCount)
	{
		fprintf(stderr, "WRN: catch system intr or in block-mode %d times, break\n", nLoopCount);
		result = ERR_SYSTEM;
	}

	switch(result)
	{
	case ERR_NO_MORE_DATA:
		return 1;
	case ERR_OK:
	case ERR_SYSTEM:
		return 2;
		break;
	case ERR_CLOSED:
		if (NULL != m_pEvent)
		{
			m_pEvent->OnClose(true);
		}
		this->internal_close();
		return -1;
		break;
	default:
		return -1;
		break;
	}
}

int32_t CNetClient::GetLastError(void)
{
	int32_t my_errno = errno;

	//int32_t err = WSAGetLastError(m_fd);
	return my_errno;	
}

int32_t CNetClient::ProcessRecv(uint32_t dwTimeout)
{
	static const int32_t s_nMaxLoopCount(10);

	int32_t nStat = this->CanRead(dwTimeout);
	if (nStat<=0)
	{
		return nStat;
	}

    enum MY_SOCK_ERROR result(ERR_OK);
    int32_t nLoopCount(0);
    unsigned char buffer[1024] = {0, };
    while(result==ERR_OK && nLoopCount<s_nMaxLoopCount)
    {
    	memset(&buffer,0,sizeof(buffer));
        size_t cbNeedRecv = std::min(sizeof(buffer), (size_t)m_cbRecvBuf);
        ssize_t nRecv = recv(m_fd, (void*)buffer, cbNeedRecv, 0);
        if (-1 == nRecv)
        {
            int32_t err = this->GetLastError();
            switch(err)
            {
            case EAGAIN:
                // The socket is marked non-blocking and the receive operation 
                // would block, or a receive timeout had been set and the 
                // timeout expired before data was received.
                result = ERR_NO_MORE_DATA;
                break;
            case ECONNREFUSED:
                // A remote host refused to allow the network connection 
                // (typically because it is not running the requested service).
                result = ERR_CLOSED;
                break;
            case EINTR:
                // The receive was interrupted by delivery of a signal 
                // before any data were available
                ++nLoopCount;
                continue;
                break;
            default:
                result = ERR_OTHERS;
                break;
            }
            break;
        }
        else if (0==nRecv)
        {
            //remote closed
            result = ERR_CLOSED;
            break;
        }
        else
        {
			if (NULL != m_pEvent)
			{
				m_buffer.Write((void*)buffer, nRecv);
			}
			// 若未指定 IClientSocketEvent, 则直接丢弃数据
        }
    }

    if (nLoopCount>=s_nMaxLoopCount)
    {   
        fprintf(stderr, "WRN: catch system intr or in block-mode %d times, break\n", nLoopCount);
        result = ERR_SYSTEM;
    }   

    switch(result)
    {   
    case ERR_OK:
    case ERR_NO_MORE_DATA:
	case ERR_SYSTEM:
        break;
    case ERR_CLOSED:
        if (NULL != m_pEvent)
        {
        	m_pEvent->OnClose(true);
        }
        this->internal_close();
        return -1; 
        break;
    default:
        return -1; 
        break;
    }   

	return 1;
}

int32_t CNetClient::ProcessData(void)
{
	if (m_buffer.empty())
	{
		return 0;
	}
	if (NULL == m_pEvent)
	{
		m_buffer.clear();
		return 0;
	}

	size_t cbLen = m_buffer.size();
	void *pBuf = calloc(sizeof(unsigned char), cbLen+1);
	m_buffer.SetPeekMode(true);
	m_buffer.Read(pBuf, cbLen);
	m_buffer.SetPeekMode(false);

	char *pc = (char*)pBuf;
	size_t cbLeft(cbLen);
	int32_t result(0);
	while(cbLeft>0)
	{
		int32_t ret = m_pEvent->OnRcvMsg(pc, cbLeft);
		if (0 == ret)
		{
			break;
		}
		else if (ret<0)
		{
			result = -1;
			break;
		}
		else
		{
			if (ret>(int)cbLeft)
			{
				ret = (int)cbLeft;
			}
			pc += ret;
			cbLeft -= ret;
		}
	}
	free(pBuf);

	if (result < 0)
	{
		this->Close();
		return -1;
	}

	int32_t nDataLen = (int)(cbLen - cbLeft);
	m_buffer.skip(nDataLen);
	return 1;
}

bool CNetClient::Process(uint32_t dwSleepTime)
{
	// receive all data
	int32_t ret = this->ProcessRecv(dwSleepTime);
	if (ret < 0)
	{
		return false;
	}

	// process all data
	ret = this->ProcessData();
	if (ret < 0)
	{
		return false;
	}

	return true;
}

IClientSocket* OpenClient(IClientSocketEvent *pHandler, const char *pszHost, uint16_t wPort)
{
	//printf("step 1\n");
	CNetClient *pClient = new CNetClient(pHandler);
	if (NULL == pClient)
	{
		return NULL;
	}
	//printf("step 2\n");
	bool ret = pClient->Open(pszHost,wPort);
	if (!ret)
	{
		SAFE_RELEASE(pClient);
		return NULL;
	}

	//printf("step 3\n");
	return pClient;
}



