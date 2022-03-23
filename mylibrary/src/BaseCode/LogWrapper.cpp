/*
 * =====================================================================================
 *
 *       Filename:  LogWrapper.cpp
 *
 *    Description:  log wrapper
 *
 *        Version:  1.0
 *        Created:  2014年12月15日 18时47分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *	 URL  http://www.linxh.blog.chinaunix.net/uid-9093140-id-3450383.html
 * =====================================================================================
 */

#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include "LogWrapper.h"
#include "StringFunc.h"
#include "iLog.h"



#define CFGMNG_TASK_NAME_LEN 	256
#define DEFAULT_LOGFILE			"./trace"

int32_t cfgmng_get_taskname(char *ac, size_t len)
{
	int count = 0;
	int nIndex = 0;
	char chPath[CFGMNG_TASK_NAME_LEN] = "";
	char cParam[100] ="";
	char *cTem = chPath;
	int32_t tmp_len;

	pid_t pId = getpid();
	sprintf(cParam, "/proc/%d/exe", pId);
	count = readlink(cParam, chPath, CFGMNG_TASK_NAME_LEN);
	if (count <0 || count>= CFGMNG_TASK_NAME_LEN)
	{
		printf("Current system not support proc.\n");
		return -1;
	}
	nIndex = count -1;
	for(; nIndex>=0; nIndex--)
	{
		if (chPath[nIndex]=='/')
		{
			nIndex++;
			cTem += nIndex;
			break;
		}
	}
	tmp_len = strlen(cTem);
	if (0 == tmp_len)
	{
		printf("Get task fail.\n");
		return -1;
	}

	if ((int32_t)len <= tmp_len+1)
	{
		printf("len (%lu) is less than taskname(%s)'s len.\n", len, cTem);
		return -1;
	}

	strcpy(ac, cTem);
	return 0;
}



CLogWrapper::CLogWrapper(enum LogLevel lvl, bool bHeldOpen)
	: m_eLevel(lvl)
	, m_file(-1)
	, m_opentime(::time(NULL))
	, m_bConsoleOutput(false)
	, m_bWithTitle(true)
	, m_bHeldOpen(bHeldOpen)
{
	if (m_bHeldOpen)
	{
		bool ret = this->Open(DEFAULT_LOGFILE);
		assert(ret);
	}
}

CLogWrapper::~CLogWrapper()
{
	if (-1 != m_file)
	{
		close(m_file);
		m_file = -1;
	}
}

CLogWrapper& CLogWrapper::Instance()
{
	static CLogWrapper s_obj(levelDebug, false);

	return s_obj;
}

bool CLogWrapper::Open(const char *pszFile)
{
    struct tm now={0};
    localtime_r(&m_opentime, &now);
	char szFile[256] = "";
	sprintf(szFile, "%s_%04d%02d%02d.log",
		pszFile, now.tm_year+1900, now.tm_mon+1, now.tm_mday);
	int32_t ret = open(szFile, O_CREAT|O_APPEND|O_RDWR, 0666);
	if (-1 == ret)
	{
		printf("open file [%s] failure, error: [%d]%s\n", szFile,
					errno, strerror(errno));
		return false;
	}

	m_file = ret;
	return true;
}

