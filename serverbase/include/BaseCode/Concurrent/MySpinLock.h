/*
 * =====================================================================================
 *
 *       Filename:  MySpinLock.h
 *
 *    Description:  spin lock object
 *
 *        Version:  1.0
 *        Created:  2014/11/18 00时05分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 *	 SpinLock 与 Mutex 的比较 http://bbs.chinaunix.net/thread-3573902-1-1.html
 *	 SpinLock 与 Mutex 的区别 http://blog.csdn.net/sunmenggmail/article/details/8105279
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYSPINLOCK_H_
#define _MYLIB_BASECODE_MYSPINLOCK_H_

#include "../mydef.h"
#include "../ILock.h"
#ifdef OS_MAC
// https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/spinlock.3.html
#include <libkern/OSAtomic.h>
#else
#include <pthread.h>
#endif

class CMySpinLock : public ILock
{
public:
    CMySpinLock();
    ~CMySpinLock();

    virtual bool Lock(bool bReadLock = true);
    virtual bool TryLock(uint32_t nTimeout, bool bReadLock = true);
    virtual void Unlock();

private:
#ifdef OS_MAC
	OSSpinLock				m_LockObj;
#else
    pthread_spinlock_t     	m_LockObj;
#endif
};


#endif  //_MYLIB_BASECODE_MYSPINLOCK_H_
