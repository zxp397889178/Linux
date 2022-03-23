/*
 * =====================================================================================
 *
 *       Filename:  NetFunc.cpp
 *
 *    Description:  common network function
 *
 *        Version:  1.0
 *        Created:  2014年11月12日 14时14分53秒
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
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
// #include <linux/sockios.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <string>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "BaseCode.h"


extern int32_t check_socket_status(int32_t sockfd, uint32_t dwTimeout);
// http://blog.chinaunix.net/uid-20593763-id-1620213.html
int32_t GetLocalIp(std::vector<uint32_t> &arIp)
{
	int32_t sck = socket(PF_INET, SOCK_DGRAM, 0);
	struct ifconf conf;
	char buffer[BUFSIZ] = "";
	conf.ifc_len = BUFSIZ;
	conf.ifc_buf = buffer;
	ioctl(sck, SIOCGIFCONF, &conf);
	int32_t num = conf.ifc_len / sizeof(struct ifreq);
	struct ifreq *ifr = conf.ifc_req;
	int32_t count(0);
	for(int32_t i=0; i<num; ++i)
	{
		struct sockaddr_in *sin = (struct sockaddr_in*)(&ifr->ifr_addr);
		ioctl(sck, SIOCGIFFLAGS, ifr);
	    if(((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
		{
			uint32_t ipv4 = sin->sin_addr.s_addr;
			arIp.push_back(ipv4);
			++count;
		}
		++ifr;
	}
	close(sck);
	return count;
}

uint32_t GetLocalIp(void)
{
    std::vector<uint32_t> arIp;
    int32_t ret = GetLocalIp(arIp);
    if (0 == ret)
    {
        return 0;
    }
    return arIp[0];
}


int32_t host_to_ip(const char *pszHost, std::vector<uint32_t> &arIp)
{
	char hostname[1025] = "";
	if (NULL==pszHost || '\0'==pszHost[0] || !strcmp(pszHost, "127.0.0.1") )
	{
		//获得本机的IP, 无法用getaddrinfo 或 gethostbyname 来获得真实 IP
		return GetLocalIp(arIp);
	}

	strcpy(hostname, pszHost);
	uint32_t addr = inet_addr(hostname);
	if (INADDR_ANY==addr || INADDR_NONE==addr)
	{
		//解析域名
		struct addrinfo hints;
		struct addrinfo *result, *addr_ptr;

		// memset(&hints, 0, sizeof(hints));
		bzero(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;	// ipv4 or ipv6
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;
		hints.ai_protocol = 0;	// any protocol

		int32_t ret = getaddrinfo(hostname, NULL, &hints, &result);
		if (ret != 0)
		{
			fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(ret));
			return -1;
		}

		// traverse the returned list
		for (addr_ptr=result; addr_ptr!=NULL;
				addr_ptr=addr_ptr->ai_next)
		{
			struct sockaddr_in *addr_ipv4 = (struct sockaddr_in*)(addr_ptr->ai_addr);
			uint32_t ipv4 = addr_ipv4->sin_addr.s_addr;
			if (ipv4 != INADDR_ANY || ipv4 != INADDR_NONE)
			{
				arIp.push_back(ipv4);
			}
		}
		freeaddrinfo(result);
	}
	else
	{
		arIp.push_back(addr);
	}

	if (0 == arIp.size())
	{
		return -1;
	}
	return 0;
}

const char* ip2str(uint32_t addr, char *szText, size_t cbText)
{
	struct in_addr host;
	host.s_addr = addr;
	char *pszHost = inet_ntoa(host);
	size_t cbHost = strlen(pszHost);
	if (cbHost>=cbText)
	{
		// 空间不足
		return NULL;
	}
	strcpy(szText, pszHost);
	return szText;
}

uint32_t str2ip(const char *ip_str)
{
	if (NULL==ip_str || '\0'==ip_str[0])
	{
		return INADDR_NONE;
	}

	struct in_addr host;
	int32_t ret = inet_aton(ip_str, &host);
	if (0 == ret)
	{
		return INADDR_NONE;
	}
	return host.s_addr;
}

// http://my.oschina.net/u/158589/blog/62896
const char* ip_to_host(const char *ip_str)
{
	struct addrinfo hints;
	struct addrinfo *result, *addr_ptr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME | AI_NUMERICHOST;
	hints.ai_protocol = 0; // any protocol
	int32_t ret = getaddrinfo(ip_str, NULL, &hints, &result);
	if (ret != 0)
	{
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(ret));
		return NULL;
	}

	std::vector<std::string> arHost;
	// traverse the returned list
	for(addr_ptr=result; addr_ptr!=NULL; addr_ptr=addr_ptr->ai_next)
	{
		char hostname[1025] = "";
		ret = getnameinfo(addr_ptr->ai_addr, addr_ptr->ai_addrlen, 
				hostname, sizeof(hostname), 
				NULL, 0, 
				NI_NAMEREQD);
		if (0==ret)
		{
			arHost.push_back(hostname);
		}
		else
		{
			struct sockaddr_in* addr = (struct sockaddr_in*)(addr_ptr->ai_addr);
			const char *pszIp = inet_ntoa(addr->sin_addr); 
			fprintf(stderr, "ip[%s], error in getnameinfo:%s\n", pszIp, gai_strerror(ret));
		}
	}
	freeaddrinfo(result);

	if (arHost.empty())
	{
		return NULL;
	}
	return arHost[0].c_str();
}

uint32_t net2local(uint32_t val)
{
	return ntohl(val);
}

uint16_t net2local(uint16_t val)
{
	return ntohs(val);
}

uint32_t local2net(uint32_t val)
{
	return htonl(val);
}

uint16_t local2net(uint16_t val)
{
	return htons(val);
}

int32_t WSAGetLastError(int32_t sockfd)
{
	/*int32_t err(0);
	int32_t len(0);
	int32_t ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)&err, 
			(socklen_t*)&len);
	if (0 == ret)
	{
		return err;
	}
	return -1;*/
	
	/*getsockopt获取的都是0，故用errno*/
	int32_t my_errno = errno;
	return my_errno;
}

