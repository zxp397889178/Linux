/*
 * =====================================================================================
 *
 *       Filename:  MyBarrier.cpp
 *
 *    Description:  barrier object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时49分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#include "MyBarrier.h"



#ifndef OS_MAC

CMyBarrier::CMyBarrier(void)
{
}


CMyBarrier::~CMyBarrier(void)
{
    pthread_barrier_destroy( &m_LockObj );
}

bool CMyBarrier::Init(unsigned long nCount)
{
    int ret = pthread_barrier_init( &m_LockObj, NULL, nCount);
    return (ret==0);
}

bool CMyBarrier::Wait()
{
    int ret = pthread_barrier_wait( &m_LockObj );
    return (ret==0);
}

#endif  //OS_MAC
