/*
 * =====================================================================================
 *
 *       Filename:  IClientMgr.h
 *
 *    Description:  client manager
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 22时37分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SVRFRAME_ICLIENTMGR_H__
#define __SVRFRAME_ICLIENTMGR_H__

#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include "IClient.h"
#include "IGlobalManager.h"

class IClientMgr : public IManager
{
public:
    virtual ~IClientMgr() {  }

    // pClient      => 客户端指针
    // headerType   => 包文类型: 1(数据含包头); 2(数据不含包头长度)
    // msgId        => 命令ID
    // pData        => 请求协议
    // pkgLen       => 包体长度，包含自身长度
    virtual int32_t ExecuteMSG(IClient *pClient, int32_t headerType,
            uint32_t msgId, byte_t *pData, uint16_t pkgLen)
    {
        return -1;
    }
    virtual void AddPendingCloseQueue(IClient *pClient) = 0;

    // .so 插件用到的函数
public:
    virtual IClient* FindById(int32_t actor, uint32_t nId) = 0;
    virtual IClient* FindByUserId(int32_t actor, const CMyString &strUserId) = 0;
    virtual int32_t FindByActor(int32_t actor, std::vector<IClient*> &arClient) = 0;
    virtual int32_t BuildClientMap(IClient* pClient) = 0;
    virtual int32_t Multicast(int32_t actor, void *pData, size_t cbData) = 0;
    virtual int32_t Broadcast(void *pData, size_t cbData) = 0;

    // 释放client
    virtual void close(IClient *pClient) = 0;

public:
    virtual int32_t getType(void) { return mtClient; }
    virtual IObject* Find(uint32_t id)
    {
        return this->FindById(1, id);
    }
    virtual IObject* Find(const CMyString &strId)
    {
        return this->FindByUserId(1, strId);
    }

    virtual bool AddObject(IObject *pObject) = 0;
    virtual bool RemoveObject(IObject *pObject) = 0;
    
};

#endif // __SVRFRAME_ICLIENTMGR_H__

