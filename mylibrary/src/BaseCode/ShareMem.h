/*
 * =====================================================================================
 *
 *       Filename:  ShareMem.h
 *
 *    Description:  share memory with shmget
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 16时49分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_SHARE_MEMORY_H_
#define _MYLIB_BASECODE_SHARE_MEMORY_H_

#include <sys/stat.h>	// S_IRUSR,...
#include "mydef.h"
#include "IException.h"

#define SHARE_MEM_MODE		(S_IRUSR|S_IWUSR|S_IRGRP)							//0640
//#define SHARE_FILE_MODE		(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)	//0666
#define SHARE_FILE_MODE		DEFFILEMODE

//exception Bad_ShareMem
class CShareMem
{
	enum ErrorCode
	{
		errOK = 0,
		errFileNotFound = -1,
		errFileNotPermission = -2,
		errFiletokError = -3,
		errShmgetError = -4,
		errShmatFailure = -5,
	};

public:
	CShareMem();
	CShareMem(key_t key_, uint32_t size_, int32_t mode=SHARE_MEM_MODE);
	CShareMem(const char *pszFile, uint32_t size_, int32_t mode=SHARE_MEM_MODE);
	~CShareMem();

	// return code is enum ErrorCode
	int32_t Open(key_t key_, uint32_t size_, int32_t mode=SHARE_MEM_MODE);
	int32_t Open(const char *pszFile, uint32_t size_, int32_t mode=SHARE_MEM_MODE);
	void Close(void);
	void Remove(void);

	inline bool IsValid(void) const  { return m_bValid; }
	inline bool GetSize(void) const  { return m_size; }
	inline void* GetPtr(void) const  { return m_ptr; }

private:
	void*		m_ptr;
	int32_t		m_id;
	int32_t		m_size;
	bool		m_bValid;
};


int RemoveShareMem(const char *pszFile);
int RemoveShareMem(key_t key_);


#endif // _MYLIB_BASECODE_SHARE_MEMORY_H_
