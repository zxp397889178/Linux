/*
 * =====================================================================================
 *
 *       Filename:  MyMutex.h
 *
 *    Description:  mutex object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 22时25分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYMUTEX_H_
#define _MYLIB_BASECODE_MYMUTEX_H_

#include "../ILock.h"
#include <pthread.h>


class CMyMutex : public ILock
{
public:
    CMyMutex();
    ~CMyMutex();

    virtual bool Lock(bool bReadLock = true);
    virtual bool TryLock(uint32_t nTimeout, bool bReadLock = true);
    virtual void Unlock();

private:
    pthread_mutex_t		m_LockObj;
};


#endif //_MYLIB_BASECODE_MYMUTEX_H_

