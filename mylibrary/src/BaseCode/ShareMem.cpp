/*
 * =====================================================================================
 *
 *       Filename:  ShareMem.cpp
 *
 *    Description:  implement of shmget
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 16时51分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "ShareMem.h"
#include "myconf.h"



CShareMem::CShareMem()
	: m_ptr((void*)-1)
	, m_id(-1)
	, m_size(0)
	, m_bValid(false)  
{
}

CShareMem::~CShareMem()
{
	this->Close();
}

CShareMem::CShareMem(key_t key_, uint32_t size_, int32_t mode)
	: m_ptr((void*)(-1))
	, m_id(-1)
	, m_size(0)
	, m_bValid(false)
{
	int ret = this->Open(key_, size_);
	if (ret != errOK)
	{
		THROW_EX(Bad_ShareMem(errno));
	}
}

CShareMem::CShareMem(const char *pszFile, uint32_t size_, int32_t mode)
	: m_ptr((void*)(-1))
	, m_id(-1)
	, m_size(0)
	, m_bValid(false)
{
	int ret = this->Open(pszFile, size_, mode);
	if (ret != errOK)
	{
		THROW_EX(Bad_ShareMem(errno));
	}
}

int32_t CShareMem::Open(key_t key_, uint32_t size_, int32_t mode)
{
	// open share memory handle
	m_id = shmget(key_, size_, IPC_CREAT|mode);
	if (-1 == m_id)
	{
		return errShmgetError;
	}

	// get the real size
	struct shmid_ds shmbuf;
	int ret = shmctl(m_id, IPC_STAT, &shmbuf);
	if (-1 != ret)
	{
		m_size = shmbuf.shm_segsz;
	}

	//attach the share buffer
	m_ptr = shmat(m_id, NULL, 0);
	if (m_ptr == (void*)(-1))
	{
		return errShmatFailure;
	}

	m_bValid = true;
	return errOK;
}

int32_t CShareMem::Open(const char *pszFile, uint32_t size_, int32_t mode)
{
	int ret = access(pszFile, F_OK);
	if (-1 == ret)
	{
		// FILE_NOT_FOUND, create new
		int fd = open(pszFile, O_CREAT, DEFFILEMODE);
		if (-1 == fd)
		{
			return errFileNotFound;
		}
		close(fd);
	}
	ret = access(pszFile, R_OK|W_OK);
	if (-1 == ret)
	{
		// NOT PRIVILEGE
		return errFileNotPermission;
	}
	key_t key = ftok(pszFile, 0);
	if (key < 0)
	{
		return errFiletokError;
	}

	return this->Open(key, size_, mode);
}

void CShareMem::Close(void)
{
	if (m_ptr != (void*)(-1))
	{
		shmdt(m_ptr);
		m_ptr = (void*)(-1);
		m_id = -1;
		m_size = 0;
	}
	m_bValid = false;
}

void CShareMem::Remove(void)
{
	int save_id = m_id;
	this->Close();
	if (save_id != -1)
	{
		int ret = shmctl(save_id, IPC_RMID, 0);
		if (ret < 0)
		{
			fprintf(stderr, "\033[0;40;31m[%d] %s\033[0m\n", errno, strerror(errno));
		}
	}
}


////////////////////////////////////////////////////////////////////////////
//
int RemoveShareMem(const char *pszFile)
{
	key_t key = ftok(pszFile, 0);
	if (key <0 )
	{
		fprintf(stderr, "\033[0;40;31m[%d] %s\033[0m\n", errno, strerror(errno));
		return -1;
	}
	return RemoveShareMem(key);
}

int RemoveShareMem(key_t key_)
{
	// open share mem object	
	int32_t id = shmget(key_, 0, IPC_CREAT|IPC_EXCL|0666);
	if (-1 == id)
	{
		fprintf(stderr, "\033[0;40;31m[%d] %s\033[0m\n", errno, strerror(errno));
		return -2;
	}

	int ret = shmctl(id, IPC_RMID, 0);
	if (ret < 0)
	{
		fprintf(stderr, "\033[0;40;31m[%d] %s\033[0m\n", errno, strerror(errno));
		return -3;
	}

	return 0;
}