int32_t print_socket_error(int32_t sockfd, const char *ext_msg /* =NULL */)
{
	int32_t ret = WSAGetLastError(sockfd);
	if (ret != 0)
	{
		if (NULL==ext_msg)
			LogErr("ERR: [%d]%s", ret, strerror(ret));
		else			
			LogErr("ERR: %s : [%d]%s", ext_msg, ret, strerror(ret));
	}

	return ret;
}

int32_t set_sock_rcvbuf(int32_t sockfd, int32_t rcvbuf)
{
	int32_t opt_rcvbuf(0);
	socklen_t len_rcvbuf = sizeof(opt_rcvbuf);
	int32_t ret = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void*)&opt_rcvbuf, &len_rcvbuf);

	if (rcvbuf > 0)
	{
		ret = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const char*)&rcvbuf, sizeof(rcvbuf));
		if (-1 == ret)
		{
			print_socket_error(sockfd, "set receive buffer size fail");	
			return opt_rcvbuf;
		}
		return rcvbuf;
	}
	return opt_rcvbuf;
}

int32_t set_sock_sndbuf(int32_t sockfd, int32_t sndbuf)
{
   	int32_t opt_sndbuf(0);
	socklen_t len_sndbuf = sizeof(opt_sndbuf);
   	int32_t ret = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void*)&opt_sndbuf, &len_sndbuf);

   	if (sndbuf > 0)
   	{
		ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const char*)&sndbuf, sizeof(sndbuf));
   		if (-1 == ret)
   		{
   			print_socket_error(sockfd, "set send buffer size fail");
			return opt_sndbuf;
   		}
		return sndbuf;
   	}

	return opt_sndbuf;
}

