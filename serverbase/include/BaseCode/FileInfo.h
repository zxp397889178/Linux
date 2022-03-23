/*
 * =====================================================================================
 *
 *       Filename:  FileInfo.h
 *
 *    Description:  wrapper of file info
 *
 *        Version:  1.0
 *        Created:  2014年12月29日 18时35分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *   URL: http://www.cnitblog.com/guopingleee/archive/2008/11/13/51411.aspx
 * =====================================================================================
 */

#ifndef _MYLIB_P2PWRAPPER_FILE_INFO_H_
#define _MYLIB_P2PWRAPPER_FILE_INFO_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <stdint.h>

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

    bool Open(const char *pszFile);
    bool Open(int32_t fd);
    bool Open(FILE *fp);

	bool IsModified(void);

	const char* GetFName(void);
	const char* GetPath(void);
	const char* GetFullPath(void);
	uint32_t   GetSize(void);
	time_t	   GetLastModifyTime(void);

	bool IsDirectory(void);
	bool IsFifo(void);
	bool IsRegular(void);
	bool IsLink(void);

public:
	static bool IsExist(const char *pszFile);
	static bool DecodeFile(const char *pszFile, 
			char *szFileName, size_t &cbFileName,
			char *szPathName, size_t &cbPathName);
private:
	std::string		m_strFile;
	struct stat		m_stat;
};

#endif	// _MYLIB_P2PWRAPPER_FILE_INFO_H_

