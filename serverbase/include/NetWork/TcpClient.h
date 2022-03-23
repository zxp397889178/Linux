/*
 * =====================================================================================
 *
 *       Filename:  TcpClient.h
 *
 *    Description:  简化的网络客户端的封装
 *
 *        Version:  1.0
 *        Created:  01/28/2020 07:05:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"
#include "SSLClient.h"

namespace net 
{

class CTcpClient
{
    enum Status
    {
        netDeactive,
        netActive,
        netPendingRecv,
        netClosing,
        netClosed,
    };

public:
    CTcpClient();
    ~CTcpClient();

    // 连接远端服务
    //返回值：0  连接远端服务成功
    //        -1 创建socket失败
    //        -2 连接远端服务失败
    //        -3 连接超时
    int32_t Open(const CMyString &strHost, uint16_t port, bool bSSL);
    int32_t Open(const CMyString &strHost, uint16_t port, bool bSSL, uint32_t timeout);

    // 发送数据， timeout以毫秒计算
    // 返回值: >=0 表示发送成功的字节数
    //         -1  socket 服务已失效
    //         -2  发送失败，socket断开
    //         -3  发送超时
    int32_t SendMsg(void* pData, size_t cbData, uint32_t timeout = 1000);

    // 接收数据
    // 返回值：>0 表示接收成功的字节数
    //          0 表示无数据可接收,处于pending状态
    //         -1 socket 失效
    //         -3 在检查socket是否有数据时异常了，错误号可查 errno
    //         -5 接收数据时异常，错误号查 errno
    //         -7 在接收文件时对方关闭
    //
    int32_t RecvMsg(void *pData, size_t &cbData, uint32_t timeout=1000);

    // 关闭socket
    void Close();

    bool IsActive(void) 
    { 
        return (m_eState==netActive) || (m_eState==netPendingRecv); 
    }

private:
    CMyString       m_strHost;
    uint16_t        m_port;
    int32_t         m_fd;

    int32_t         m_eState;

private:
    CSSLClient      m_ssl;
};

};