void CLogWrapper::LogSave(enum LogLevel level, const char *fmt, ...)
{
	if (level < m_eLevel)
	{
		return;
	}

	struct timeval tv = {0};
	int ret = gettimeofday(&tv, NULL);
	if (-1==ret)
	{
		printf("call gettimeofday failure, err:[%d] %s.\n", errno, strerror(errno) );
		return;
	}
    struct tm now = {0};
    struct tm *pNow = localtime_r(&tv.tv_sec, &now);
	if (NULL==pNow)
	{
		printf("call localtime_r failure, err:[%d] %s.\n", errno, strerror(errno) );
		return;
	}
	char szTitle[2][256] = {"", };
	char szTask[256] = "";
	cfgmng_get_taskname(szTask, sizeof(szTask));
	if (m_bWithTitle)
	{
		sprintf(szTitle[0], "[%02d:%02d:%02d.%-3d] <%s:%d> ", 
			now.tm_hour, now.tm_min, now.tm_sec, (int32_t)(tv.tv_usec/1000), szTask, getpid());
		sprintf(szTitle[1], "[%02d:%02d:%02d.%-3d] ", 
			now.tm_hour, now.tm_min, now.tm_sec, (int32_t)(tv.tv_usec/1000) );
	}
	else
	{
		strcpy(szTitle[0], " ");
		strcpy(szTitle[1], " ");
	}

	char szContent[2048] = "";
	bool bHasColor(true);
	if (level == levelFatalError)
	{
		strcat(szContent, "\033[5;40;31m");	// Flicker RED
		if (m_bWithTitle)
		{
			strcat(szContent, "FATAL:");
		}
	}
	else if (level == levelError)
	{
		strcat(szContent, "\033[1;40;31m");	// RED
		if (m_bWithTitle)
		{
			strcat(szContent, "ERR:");
		}
	}
	else if (level == levelWarn)
	{
		strcat(szContent, "\033[1;40;33m");	// YELLOW
		if (m_bWithTitle)
		{
			strcat(szContent, "WRN:");
		}
	}
	else if (level == levelImportant)
	{
		strcat(szContent, "\033[1;40;32m");		// GREEN
	}
	else if (level == levelDebug)
	{
		if (m_bWithTitle)
		{
			strcat(szContent, "DEBUG:");
		}
		bHasColor = false;
	}
	else
	{
		bHasColor = false;
	}

	char *pc = &szContent[strlen(szContent)];
	size_t len = sizeof(szContent) - strlen(szContent);
	va_list args;
	va_start(args, fmt);
	//int ret = vsnprintf(pc, len, fmt, args);
	vsnprintf(pc, len, fmt, args);
	va_end(args);

	if (bHasColor)
		strcat(szContent, "\033[0m");
	strcat(szContent, "\n");

	// save to file
	bool openfile(true);
	if (m_bHeldOpen)
	{
		if (!this->IsSameDay())
		{
			close(m_file);
			m_file = -1;
			openfile = this->Open(DEFAULT_LOGFILE);
			assert(openfile);
			m_opentime = ::time(NULL);
		}
	}
	else
	{
		openfile = this->Open(DEFAULT_LOGFILE);
		assert(openfile);
	}
	if (openfile)
	{
		write(m_file, szTitle[0], strlen(szTitle[0]) );
		write(m_file, szContent, strlen(szContent) );
		if (!m_bHeldOpen)
		{
			close(m_file);
			m_file = -1;
		}
	}

	if (m_bConsoleOutput)
	{
		fprintf(stdout, "%s%s",szTitle[1], szContent);
		fflush(stdout);
	}
}

bool CLogWrapper::IsSameDay(void)
{
	time_t t1 = ::time(NULL);
    struct tm now = {0};
    struct tm o1 = {0};
    localtime_r(&t1, &now);
    localtime_r(&m_opentime, &o1);
	if (now.tm_year == o1.tm_year &&
		now.tm_mon == o1.tm_mon &&
		now.tm_mday == o1.tm_mday)
	{
		return true;
	}

	return false;
}

const char* color_text(const char* color, const char *str, CMyString &str1)
{
    str1.Format("%s%s%s", color, str, _color_end);
    return str1.c_str();
}

const char* color_int(const char* color, int32_t val, CMyString &str1)
{
    str1.Format("%s%d%s",  color, val, _color_end);
    return str1.c_str();
}

const char* color_uint(const char* color, uint32_t val, CMyString &str1)
{
    str1.Format("%s%u%s",  color, val, _color_end);
    return str1.c_str();
}

const char* color_int64(const char* color, int64_t val, CMyString &str1)
{
    str1.Format("%s%ld%s",  color, val, _color_end);
    return str1.c_str();
}

const char* color_uint64(const char* color, uint64_t val, CMyString &str1)
{
    str1.Format("%s%lu%s",  color, val, _color_end);
    return str1.c_str();
}

const char* color_double(const char* color, double val, CMyString &str1)
{
    CMyString str2 = double_to_str(val, 4);
    str1.Format("%s%s%s",  color, str2.c_str(), _color_end);
    return str1.c_str();
}


