/*
 * =====================================================================================
 *
 *       Filename:  MyMutex.cpp
 *
 *    Description:  mutex object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 22时25分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#include <assert.h>
#include "MyMutex.h"
#include "../TimeFunc.h"


//////////////////////////////////////////////////////////////////////////
//CMyMutex

CMyMutex::CMyMutex()
{
    int ret = pthread_mutex_init(&m_LockObj, NULL);
    assert(ret==0);
}

CMyMutex::~CMyMutex()
{
    pthread_mutex_destroy(&m_LockObj);
}

bool CMyMutex::Lock(bool bReadLock)
{
    int ret = pthread_mutex_lock(&m_LockObj);
    return (ret==0);
}

bool CMyMutex::TryLock(uint32_t nTimeout, bool bReadLock)
{
    uint64_t timeStart = Now(true);
	uint64_t timeEnd = timeStart+nTimeout*E3;
    int ret(0);
    do{
        ret = pthread_mutex_trylock(&m_LockObj);
        //PthreadSleep(10);
		nsleep(10);
    }while(ret!=0 && (Now(true) < timeEnd) );
    return (ret==0);
}

void CMyMutex::Unlock()
{
    int ret = pthread_mutex_unlock(&m_LockObj);
    assert(ret==0);
}


///////////////////////////////////////////////////////////////////////////
//
ILock* OpenMutex(void)
{
	CMyMutex *pMutex = new CMyMutex;
	return pMutex;
}


