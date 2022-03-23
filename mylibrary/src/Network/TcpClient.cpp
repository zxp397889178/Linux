/*
 * =====================================================================================
 *
 *       Filename:  TcpClient.cpp
 *
 *    Description:  简化的网络客户端的封装
 *
 *        Version:  1.0
 *        Created:  01/28/2020 07:16:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "TcpClient.h"
#include "NetFunc.h"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

namespace net 
{

CTcpClient::CTcpClient()
    : m_port(0)
    , m_fd(-1)
    , m_eState(netDeactive)
{
}

CTcpClient::~CTcpClient()
{
    this->Close();
}

int32_t CTcpClient::Open(const CMyString &strHost, uint16_t port, bool bSSL)
{
    int32_t fd = create_tcp_socket(true, true, true);
    if (-1 == fd)
    {
        LogFatal("[%s:%s:%d]create tcp socket failure, errno: %d, error: %s", 
                __FILE__, __func__, __LINE__, errno, strerror(errno));
        return -1;
    }

    if (!open_tcp_connect(fd, strHost.c_str(), port))
    {
        LogFatal("[%s:%s:%d]connect to %s:%d failure, errno: %d, error: %s.",
                __FILE__, __func__, __LINE__, strHost.c_str(), port,
                errno, strerror(errno));
        return -2;
    }

    if (bSSL)
    {
        int32_t ret = m_ssl.Init(fd);
        if (ret < 0)
        {
            return ret-2;
        }
    }

    m_fd = fd;
    m_strHost = strHost;
    m_port = port;
    m_eState = netActive;
    return 0;
}

int32_t CTcpClient::Open(const CMyString &strHost, uint16_t port, bool bSSL, uint32_t timeout)
{
    int32_t fd = create_tcp_socket(true, true, true);
    if (-1 == fd)
    {
        LogFatal("[%s:%s:%d]create tcp socket failure, errno: %d, error: %s", 
                __FILE__, __func__, __LINE__, errno, strerror(errno));
        return -1;
    }

    int32_t ret = open_tcp_connect(fd, strHost.c_str(), port, timeout);
    if (ret < 0)
    {
        if (ret == -2)
        {
            LogFatal("[%s:%s:%d]connect to %s:%d timeout.",
                    __FILE__, __func__, __LINE__, strHost.c_str(), port);
            return -3;
        }
        LogFatal("[%s:%s:%d]connect to %s:%d failure, errno: %d, error: %s.", 
                __FILE__, __func__, __LINE__, strHost.c_str(), port, 
                errno, strerror(errno));
        return -2;
    }

    if (bSSL)
    {
        if (!m_ssl.Init(fd))
        {
            return -3;
        }
    }

    m_fd = fd;
    m_strHost = strHost;
    m_port = port;
    m_eState = netActive;
    return 0;
}

int32_t CTcpClient::SendMsg(void *pData, size_t cbData, uint32_t timeout)
{
    if (!this->IsActive())
    {
        return -1;
    }

    if (m_ssl.IsActive())
    {
        int32_t ret = m_ssl.SendMsg(pData, cbData);
        if (ret < 0)
        {
            m_ssl.Close();
            this->Close();
        }
        return ret;
    }

    uint32_t sendData(0);
    int32_t ret = send_data_ex(m_fd, pData, cbData, sendData, timeout);
    if (ret < 0)
    {
        if (ret==-1)
        {
            m_eState = netDeactive;
        }
        else if (ret<=-2 && ret>=-4)
        {
            m_eState = netClosing;
        }
        else if (ret == -7) 
        {
            LogWarn("send data to %s:%d timeout.", m_strHost.c_str(), m_port);
            return -3;
        }

        if (!this->IsActive())
        {
            LogFatal("[%s:%s:%d]send data to %s:%d failure, errno: %d, error: %s",
                    __FILE__, __func__, __LINE__, m_strHost.c_str(), m_port,
                    errno, strerror(errno));
            this->Close();
            return -2;
        }
    }

    return (int32_t)sendData;
}

int32_t CTcpClient::RecvMsg(void *pData, size_t &cbData, uint32_t timeout)
{
    if (!this->IsActive())
    {
        return -1;
    }
/*
    int32_t ret = check_socket_status(m_fd, timeout);
    switch(ret)
    {
    case 0:     // timeout
    case 2:     // can write
        //LogDebug("TCP>check sck status timeout.");
        m_eState = netPendingRecv;
        cbData = 0;
        return 0;
        break;
    case 1:     // can read
    case 3:     // can read & write
        break;
    case -1:    // non-socket
        m_eState = netDeactive;
        break;
    default:
        m_eState = netClosing;
        break;
    }

    // 导常，中止并回退
    if (!this->IsActive())
    {
        LogFatal("[%s:%s:%d]send data to %s:%d failure, errno: %d, error: %s",
                __FILE__, __func__, __LINE__, m_strHost.c_str(), m_port,
                errno, strerror(errno));
        this->Close();
        return -3;
    }
    // 等待接收，下次再循环
    if (m_eState == netPendingRecv)
    {
        return 0;
    }
*/    
    if (m_ssl.IsActive())
    {
        int32_t ret = m_ssl.RecvMsg(pData, cbData);
        if (ret < 0)
        {
            m_ssl.Close();
            this->Close();
        }
        return ret; 
    }

    // 接收数据
    int32_t result(0);
    ssize_t val = recv(m_fd, pData, cbData, 0);
    if (val <0)
    {
        if (errno == EAGAIN)
        {
            LogDebug("TCP>pending to read data.");
            m_eState = netPendingRecv;
        }
        else
        {
            m_eState = netClosing;
            result = -5;
        }
    }
    else if (val==0)
    {
        m_eState = netClosing;
        result = -7;
    }
    else
    {
        cbData = val;
        m_eState = netActive;
        result = (int32_t)val;
    }

    if (!this->IsActive())
    {
        this->Close();
    }

    return result;
}

void CTcpClient::Close()
{
    if (m_fd > 0)
    {
        shutdown(m_fd, SHUT_RDWR);
        close(m_fd);
        m_fd = -1;
    }

    m_eState = netClosed;
}

}

