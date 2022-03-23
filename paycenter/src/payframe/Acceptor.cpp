/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.cpp
 *
 *    Description:  gate client acceptor
 *
 *        Version:  1.0
 *        Created:  2019年09月09日 01时12分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Acceptor.h"
#include "MyNet.h"

CAcceptor::CAcceptor()
    : m_nBalanceIndex(0)
{

}

CAcceptor::~CAcceptor()
{
    VEC_NETINFO::iterator it(m_vecNetInfo.begin());
    for(; it != m_vecNetInfo.end(); ++it)
    {
        NetInfo &netinfo = *it;
        LogImpt("server shutdown port %u.", netinfo.port);
        close_tcp_socket(netinfo.fd, true);
    }
    m_vecNetInfo.clear();
}


int32_t CAcceptor::Init(void)
{
    int32_t count(0);
    int32_t net_cnt = CConfig::Instance().getNetCount();
    for(int32_t idx=0; idx<net_cnt; ++idx)
    {
        NetInfo netinfo;
        CConfig::Instance().getNet(idx, netinfo);

        netinfo.fd = open_tcp_socket(netinfo.port, true, false, true, 4*1024*1024);
        if (-1 == netinfo.fd)
        {
            return -1001;
        }
        //printf("DEBUG>accept fd=%d\n", netinfo.fd);

        m_vecNetInfo.push_back(netinfo);
        ++count;
        LogImpt("server listen at port %u", netinfo.port);
    }
    return count;
}

int32_t CAcceptor::Accept(VEC_NETACCEPT &vecClient, uint32_t accMax)
{
	fd_set rd_events;
	fd_set ex_events;
	FD_ZERO(&rd_events);
	FD_ZERO(&ex_events);
	
	int32_t maxFd = 0;
    VEC_NETINFO::iterator it(m_vecNetInfo.begin());
    for(; it != m_vecNetInfo.end(); ++it)
    {
        NetInfo &netinfo = *it;
        FD_SET(netinfo.fd, &rd_events);
        FD_SET(netinfo.fd, &ex_events);
        if (maxFd < netinfo.fd)
        {
            maxFd = netinfo.fd;
        }
    }

    //printf("DEBUG>maxfd=%d\n", maxFd);
    int32_t nAcceptTimeout = CConfig::Instance().getAcceptTimeout();
    //printf("DEBUG>accept timeout %u ms\n", nAcceptTimeout);
	struct timeval tv = { nAcceptTimeout/1000, (nAcceptTimeout% 1000) * 1000 };
    int32_t result(-1005);
	int32_t ret = ::select(maxFd+1, &rd_events, NULL, &ex_events, &tv);
    switch(ret)
    {
	case -1:
		LogErr("wait to accept error,[%d] %s", errno, strerror(errno));
		result = -1;
		break;
	case 0:
		LogTrace("not any client pending accept.");
		result = 0;
		break;
	default:
        result = doAccept(rd_events, ex_events, vecClient, accMax);
        break;

    }

    return result;
}

int32_t CAcceptor::doAccept(fd_set& rd_events, fd_set& ex_events,
        VEC_NETACCEPT &vecAcc, uint32_t accMax)
{
    for(size_t idx(0); 
            ((idx<m_vecNetInfo.size()) && (vecAcc.size()<accMax)); 
            ++idx)
    {
        int32_t svr_idx = (m_nBalanceIndex++)%m_vecNetInfo.size();

        NetInfo &netinfo = m_vecNetInfo.at(svr_idx);
        if (FD_ISSET(netinfo.fd, &ex_events))
        {
            LogFatal("[%s:%s:%d]port %u occur error, result:%d, %s",
                    __FILE__, __func__, __LINE__, netinfo.port, errno, strerror(errno));
            // TOOD: 移出故障的PORT
            //
            return -1;
        }

        if (FD_ISSET(netinfo.fd, &rd_events))
        {
            uint32_t acc_cnt = static_cast<uint32_t>(vecAcc.size());
    		while ( acc_cnt < accMax)
    		{
    			struct sockaddr_in remote_addr = {0};
    			socklen_t socklen = sizeof(remote_addr);
    			int32_t fdChild = ::accept(netinfo.fd, 
                        (struct sockaddr*)&remote_addr, &socklen);
    			if (-1 == fdChild)
    			{
    				if (errno != EAGAIN && errno != EWOULDBLOCK)
    				{
    					LogErr("port %u occur error, result:%d, %s", 
                                netinfo.port, errno, strerror(errno));
    				}
    				break;
    			}
    			if (0 == fdChild)
    			{
    				break;
    			}

    			vecAcc.push_back(fdChild);
                ++acc_cnt;
    		}
        }
    }
    int32_t result = static_cast<int32_t>(vecAcc.size());
    return result;
}

bool CAcceptor::getNetInfo(uint32_t port, NetInfo &netinfo)
{
    bool result(false);
    VEC_NETINFO::iterator it(m_vecNetInfo.begin());
    for(; it != m_vecNetInfo.end(); ++it)
    {
        NetInfo &net1 = *it;
        if (net1.port == port)
        {
            netinfo = net1;
            result = true;
            break;
        }
    }
    return result;
}
