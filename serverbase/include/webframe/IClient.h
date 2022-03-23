/*
 * =====================================================================================
 *
 *       Filename:  IClient.h
 *
 *    Description:  client interface
 *
 *        Version:  1.0
 *        Created:  2019年09月27日 13时18分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SVRFRAME_CLIENT_INTERFACE_H__
#define __SVRFRAME_CLIENT_INTERFACE_H__

#pragma once

class IClient
{
public:
    virtual ~IClient()
    {
    }

    virtual void* allocateNewBlock(uint32_t cbSize) = 0;
    virtual void* getDataBlock(void) = 0;
    virtual uint32_t getClientId(void) const = 0;
    virtual uint32_t getWorkerId(void) const = 0;
    virtual uint32_t getIp(void) const = 0;
    virtual const char* getIpStr(void) = 0;

    // 获得初始连接时间
    virtual uint64_t getCreateTime(void) = 0;
    // 获得初始接收消息的时间
    virtual uint64_t getFirstRecvTime(void) = 0;
    // 独得初始处理消息的时间
    virtual uint64_t getFirstDealTime(void) = 0;

    // 重置IP地址
    virtual void setIp(const CMyString &strIp) = 0;

public:
    virtual void SendMsg(CByteStream &buff) = 0;
    virtual void SendMsg(const void* pData, size_t cbData) = 0;
};

#endif // __SVRFRAME_CLIENT_INTERFACE_H__

