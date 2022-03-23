/*
 * =====================================================================================
 *
 *       Filename:  ILock.h
 *
 *    Description:  lock object interface
 *
 *        Version:  1.0
 *        Created:  2014/11/17 22时21分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_LOCK_H_
#define _MYLIB_BASECODE_LOCK_H_

#include <stddef.h>
#include <string>
#include "IException.h" 
#include "mydef.h"

//exception Bad_lockobj
class ILock
{
public:
    virtual ~ILock() {}

    virtual bool Lock(bool bReadLock = true) = 0;
    virtual bool TryLock(uint32_t nTimeout, bool bReadLock = true) = 0;
    virtual void Unlock() = 0;
};

class ISemaphore
{
public:
	virtual ~ISemaphore() {};

	virtual bool Wait(void) = 0;
	virtual bool TryWait(uint32_t nTimeout) = 0;
	virtual bool Post(void) = 0;
};

class ICondition : public ILock
{
public:
	virtual ~ICondition() {};

    virtual bool Wait(uint32_t nTimeout = (uint32_t)-1) = 0;
    virtual bool Signal() = 0;
    virtual bool Broadcast() = 0;
};

///////////////////////////////////////////////////////////////////////////////
//CGuard
#if 0
class  CGuard
{
public:
    CGuard(ILock *pLocker, bool bReadLock = true)
        : m_pLocker(pLocker)
    {
        if (NULL != m_pLocker)
        {
            m_pLocker->Lock(bReadLock);
        }
    }
    ~CGuard()
    {
        if (NULL != m_pLocker)
        {
            m_pLocker->Unlock();
        }
    }
private:
    ILock*      m_pLocker;
};
#endif

class CGuard
{
public:
    CGuard(ILock *pLocker, bool bReadLock = true, const char* extra_flag = NULL);
    ~CGuard();
private:
    ILock*      m_pLocker;
	std::string	m_strExtraFlag;
};

#define GUARD(lock, readonly)	CGuard guard(lock, readonly, __FUNCTION__)
#define READ_GUARD(lock)		CGuard guard(lock, true, __FUNCTION__)
#define WRITE_GUARD(lock)		CGuard guard(lock, false, __FUNCTION__)

///////////////////////////////////////////////////////////////////////////////
//Common Function

ILock* OpenMutex(void);
ILock* OpenRwLock(void);
ILock* OpenSpinLock(void);

ISemaphore* OpenSemaphore(uint32_t nInitVal=0);
ISemaphore* OpenSemaphore(const char *pszName, uint32_t nInitVal=0);

ICondition* OpenCondition(void);


#endif // _MYLIB_BASECODE_LOCK_H_