void set_sock_linger(int32_t sockfd, bool linger, int32_t linger_timeout)
{
	struct linger no_linger = {linger ? 1 : 0, linger_timeout};
	int32_t ret = setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const void*)&no_linger, sizeof(no_linger));
	if (-1 == ret)
	{
		print_socket_error(sockfd, "set not linger fail");
	}
}

int32_t create_tcp_socket(bool tcp /*=true */, bool blocked /*=false*/, 
		bool linger /*=false*/, int32_t rcvbuf /*=-1*/, int32_t sndbuf /*=-1*/)
{
	int32_t sockfd(-1);

	if (tcp)
	{
		sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else
	{
		sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	}
	if (-1 == sockfd)
	{
		return -1;
	}

	if (!blocked)
	{
		// 设置非阻寒
	    int32_t flags = fcntl(sockfd, F_GETFL);
		int32_t ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
		if (-1 == ret)
		{
			print_socket_error(sockfd, "set non-block fail");
			return -1;
		}
	}


	// 设置接收缓存
	set_sock_rcvbuf(sockfd, rcvbuf);

	// 设置发送缓存
	set_sock_sndbuf(sockfd, sndbuf);
	
	//设置linger
	set_sock_linger(sockfd, linger, 0);

	return sockfd;
}

int32_t open_tcp_port(int32_t sockfd, uint16_t port, bool listen_port /*=true*/, const char *bindip /*=NULL*/)
{
	// 设置是否复用socket 
	int one(1);
	int32_t ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if (ret < 0)
	{
		char text[256] = "";
		sprintf(text, "bind socket at port %d failure.", port);
		print_socket_error(sockfd, text);
		return -1;
	}

	// 设置tcp服务器端口
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	if (NULL==bindip || '\0'==bindip[0])
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(bindip);
	}
	addr.sin_port = htons(port);
	ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if (-1 == ret)
	{
		char text[256] = "";
		sprintf(text, "bind socket at port %d failure.", port);
		print_socket_error(sockfd, text);
		return -1;
	}

	// 侦听端口
	if (listen_port)
	{
		ret = listen(sockfd, SOMAXCONN);
		if (-1 == ret)
		{
			char text[256] = "";
			sprintf(text, "listensocket at port %d failure.", port);
			print_socket_error(sockfd, text);
			return -1;
		}
	}

	return 0;
}
	
int32_t open_tcp_socket(uint16_t port, bool tcp/*=true*/, bool blocked/*=false*/, 
		bool linger/*=false*/, int32_t rcvbuf/*=-1*/, int32_t sndbuf/*=-1*/,
		const char* bindip/*=NULL*/)
{
	int32_t sockfd = create_tcp_socket(tcp, blocked, linger, rcvbuf, sndbuf);
	if (-1 == sockfd)
	{
		return -1;
	}
	int ret = open_tcp_port(sockfd, port, (tcp?true:false), bindip);
	if (ret != 0)
	{
		shutdown(sockfd, SHUT_RDWR);
		close(sockfd);
		sockfd=-1;
		return -1;
	}
	return sockfd;
}

void close_tcp_socket(int32_t &sockfd, bool linger)
{
	if (sockfd < 0)
	{
		return;
	}

	if (linger)
	{
		set_sock_linger(sockfd, false, 0);
	}

	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
	sockfd = -1;
}

