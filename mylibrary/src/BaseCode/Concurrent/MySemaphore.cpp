/*
 * =====================================================================================
 *
 *       Filename:  MySemaphore.cpp
 *
 *    Description:  semaphore object
 *
 *        Version:  1.0
 *        Created:  2014/11/17 23时58分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  skyunion
 *
 * =====================================================================================
 */

#include <iostream>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "MySemaphore.h"
#include "../TimeFunc.h"
#include "../myconf.h"

extern const char* GetThreadUniqueName(char *szText, size_t cbText);

#define FILE_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)


CMySemaphore::CMySemaphore(uint32_t nInitValue)
	: m_pLockObj(NULL)
	, m_strSemaphore()
{
#ifdef OS_MAC
	char szSemaphore[128] = "";
	GetThreadUniqueName(szSemaphore, sizeof(szSemaphore));
	sem_t *pSem = sem_open(szSemaphore, O_CREAT | O_RDWR, FILE_MODE, nInitValue);
	if (pSem == SEM_FAILED)
	{
		//CMyString str("[%d]%s", errno, strerror(errno));
		//THROW(Bad_lockobj, str.c_str());
		THROW_EX(Bad_lockobj(errno));
	}
	else
	{
		m_pLockObj = pSem;
		m_strSemaphore = szSemaphore;
	}
#else
	m_pLockObj = (sem_t*)calloc(1, sizeof(sem_t));
	if (NULL == m_pLockObj)
	{
		THROW_EX(Bad_lockobj(errno));
	}
	else
	{
	    int ret = sem_init(m_pLockObj, 1, nInitValue);
		if (ret != 0)
		{
			free(m_pLockObj);
			THROW_EX(Bad_lockobj(errno));
		}
	}
#endif
}

CMySemaphore::CMySemaphore(const char *pszName, uint32_t nInitValue)
	: m_pLockObj(NULL)
	, m_strSemaphore(pszName)
{
	m_pLockObj = sem_open(pszName, O_CREAT|O_RDWR, FILE_MODE, nInitValue);
	if (m_pLockObj == SEM_FAILED)
	{
		//CMyString str("[%d]%s", errno, strerror(errno));
		//THROW(Bad_lockobj, str.c_str());
		THROW_EX(Bad_lockobj(errno));
	}
}

CMySemaphore::~CMySemaphore()
{
	if (NULL != m_pLockObj)
	{
#ifdef OS_MAC
		sem_close(m_pLockObj);
		if (!m_strSemaphore.empty())
		{
			sem_unlink(m_strSemaphore.c_str());
		}
#else
		if (m_strSemaphore.empty())
		{
    		sem_destroy(m_pLockObj);
			free(m_pLockObj);
		}
		else
		{
			sem_close(m_pLockObj);
			sem_unlink(m_strSemaphore.c_str());
		}
#endif
	}
	m_pLockObj = NULL;
}

bool CMySemaphore::Post(void)
{
    int ret = sem_post(m_pLockObj);
    return (ret == 0);
}

bool CMySemaphore::Wait(void)
{
    int ret = sem_wait(m_pLockObj);
	return (ret==0);
}

bool CMySemaphore::TryWait(uint32_t nTimeout)
{
    int ret(0);
    if (nTimeout == (uint32_t)-1)
    {
        ret = sem_wait(m_pLockObj);
		return (ret==0);
    }
	else if (0==nTimeout)
	{
		ret = sem_trywait(m_pLockObj);
		return (ret==0);
	}
	else
	{
#ifdef OS_MAC
		uint64_t nStart = Now();
		do{
			ret = sem_trywait(m_pLockObj);
			if (ret!=0)
			{
				usleep(500);
			}
		}while(0==ret || Now()>=nStart+nTimeout);
#else
	    struct timespec timeout;
		timeout.tv_sec = (nTimeout/E3);
		timeout.tv_nsec = (nTimeout%E3)*E6;
    	ret = sem_timedwait(m_pLockObj, &timeout);
#endif
	}
    return (ret==0);
}

bool CMySemaphore::IsLocked(void)
{
	int val(0);
#ifdef OS_MAC
	int ret = semctl(m_pLockObj, 0, GETVAL, &val);
	return (0==ret && val!=0);
#else
	int ret = sem_getvalue(m_pLockObj, &val);
	return (0==ret && val!=0);
#endif	
}

///////////////////////////////////////////////////////////////////////////
//

ISemaphore* OpenSemaphore(uint32_t nInitVal)
{
	CMySemaphore *pSem(NULL);
	try
	{
		pSem = new CMySemaphore(nInitVal);
	}
	catch(std::exception &e)
	{
		fprintf(stderr, "\033[0;40;31m%s\033[0m\n", e.what());
		pSem = NULL;
	}
	catch(...)
	{
		pSem = NULL;
	}
	return pSem;
}

ISemaphore* OpenSemaphore(const char *pszName, uint32_t nInitVal)
{
	CMySemaphore *pSem(NULL);
	try
	{
		pSem = new CMySemaphore(pszName, nInitVal);
	}
	catch(std::exception &e)
	{
		fprintf(stderr, "\033[0;40;31m%s\033[0m\n", e.what());
		pSem = NULL;
	}
	catch(...)
	{
		pSem = NULL;
	}
	
	return pSem;
}


