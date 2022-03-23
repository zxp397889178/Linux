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

#include "BaseCode.h"
#include "IGlobalManager.h"

enum enumActor
{
    actorClient = 1,     // 客户端
    actorGM,             // GM端
};

class IClient;
class IClientMgr;
typedef void (*GPFN_CLIENT_FREE)(IClient*, IClientMgr*);
// typedef void (*GPFN_CLIENT_FREE)(IClient*, IClientMgr*, ISimDB*, CMyRedis*);

class IClientObject
{
public:
    virtual ~IClientObject()
    {

    }

    virtual void release(void) = 0;

    // 获得宿主指针
    virtual IClient* getParent(void) = 0;
};

class IClient : public IObject
{
public:
    virtual ~IClient()
    {
    }

    // 分配一个POD数据块(m_pDataBlock)
    virtual void* allocateDataBlock(uint32_t cbSize) = 0;
    virtual void* getDataBlock(void) = 0;

    virtual uint32_t getClientId(void) const = 0;
    virtual uint32_t getWorkerId(void) const = 0;
    virtual uint32_t getIp(void) const = 0;
    virtual const char* getIpStr(void) = 0;
    virtual void setIp(uint32_t val) = 0;
    virtual void setIp(const CMyString &val) = 0;

    // 角色信息
    virtual int32_t  getActor(void) = 0;
    virtual void setActor(int32_t actor) = 0;
    // INT型的key
    virtual uint32_t getId(void) = 0;
    virtual void setId(uint32_t userId) = 0;
    // 字符串型的key
    virtual CMyString getUserId(void) = 0;
    virtual CMyString getStrId(void) { return this->getUserId(); }
    virtual void setUserId(const CMyString &val) = 0;

    virtual void SendMsg(CByteStream &buff) = 0;
    virtual void SendMsg(void* pData, size_t cbData) = 0;

public:
    // 注册一函数指针，用于在释放时自动调用相关操作
    virtual void resisterReleaseRoutine(GPFN_CLIENT_FREE pfn)
    {
        m_pfnClientFree = pfn; 
    }
    virtual bool hasReleaseRoutine(void)
    {
        return (m_pfnClientFree != NULL);
    }
    GPFN_CLIENT_FREE getReleaseRoutine(void)
    {
        return m_pfnClientFree;
    }

    // 分配一个类对象(m_pObject)
    virtual IClientObject* allocateObject(IClientObject* ptr) = 0;
    virtual IClientObject* getObject(void) = 0;
    
public:
    // 设置是否手动释放资源,缺省是自动释放
    // 若在client shutdown时不自动释放资源，必须调用 setManualRelease();
    virtual void setManualRelease(void) = 0;
    // 告诉系统不在使用此资源，可以释放
    virtual void setNotUseAnymore(void) = 0;

protected:
    GPFN_CLIENT_FREE        m_pfnClientFree; // 回调的释放函数
    void*                   m_pDataBlock;    // POD 数据块
    IClientObject*          m_pObject;       // IClientObject 对象
};


#endif // __SVRFRAME_CLIENT_INTERFACE_H__