// maxloop_ 	 0   not any retry
// 			   	>0   max re-send count
// sendbuf	   	-1   not use sendbuf size
// 				 0   error, return 0
//			   	>0   max size each send
// return: 		>0   sended data
// 				0 	not any data send
// 		   		-1	socket fail, or not socket
// 		   		-2	connect reset by peer (对端主动关闭)
// 		   		-3  close by send side    (本端主动关闭) 
// 		   		-4	other send error
// 		   		-5  send buffer overflow
// 		   		-6  send retry timeout
// 		   		-9  send try count>maxloop_
// 		   
int32_t send_data(int32_t sockfd, void *pBuf, size_t cbLen, uint32_t maxloop_, uint32_t sendbuf_)
{
	static const int32_t s_nNetPkgSize= 4096;

	if (NULL==pBuf || 0==cbLen || 0==sendbuf_)
	{
		// not any data send
		return 0;
	}

	if (-1==sockfd)
	{
		return -1;
	}

	char *pc = (char*)(pBuf);
	int32_t cbLeft((int)cbLen);
	int32_t nTotalSend(0);
	int32_t result(0);
	uint32_t nLoopCount(0);	// 用于监测客户端网络的健康状态，若超出一定限制，则认为不适合再发送数据
#define NETMSG_ALIGN(len, pkg)	( ((len)+ (pkg)-1) & ~((pkg)-1) )
	int32_t net_count = NETMSG_ALIGN(cbLen, s_nNetPkgSize) / s_nNetPkgSize;
	maxloop_ *= net_count;

 	while(cbLeft>0 && ((0==maxloop_ && 0==nLoopCount) || nLoopCount<maxloop_))
	{
		int32_t cbNeedSend = std::min(sendbuf_, (uint32_t)cbLeft);	// 确保不会超出发送缓冲区
		cbNeedSend = std::min(s_nNetPkgSize, cbNeedSend);
		ssize_t nSend = send(sockfd, (const void*)pc, (size_t)cbNeedSend, 0);
		if (-1 == nSend)
		{
			int32_t err = WSAGetLastError(sockfd);
			switch(err)
			{
			case EAGAIN:
			//case EWOULDBLOCK:
				//  The socket is marked non-blocking and the requested operation would block.
				if (0==maxloop_ || nLoopCount>maxloop_)
				{
					result = -6;
					break;
				}

				{
					bool bCanResend(false);
					while(nLoopCount<maxloop_)
					{
						int32_t sockStat = check_socket_status(sockfd, 500);
						//if (HAS_PROP(sockStat, 2))
						if (sockStat==2 || sockStat==3)
						{
							bCanResend = true;
							break;
						}
						LogDebug("sockfd %d status %d", sockfd, sockStat);
						++nLoopCount;
					}
					if (bCanResend)
					{
						nLoopCount = 0;
						continue;
					}
				}
				result = -5;
				break;
			case EINTR:
				// A signal occurred before any data was transmitted.
				// can write, continue;
				LogWarn("WRN: receive a signal when send mesage.");
				++nLoopCount;
				continue;
				break;
			case ECONNRESET:
				// Connection reset by peer.
				result = -2;
				LogWarn("WRN: remote host has close this socket.");
				break;
			case EPIPE:
				// The local end has been shut down on a connection oriented socket. 
				// In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.
				result = -3;
				LogWarn("WRN: client has shutdown this socket");
				break;
			case ENOTSOCK:
				result = -1;
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
				result = -4;
				LogErr("ERR: send msg failure, [%d]%s", errno, strerror(errno));
				break;
			}
			break;
		}
		else if (0==nSend)
		{
			// not send any data, maybe error
			LogWarn("WRN: send ZEOR data, maybe failure.");
			continue;
		}
		else
		{
			pc += nSend;
			cbLeft -= nSend;
			nTotalSend += nSend;
			nLoopCount = 0;
		}
	} 


	if (0==result && nLoopCount>maxloop_ && cbLeft>0)
	{
		LogWarn("WRN: catch system intr or in block-mode %d times, break", nLoopCount);
		result = -9;
	}

	if (0==result)
	{
		//send ok
		result = nTotalSend;
	}
	else if (-5 == result || -6 == result || -9 == result)
	{
		if (nTotalSend > 0)
		{
			result = nTotalSend;
		}
	}
	return result;
}

