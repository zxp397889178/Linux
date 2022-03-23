/*
 * =====================================================================================
 *
 *       Filename:  MemFile.h
 *
 *    Description:  memory map file wrapper
 *
 *        Version:  1.0
 *        Created:  2014/12/04 21时49分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MEMORY_FILE_H_
#define _MYLIB_BASECODE_MEMORY_FILE_H_

#include <sys/types.h>
#include "IException.h"


//exception Bad_MemFile 
class CMemFile
{
public:
	CMemFile();
	CMemFile(const char *pszFile, size_t cbLen, 
			bool bReadOnly=false, bool bPrivate=false);
	~CMemFile();

	int Open(const char *pszFile, size_t cbLen, 
			bool bReadOnly = false, bool bPrivate = false);
	void Close(void);
	bool FlushAll(bool bSyncWrite=true);
	bool Flush(void *ptr, size_t cbLen, bool bSyncWrite=true);

	inline void* GetBasePtr(void) const { return m_pBase; }
	inline size_t GetLength(void) const { return m_cbLength; }

private:
	void*	m_pBase;
	size_t	m_cbLength;
	int32_t	m_fd;
};



#endif //_MYLIB_BASECODE_MEMORY_FILE_H_

