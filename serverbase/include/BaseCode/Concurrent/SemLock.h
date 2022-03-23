/*
 * =====================================================================================
 *
 *       Filename:  SemLock.h
 *
 *    Description:  http://blog.csdn.net/turkeyzhou/article/details/8610239
 *
 *        Version:  1.0
 *        Created:  2014年11月20日 20时36分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_SEMLOCK_H_
#define _MYLIB_BASECODE_SEMLOCK_H_

#include "../ILock.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class CSemLock : public ILock
{
public:
	CSemLock();
	explicit CSemLock(key_t iKey);
	~CSemLock();

	void init(key_t iKey);

	virtual bool Lock(bool bReadLock);
	virtual bool TryLock(unsigned long nMilliSeconds, bool bReadLock);
	virtual void Unlock();

private:
	int rlock(void) const;
	int unrlock(void) const;
	bool tryrlock(void) const;
	int wlock(void) const;
	int unwlock(void) const;
	bool trywlock(void) const;

private:
	// 信号号 ID
	int 	m_semID;
	// 信号量 key
	key_t 	m_semKey;


	bool	m_bReadLock;
};


#endif //_MYLIB_BASECODE_SEMLOCK_H_