int32_t send_data_ex(int32_t sockfd, void *pBuf, size_t cbLen, uint32_t &totalSend, uint32_t timeout)
{
	static const int32_t s_nNetPkgSize= 4096;

	totalSend = 0;
	if (NULL==pBuf || 0==cbLen)
	{
		// not any data send
		return 0;
	}

	if (-1==sockfd)
	{
		return -1;
	}

	char *pc = (char*)(pBuf);
	int32_t cbLeft((int)cbLen);
	int32_t result(0);
	uint32_t nLoopCount(0);	// 用于监测客户端网络的健康状态，若超出一定限制，则认为不适合再发送数据

	uint64_t begin = Now();
	do
	{
		int32_t cbNeedSend = std::min(s_nNetPkgSize, cbLeft);	
		ssize_t nSend = send(sockfd, (const void*)pc, (size_t)cbNeedSend, 0);
		if (-1 == nSend)
		{
			int32_t err = WSAGetLastError(sockfd);
			switch(err)
			{
			case EAGAIN:
			//case EWOULDBLOCK:
				//  The socket is marked non-blocking and the requested operation would block.
				{
					bool bCanResend(false);
					do
					{
						int32_t sockStat = check_socket_status(sockfd, 500);
						if (sockStat==2 || sockStat==3)
						{
							bCanResend = true;
							break;
						}
						++nLoopCount;
						if (timeout == (uint32_t)-1) continue;
					}while(begin + timeout > Now());

					if (bCanResend)
					{
						continue;
					}
				}
				// 发送超时
				result = -7;
				break;
			case EINTR:
				// A signal occurred before any data was transmitted.
				// can write, continue;
				LogWarn("WRN: receive a signal when send mesage.");
				++nLoopCount;
				if (timeout == (uint32_t)-1) continue;
				if (begin + timeout > Now()) result = -7;
				break;
			case ECONNRESET:
				// Connection reset by peer.
				result = -2;
				LogWarn("WRN: remote host has close this socket.");
				break;
			case EPIPE:
				// The local end has been shut down on a connection oriented socket. 
				// In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.
				result = -3;
				LogWarn("WRN: client has shutdown this socket");
				break;
			case ENOTSOCK:
				result = -1;
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
				result = -4;
				LogErr("ERR: send msg failure, [%d]%s", errno, strerror(errno));
				break;
			}
			break;
		}
		else if (0==nSend)
		{
			// not send any data, maybe error
			LogWarn("WRN: send ZEOR data, maybe failure.");
			++nLoopCount;
			if (timeout == (uint32_t)-1) continue;
			if (begin + timeout > Now())
			{
				result = -7;
				break;
			}
		}
		else
		{
			pc += nSend;
			cbLeft -= nSend;
			totalSend += nSend;
			nLoopCount = 0;
		}
	}while(cbLeft > 0);

	if (result < 0)
	{
		LogErr("send data failure, result %d, cost %d ms, send %u bytes, left %d bytes, loop %d times",
			result, Now()-begin, totalSend, cbLen-totalSend, nLoopCount);
	}
	else
	{
		LogTrace("send data OK, cost %lu ms, send %u bytes, loop %u times",
			Now()-begin, totalSend, nLoopCount);
	}

	return result;
}

