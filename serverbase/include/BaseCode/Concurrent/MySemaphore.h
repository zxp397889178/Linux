/*
 * =====================================================================================
 *
 *       Filename:  MySemaphore.h
 *
 *    Description:  semaphore object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时58分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *   http://www.cnblogs.com/Anker/archive/2013/01/13/2858765.html
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MY_SEMAPHORE_H_
#define _MYLIB_BASECODE_MY_SEMAPHORE_H_

#include "../mydef.h"
#include "../ILock.h"
#include "../MyString.h"
//#define _XOPEN_SOURCE 600
#include <semaphore.h>

class  CMySemaphore : public ISemaphore
{
    enum { TRY_WAIT_ONCE = 5 };
public:
    explicit CMySemaphore(uint32_t nInitValue);
	CMySemaphore(const char *pszName, uint32_t nInitValue);
    ~CMySemaphore();

	// ISemaphore
    virtual bool Post(void);
    virtual bool Wait(void);
	virtual bool TryWait(uint32_t nTimeout);
	virtual bool IsLocked(void);

private:
	sem_t*		m_pLockObj;
	CMyString	m_strSemaphore;
};


#endif  //_MYLIB_BASECODE_MY_SEMAPHORE_H_

