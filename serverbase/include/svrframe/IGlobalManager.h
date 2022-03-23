/*
 * =====================================================================================
 *
 *       Filename:  IGlobalMgr.h
 *
 *    Description:  全局管理器的接口
 *
 *        Version:  1.0
 *        Created:  03/10/2020 07:49:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

class IObject
{
public:
    virtual ~IObject() {};

    virtual uint32_t getId(void) = 0;
    virtual CMyString getStrId(void) = 0;

    virtual void* getValue(void) = 0;
};

class IManager
{
public:
    virtual ~IManager() {};

    virtual int32_t getType(void) = 0;
    virtual IObject* Find(uint32_t id) = 0;
    virtual IObject* Find(const CMyString &strId) = 0;

    virtual bool AddObject(IObject *pObject) = 0;
    virtual bool RemoveObject(IObject *pObject) = 0; 
    virtual bool RemoveObject(uint32_t id)
    {
        return this->RemoveObject(this->Find(id));
    }
    virtual bool RemoveObject(const CMyString &strId)
    {
        return this->RemoveObject(this->Find(strId));
    }

    // 调用一个方法，或者一个类的实现
    // 返回值：0 成功，<0 失败
    virtual int32_t CreateRoutine(void *pParam, int64_t &val) = 0;
};

enum ManagerType
{
    mtClient = 1,
    mtRoom,
};

class IGlobalManager
{
public:
    virtual ~IGlobalManager() {  };

    // 增加 IManager 对象，由IGlobalManager 释放内存
    virtual bool AddManager(IManager *pMgr) = 0;
    // 注册 IManager 对象，外部释放内存
    virtual bool RegisterManager(IManager *pMgr) = 0;
    // 删除 IManager 对象，释放所有 IObject 对象
    virtual bool RemoveManager(IManager *pMgr) = 0;
    // 查找到 IManager 对象指针
    virtual IManager* FindManager(int32_t nType) = 0;

    virtual bool AddObject(int32_t nType, IObject *pObject) = 0;
    virtual bool RemoveObject(int32_t nType, IObject *pObject) = 0;
    virtual IObject* FindObject(int32_t nType, int32_t nId) = 0;
    virtual IObject* FindObject(int32_t nType, const CMyString &strId) = 0;

};

IGlobalManager* getGlobalManager(void);