// dwTimeout  in millisecond (1/1000 s)
// return 	[1 ==>can read, 2 ==>can write, 3 ==>can read & write]
// 			0	timeout
// 			-1	invalid socket
// 			-2	select failure
//			-3  socket catch exception
int32_t check_socket_status(int32_t sockfd, uint32_t dwTimeout)
{
	if (-1 == sockfd)
	{
		return -1;
	}

	int32_t result(0);
	fd_set wr_events;
	fd_set ex_events;
	fd_set rd_events;
	FD_ZERO(&wr_events);
	FD_ZERO(&ex_events);
	FD_ZERO(&rd_events);
	FD_SET(sockfd, &wr_events);
	FD_SET(sockfd, &rd_events);
	FD_SET(sockfd, &ex_events);

	long nSeconds = (long)(dwTimeout/1000);
	long nMicroSec = (long)((dwTimeout%1000)*1000);
	struct timeval timeout = { nSeconds, nMicroSec };
	int32_t ret = select((int32_t)sockfd+1, &rd_events, &wr_events, &ex_events, &timeout);
	switch(ret)
	{
	case -1:
		result = -2;
		LogErr("ERR: check status failure, [%d]%s", errno, strerror(errno));
		break;
	case 0:
		LogErr("ERR: check status timeout"); 
		result = 0;
		break;
	default:
		if (FD_ISSET(sockfd, &ex_events))
		{
			LogErr("ERR: check sockFd status failure");
			result = -3;
			break;
		}

#if 0
		{
			int32_t error(-1);
			socklen_t errlen(sizeof(error));
			getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &errlen);
			if (error != 0)
			{
				LogErr("ERR: socket occur error %d, %s, sockfd %u,select ret=%d", error, strerror(error), sockfd, ret);
				result = -4;
				break;
			}
		}
#endif

		if (FD_ISSET(sockfd, &wr_events))
		{
			//int32_t val = print_socket_error(sockfd, NULL);
			//if (0==val)
			{
				result |= 2;
			}
		}

		if (FD_ISSET(sockfd, &rd_events))
		{
			//int32_t val = print_socket_error(sockfd, NULL);
			//if (0==val)
			{
				result |= 1;
			}
		}
		break;
	} // end of switch

	return result;
}

int32_t set_sck_nonblock(int32_t sockfd)
{
	int32_t flags = fcntl(sockfd, F_GETFL);
	int32_t newflags = flags | O_NONBLOCK;
	// 设置非阻寒
	int32_t ret = fcntl(sockfd, F_SETFL, newflags);
	if (-1 == ret)
	{
		print_socket_error(sockfd, "set non-block fail");
		return -1;
	}
	return 0;
}

