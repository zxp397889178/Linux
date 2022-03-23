/*
 * =====================================================================================
 *
 *       Filename:  FileLock.cpp
 *
 *    Description:  linux file lock
 *
 *        Version:  1.0
 *        Created:  2014年11月20日 20时02分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "FileLock.h"



CFileLock::CFileLock(const char *pszFile)
	: m_fd(-1)
	, m_bOpen(false)
{
	int fd = open(pszFile, O_CREAT|O_RDWR, 0755);
	if (fd != -1)
	{
		m_fd = fd;
		m_bOpen = true;
	}
}

CFileLock::CFileLock(int fd)
	: m_fd(-1)
	, m_bOpen(false)
{
	m_fd = fd;
}

CFileLock::~CFileLock()
{
	if (m_bOpen && m_fd!=-1)
	{
		close(m_fd);
	}
}

bool CFileLock::Lock(bool bReadLock)
{
	if (m_fd==-1)
		return false;
	return this->doLock(bReadLock ? F_RDLCK : F_WRLCK, F_SETLKW, 0);
}

bool CFileLock::TryLock(uint32_t nMilliSeconds, bool bReadLock)
{
	if (m_fd==-1)
		return false;
	return this->doLock(bReadLock ? F_RDLCK : F_WRLCK, F_SETLK, 0);
}

void CFileLock::Unlock()
{
	if (m_fd==-1)
		return;
	this->doLock(F_UNLCK, F_SETLK);
}

bool CFileLock::doLock(short lockType, int cmd, off_t start)
{
	struct flock lock;
	lock.l_type = lockType;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_pid = getpid();
	int ret = fcntl(m_fd, cmd, &lock);
	return (ret==0);
}


