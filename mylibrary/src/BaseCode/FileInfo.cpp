/*
 * =====================================================================================
 *
 *       Filename:  FileInfo.cpp
 *
 *    Description:  implement of FileInfo.h
 *
 *        Version:  1.0
 *        Created:  2014年12月29日 18时37分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "FileInfo.h"
#include "iLog.h"
#include "LogShmWrapper.h"
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



CFileInfo::CFileInfo()
	: m_strFile()
{
}

CFileInfo::~CFileInfo()
{
}

bool CFileInfo::Open(int32_t fd)
{
	// linux下获取程序当前目录绝对路径 
	// http://blog.chinaunix.net/uid-14214482-id-4299670.html
	char buf[256] = "";
	snprintf(buf, sizeof(buf), "/proc/self/fd/%d", fd);
	char full_path[1024] = "";
	ssize_t ret = readlink(buf, full_path, sizeof(full_path)-1);
	if (-1 == ret)
	{
		LogErr("call readlink error, error [%d]%s", errno, strerror(errno));
		return false;
	}

	int32_t ret2 = fstat(fd, &m_stat);
	if (-1 == ret2)
	{
		LogErr("call fstat error, error [%d] %s", errno, strerror(errno));
		return false;
	}
	m_strFile = full_path;
	if (this->IsDirectory())
	{
		m_strFile += "/";
	}
    return true;
}

bool CFileInfo::Open(FILE *fp)
{
    if (NULL == fp)
    {
        return false;
    }
    return this->Open(fileno(fp));
}

bool CFileInfo::Open(const char *pszFile)
{
	//int32_t fd = open(pszFile, O_RDONLY, 0666);
	int32_t fd = open(pszFile, O_RDONLY);
	if (-1 == fd)
	{
		LogErr("can not open file %s to read, errno=%d, msg=%s.", 
                pszFile, errno, strerror(errno));
		return false;
	}
    if (!this->Open(fd))
    {
        close(fd);
        return false;
    }
	close(fd);

	return true;
}

const char* CFileInfo::GetFName(void)
{
	if (this->IsDirectory())
	{
		return "";
	}

	std::string::size_type idx = m_strFile.rfind("/");
	if (idx == std::string::npos)
	{
		return m_strFile.c_str();
	}
	else
	{
		return m_strFile.substr(++idx).c_str();
	}
}

const char* CFileInfo::GetPath(void)
{
	if (this->IsDirectory())
	{
		return m_strFile.c_str();
	}

	std::string::size_type idx = m_strFile.rfind("/");
	if (idx == std::string::npos)
	{
		assert(0);
		return "";
	}
	return m_strFile.substr(0, idx+1).c_str();
}

const char* CFileInfo::GetFullPath(void)
{
	return m_strFile.c_str();
}

uint32_t CFileInfo::GetSize(void)
{
	return m_stat.st_size;
}

time_t CFileInfo::GetLastModifyTime(void)
{
	return m_stat.st_mtime;
}

bool CFileInfo::IsModified(void)
{
	struct stat buf = {0};
	int ret = stat(m_strFile.c_str(), &buf);
	if (-1 ==  ret)
	{
		LogErr("read file [%s] stat failure, error [%d] %s", 
				m_strFile.c_str(), errno, strerror(errno));
		return false;
	}
	if ((buf.st_mtime == m_stat.st_mtime) && (buf.st_ctime == m_stat.st_ctime))
	{
		return false;
	}

	LogDebug("file [%s] has changed.", m_strFile.c_str());
	memcpy(&m_stat, &buf, sizeof(m_stat));
	return true;
}

#define HAS_FLAG(attrs, flag)	(((attrs) & (flag))==(flag))

bool CFileInfo::IsDirectory(void)
{
	return HAS_FLAG(m_stat.st_mode, S_IFDIR);
}

bool CFileInfo::IsFifo(void)
{
	return HAS_FLAG(m_stat.st_mode, S_IFIFO);
}

bool CFileInfo::IsRegular(void)
{
	return HAS_FLAG(m_stat.st_mode, S_IFREG);
}

bool CFileInfo::IsLink(void)
{
	return HAS_FLAG(m_stat.st_mode, S_IFLNK);
}

bool CFileInfo::IsExist(const char *pszFile)
{
	struct stat buf;
	int fd = stat(pszFile, &buf);
	if (0 != fd)
	{
		return false;
	}
	return true;
}

bool CFileInfo::DecodeFile(const char *pszFile,
		char *szFileName, size_t &cbFileName,
		char *szPathName, size_t &cbPathName)
{
	if (NULL == pszFile || '\0' == pszFile[0])
	{
		return false;
	}
	std::string strFile(pszFile);
	std::string::size_type idx = strFile.rfind("/");
	// printf("%s : pos=>%d, len=>%d\n", strFile.c_str(), idx, strFile.length());
	if (idx == std::string::npos || idx == strFile.length())
	{
		// full path
		szFileName[0] = '\0';
		cbFileName = 0;
		cbPathName = strFile.length();
		strcpy(szPathName, strFile.c_str());
		return true;
	}
	cbPathName = idx;
	cbFileName = strFile.length() - idx - 1;
	strcpy(szFileName, strFile.substr(++idx).c_str());
	strcpy(szPathName, strFile.substr(0, idx).c_str());
	return true;
}


