/*
 * =====================================================================================
 *
 *       Filename:  MyRwLock.cpp
 *
 *    Description:  rwlock object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时11分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#include "MyRwLock.h"
#include "../TimeFunc.h"


//////////////////////////////////////////////////////////////////////////
CMyRwLock::CMyRwLock()
{
    pthread_rwlock_init(&m_LockObj, NULL);
}

CMyRwLock::~CMyRwLock()
{
    pthread_rwlock_destroy(&m_LockObj);
}

bool CMyRwLock::Lock(bool bReadLock /* = true */)
{
    int ret(0);
    if (bReadLock)
        ret = pthread_rwlock_rdlock(&m_LockObj);
    else
        ret = pthread_rwlock_wrlock(&m_LockObj);
    return (ret==0);
}

bool CMyRwLock::TryLock(uint32_t nMilliSeconds, bool bReadLock /* = true */)
{
    int ret(-1);
	uint64_t startTime = Now();
	do{
		if (bReadLock)
		{
			ret = pthread_rwlock_tryrdlock(&m_LockObj);
		}
		else
		{
			ret = pthread_rwlock_trywrlock(&m_LockObj);
		}
	}while(ret!=0 && Now()<(startTime+nMilliSeconds));

	return (ret==0);
}

void CMyRwLock::Unlock()
{
    pthread_rwlock_unlock(&m_LockObj);
}


////////////////////////////////////////////////////////////////////////////
//
ILock* OpenRwLock(void)
{
	CMyRwLock *pObj = new CMyRwLock;
	return pObj;
}
