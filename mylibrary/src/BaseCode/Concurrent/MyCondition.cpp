/*
 * =====================================================================================
 *
 *       Filename:  MyCondition.cpp
 *
 *    Description:  condition object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时37分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#include "MyCondition.h"
#include <stdio.h>
#include <assert.h>
#include "../TimeFunc.h"



CMyCondition::CMyCondition()
{
    pthread_mutex_init(&m_LockObj, NULL);
    pthread_cond_init(&m_CondObj, NULL);
}

CMyCondition::~CMyCondition()
{
    pthread_mutex_destroy(&m_LockObj);
    pthread_cond_destroy(&m_CondObj);
}

bool CMyCondition::Wait(uint32_t nTimeout)
{
    int ret(0);
    if ((uint32_t)-1 == nTimeout)
    {
        ret = pthread_cond_wait(&m_CondObj, &m_LockObj);
    }
    else
    {
        // http://blog.csdn.net/hairetz/article/details/4535920
        struct timespec timeout;
		timeout.tv_sec = time(NULL) + (nTimeout/E3);
		timeout.tv_nsec = (nTimeout%E3)*E6;
        ret = pthread_cond_timedwait(&m_CondObj, &m_LockObj, &timeout);
/* 		
		switch(ret)
		{
		case ETIMEDOUT:
			// Handle timeout
			break;
		case EINTR:
			// interupted by signal
			break;
		case 0:
			// condition received a condition signal
			break;
		case EBUSY:
		default:
			// Handle errors
			break;
		}
*/		
    }
    return (ret==0);
}

bool CMyCondition::Signal()
{
    int ret = pthread_cond_signal(&m_CondObj);
    return (ret==0);
}

bool CMyCondition::Broadcast()
{
    int ret = pthread_cond_broadcast(&m_CondObj);
    return (ret==0);
}

bool CMyCondition::Lock(bool bReadLock)
{
    int ret = pthread_mutex_lock(&m_LockObj);
    return (ret==0);
}

bool CMyCondition::TryLock(uint32_t nTimeout, bool bReadLock)
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

void CMyCondition::Unlock()
{
    int ret = pthread_mutex_unlock(&m_LockObj);
    assert(ret==0);
}


////////////////////////////////////////////////////////////////////////////
//

ICondition* OpenCondition(void)
{
	return new CMyCondition();
}

