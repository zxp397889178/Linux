/*
 * =====================================================================================
 *
 *       Filename:  FileLock.h
 *
 *    Description:  linux file lock
 *
 *        Version:  1.0
 *        Created:  2014年11月20日 19时58分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_FILE_LOCK_H_
#define _MYLIB_BASECODE_FILE_LOCK_H_

#include <unistd.h>
#include <fcntl.h>
#include "../ILock.h"

class CFileLock : public ILock
{
public:
	explicit CFileLock(const char *pszFile);
	explicit CFileLock(int fd);
	~CFileLock();

	virtual bool Lock(bool bReadLock);
	virtual bool TryLock(uint32_t nMilliSeconds, bool bReadLock);
	virtual void Unlock();

private:
	bool doLock(short lockType, int cmd, off_t start=0);

private:
	int 	m_fd;
	bool	m_bOpen;
};


#endif // _MYLIB_BASECODE_FILE_LOCK_H_
