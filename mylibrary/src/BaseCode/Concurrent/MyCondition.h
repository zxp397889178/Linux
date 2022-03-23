/*
 * =====================================================================================
 *
 *       Filename:  MyCondition.h
 *
 *    Description:  conditon object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时36分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MY_CONDITION_H_
#define _MYLIB_BASECODE_MY_CONDITION_H_

#include "../ILock.h"
#include <pthread.h>


class CMyCondition : public ICondition
{
public:
    CMyCondition();
    ~CMyCondition();

    virtual bool Wait(uint32_t nTimeout);
    virtual bool Signal();
    virtual bool Broadcast();

	// ILock
	virtual bool Lock(bool bReadLock);
	virtual bool TryLock(uint32_t nTimeout, bool bReadLock);
	virtual void Unlock(void);

private:
    pthread_cond_t      m_CondObj;
    pthread_mutex_t     m_LockObj;
};


#endif //_MYLIB_BASECODE_MY_CONDITION_H_ 
