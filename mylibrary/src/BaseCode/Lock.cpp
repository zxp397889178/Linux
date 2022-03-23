/*
 * =====================================================================================
 *
 *       Filename:  Lock.cpp
 *
 *    Description:  实现一些lock类功能
 *
 *        Version:  1.0
 *        Created:  2015年09月26日 14时18分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "ILock.h"
#include "Misc.h"
#include "iLog.h"

CGuard::CGuard(ILock *pLocker, bool bReadLock, const char* extra_flag)
	: m_pLocker(pLocker)
{
	if (NULL != extra_flag)
	{
		m_strExtraFlag = extra_flag;
	}
	if (m_pLocker != NULL)
	{
		LogTrace("[pid %u, tid %u %s]try to %s lock"
				, getpid(), gettid(), m_strExtraFlag.c_str()
				, (bReadLock ? "READ" : "WRITE")); 

		m_pLocker->Lock(bReadLock);

		LogTrace("[pid %u, tid %u %s]enter %s lock OK"
				, getpid(), gettid(), m_strExtraFlag.c_str()
				, (bReadLock ? "READ" : "WRITE")); 
				
	}
}

CGuard::~CGuard()
{
	if (m_pLocker != NULL)
	{
		m_pLocker->Unlock();
		LogTrace("[pid %u, tid %u %s]leave lock."
				, getpid(), gettid(), m_strExtraFlag.c_str());
	}
}


