/*
 * =====================================================================================
 *
 *       Filename:  MyRwLock.h
 *
 *    Description:  rwlock object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时11分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_RWLOCK_H_
#define _MYLIB_BASECODE_RWLOCK_H_

#include "../ILock.h"
#include <pthread.h>

class CMyRwLock : public ILock
{
public:
    CMyRwLock();
    ~CMyRwLock();

    virtual bool Lock(bool bReadLock = true);
    virtual bool TryLock(uint32_t nMilliSeconds, bool bReadLock = true);
    virtual void Unlock();

private:
    pthread_rwlock_t            m_LockObj;
    //pthread_rwlockattr_t        m_AttrObj;
};


#endif //_MYLIB_BASECODE_RWLOCK_H_