bool open_tcp_connect(int32_t sockfd, const char *pszIp, uint16_t port)
{
	string strHost = pszIp;
	struct hostent *pHost = gethostbyname(strHost.c_str());
	if (pHost != NULL)
	{
		strHost = inet_ntoa(*((struct in_addr *)pHost->h_addr));
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(strHost.c_str());
	addr.sin_port = htons(port);

	// call connect
	int32_t ret = connect(sockfd, (const struct sockaddr*)&addr, (socklen_t)sizeof(addr));
    if (ret==-1)
    {
        LogFatal("connect to %s:%d failure, errno %d, error %s",
                pszIp, port, errno, strerror(errno));
        return false;
    }
    return true;
}

int32_t open_tcp_connect(int32_t sockfd, const char *pszIp, uint16_t port, uint32_t timeout)
{
	string strHost = pszIp;
	struct hostent *pHost = gethostbyname(strHost.c_str());
	if (pHost != NULL)
	{
		strHost = inet_ntoa(*((struct in_addr *)pHost->h_addr));
	}

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(strHost.c_str());
	addr.sin_port = htons(port);

	// set to no-block socket
	int32_t flags = fcntl(sockfd, F_GETFL, 0);
	int32_t oldflags(flags);
	int32_t newflags = flags | O_NONBLOCK;
	int32_t ret(-1);
	if (newflags != oldflags)
	{
		// 设置非阻寒
		ret = fcntl(sockfd, F_SETFL, newflags);
		if (-1 == ret)
		{
			print_socket_error(sockfd, "set non-block fail");
			return -1;
		}
	}

	// call connect
	ret = connect(sockfd, (const struct sockaddr*)&addr, (socklen_t)sizeof(addr));
	if (ret != -1)
	{
		// connect succeed.
		return 0;
	}

	// wait for timeout
#if 0
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(sockfd, &rset);
	FD_SET(sockfd, &wset);
	struct timeval tv = {timeout/1000, (timeout%1000)*1000 };
	int32_t status = select(sockfd+1, &rset, &wset, NULL, (timeout==0) ? NULL : &tv);
	if (0 == status)
	{
		// timeout
		return -2;
	}
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
	{
		int32_t error(-1);
		socklen_t len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
		{
			// error
			return -3;
		}
		if (error != 0)
		{
			// error
			printf("getsocket error %d, %s\n", error, strerror(error) );
			return -4;
		}
	}
	else
	{
		// error
		return -5;
	}
#else
	int32_t status = check_socket_status(sockfd, timeout);
	if (status <= 0)
	{
		//printf("socket error %d, %s\n", errno, strerror(errno));
		return status - 1;
	}
#endif

	return 0;
}

void set_sock_nagle(int32_t sockfd, bool bDisable)
{
    // TODO:
    int flag = 1;
    if (!bDisable) flag = 0;
    //setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
}

bool decode_url(const CMyString &url, CMyString &strHost, int32_t &port, CMyString &strCgi, CMyString &strParam, bool &bSSL)
{
    bSSL = false;
    CMyString temp;
    if (strncasecmp(url.c_str(), "http://", 7) == 0)
    {
        temp = url.substr(7);
    }
    else if (strncasecmp(url.c_str(), "https://",8) == 0)
    {
        bSSL = true;
        temp = url.substr(8);
    }
    else
    {
        return false;
    }
    
    std::string::size_type pos1 = temp.find_first_of('/');
    if (pos1 == std::string::npos)
    {
        strHost = temp;
        std::string::size_type pos4 = strHost.find_first_of(":");
        if (pos4 != std::string::npos)
        {
            CMyString temp1 = strHost.substr(pos4+1);
            port = StrToLong(temp1.c_str());
            strHost = strHost.substr(0, pos4);
        }
        else
        {
            port = bSSL ? 443 : 80;
        }
        return true;
    }
    strHost = temp.substr(0, pos1);
    std::string::size_type pos4 = strHost.find_first_of(":");
    if (pos4 != std::string::npos)
    {
        CMyString temp1 = strHost.substr(pos4+1);
        port = StrToLong(temp1.c_str());
        strHost = strHost.substr(0, pos4);
    }
    else
    {
        port = bSSL ? 443 : 80;
    }
    temp = temp.substr(pos1+1);

    std::string::size_type pos2 = temp.find_first_of('?');
    if (pos2 == std::string::npos)
    {
        strCgi = temp;
        return true;
    }
    else
    {
        strCgi = temp.substr(0, pos2);
        strParam = temp.substr(pos2+1);
    }

    return true;
}

bool decode_url_ex(const CMyString &strUrl, CMyString &strHost, int32_t &port, CMyString &strCgi, CMyString &strParam, bool &bSSL)
{
    char host[256] = "";
    char uri[512] = "";

    bSSL = false;
    if (false) { }
    else if (sscanf(strUrl.c_str(), "http://%[^:/]:%d/%s", host, &port, uri) == 3) {
    }
    else if (sscanf(strUrl.c_str(), "http://%[^:/]/%s", host, uri) == 2) {
        port = 80;
    }
    else if (sscanf(strUrl.c_str(), "http://%[^:/]:%d", host, &port) == 2) {
        uri[0] = '\0';
    }
    else if (sscanf(strUrl.c_str(), "http://%[^:/]", host) == 1) {
        port = 80;
        uri[0] = '\0';
    }
    else if (sscanf(strUrl.c_str(), "https://%[^:/]:%d/%s", host, &port, uri) == 3) {
        bSSL=true;
    }
    else if (sscanf(strUrl.c_str(), "https://%[^:/]/%s", host, uri) == 2) {
        bSSL=true;
        port = 443;
    }
    else if (sscanf(strUrl.c_str(), "https://%[^:/]:%d", host, &port) == 2) {
        bSSL=true;
        uri[0] = '\0';
    }
    else if (sscanf(strUrl.c_str(), "https://%[^:/]", host) == 1) {
        bSSL=true;
        port = 443;
        uri[0] = '\0';
    }
    else
    {
        return false;
    }

    strHost = host;
    strCgi = uri;
    std::string::size_type pos = strCgi.find_first_of("?");
    if (pos != std::string::npos)
    {
        strParam = strCgi.substr(pos+1);
        strCgi = strCgi.substr(0, pos);
    }

    return true;
}


