/*
 * =====================================================================================
 *
 *       Filename:  MySpinLock.cpp
 *
 *    Description:  spin-lock object
 *
 *        Version:  1.0
 *        Created:  2014/11/18 00时06分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */


#include "MySpinLock.h"
#include "../TimeFunc.h"
#include <assert.h>


//////////////////////////////////////////////////////////////////////////
//CMySpinLock
CMySpinLock::CMySpinLock()
{
#ifndef OS_MAC
    int ret = pthread_spin_init(&m_LockObj, 0);
    assert(ret==0);
#endif
}

CMySpinLock::~CMySpinLock()
{
#ifndef OS_MAC
    pthread_spin_destroy(&m_LockObj);
#endif
}

bool CMySpinLock::Lock(bool bReadLock)
{
#ifdef OS_MAC
	OSSpinLockLock(&m_LockObj);
	return true;
#else
    int ret = pthread_spin_lock(&m_LockObj);
    return (ret==0);
#endif
}

bool CMySpinLock::TryLock(uint32_t nTimeout, bool bReadLock)
{
    int ret(0);
    uint64_t startTime = Now(true);
	uint64_t endTime = startTime + nTimeout * E3;
    do{
#ifdef OS_MAC
		if ( OSSpinLockTry(&m_LockObj) )
		{
			ret = 0;
		}
#else
        ret = pthread_spin_trylock(&m_LockObj);
#endif
		nsleep(1);
    }while(ret!=0 && Now(true)<endTime);
    return (ret==0);
}

void CMySpinLock::Unlock()
{
#ifdef OS_MAC
	OSSpinLockUnlock(&m_LockObj);
#else
    pthread_spin_unlock(&m_LockObj);
#endif
}

////////////////////////////////////////////////////////////////////////////
//

ILock* OpenSpinLock(void)
{
	return new CMySpinLock;
}


