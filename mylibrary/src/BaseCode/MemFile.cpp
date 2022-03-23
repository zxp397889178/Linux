/*
 * =====================================================================================
 *
 *       Filename:  MemFile.cpp
 *
 *    Description:  memory map file wrapper
 *
 *        Version:  1.0
 *        Created:  2014/12/04 21时52分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *	 URL http://see.xidian.edu.cn/cpp/html/138.html
 * =====================================================================================
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "MemFile.h"
#include "myconf.h"
#include <algorithm>


#define FILE_PERMS	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

CMemFile::CMemFile()
	: m_pBase(NULL)
	, m_cbLength(0)
	, m_fd(-1)
{

}

CMemFile::~CMemFile()
{
	if (NULL != m_pBase)
	{
		this->FlushAll(true);
		this->Close();
	}
}

CMemFile::CMemFile(const char *pszFile, size_t cbLen, bool bReadOnly,bool bPrivate)
	: m_pBase(NULL)
	, m_cbLength(0)
{
	int ret = this->Open(pszFile, cbLen, bReadOnly, bPrivate);
	if (ret != 0)
	{
		THROW_EX(Bad_MemFile(errno));
	}
}

int CMemFile::Open(const char *pszFile, size_t cbLen, bool bReadOnly,bool bPrivate)
{
	int fd = open(pszFile, O_CREAT|O_RDWR, FILE_PERMS);
	if (fd <= 0)
	{
		return -1;
	}
	int prot = PROT_READ;
	struct stat sb;
	int ret = fstat(fd, &sb);
	if (-1 == ret)
	{
		close(fd);
		return -2;
	}

	if (bReadOnly)
	{
		cbLen = sb.st_size;
	}
	else
	{
		if (cbLen < (size_t)sb.st_size)
			cbLen = sb.st_size;
		lseek64(fd, cbLen, SEEK_SET);
		write(fd, "\0", 1);
		lseek64(fd, 0, SEEK_SET);
	}
	prot = PROT_READ|PROT_WRITE;
	int flag = bPrivate ? MAP_PRIVATE : MAP_SHARED;
	void *pc = mmap(NULL, cbLen, prot, flag, fd, 0);
	if (pc == MAP_FAILED)
	{
		close(fd);
		return -3;
	}
	m_fd = fd;
	close(fd);
	m_fd = -1;

	m_pBase = pc;
	m_cbLength = cbLen;
	return 0;
}

void CMemFile::Close(void)
{
	int ret = munmap(m_pBase, m_cbLength);
	if (-1 == ret)
	{
		printf("on close\n");
		fprintf(stderr, "\033[0;40;31m[%d]%s\033[0m\n", errno, strerror(errno));
	}

	if (-1 != m_fd)
	{
		close(m_fd);
		m_fd = -1;
	}
}

bool CMemFile::FlushAll(bool bSyncWrite)
{
	int flags = bSyncWrite ? MS_SYNC : MS_ASYNC;
	int ret = msync(m_pBase, m_cbLength, flags);
	if (-1==ret)
	{
		printf("on flush.\n");
		fprintf(stderr, "\033[0;40;31m[%d]%s\033[0m\n", errno, strerror(errno));
		return false;
	}
	return true;
}

bool CMemFile::Flush(void *ptr, size_t cbLen, bool bSyncWrite)
{
	char *p1 = (char*)ptr;
	char *p2 = (char*)m_pBase;
	if (p1 < p2 || p1>p2+m_cbLength)
	{
		fprintf(stderr, "\033[0;40;31mflush address invalid\033[0m\n");
		return false;
	}
	if (p1+cbLen > p2+m_cbLength)
	{
		cbLen = p2 + m_cbLength - p1;
	}

	int flags = bSyncWrite ? MS_SYNC : MS_ASYNC;
	int ret = msync(ptr, cbLen, flags);
	if (-1==ret)
	{
		fprintf(stderr, "\033[0;40;31m[%d]%s\033[0m\n", errno, strerror(errno));
		return false;
	}
	return true;
}

