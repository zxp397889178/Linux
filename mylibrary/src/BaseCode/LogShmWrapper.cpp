/*
* =====================================================================================
*
*       Filename:  LogShmWrapper.cpp
*
*    Description:  共享内存日志封装
*
*        Version:  1.0
*        Created:  2015年4月24日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#include "LogShmWrapper.h"
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
using namespace std;

#define CFGMNG_TASK_NAME_LEN 	256

LogShmWrapper::LogShmWrapper()
{
	m_eLevel = levelInfo;
	memset(m_process_name, 0, sizeof(m_process_name));
	m_init_flag = INIT;
	//m_bWithTitle = true;
}

LogShmWrapper::~LogShmWrapper()
{

}

LogShmWrapper &LogShmWrapper::Instance()
{
	static LogShmWrapper s_obj;

	return s_obj;
}

bool LogShmWrapper::Init(const char *xml_file, const char *type)
{
	//得到共享内存连接
	if (!m_shm_data.create(xml_file, type, false))
	{
		cout << "create IPC failed!" << endl;
		m_init_flag = FAILED;
		return false;
	}

	//得到程序名称
	int ret = GetProcessName();
	if (ret == -1)
	{
		m_init_flag = FAILED;
		return false;
	}
	//cout << "m_process_name = " <<m_process_name << endl;
	m_init_flag = SUCCESS;
	return true;
}

void LogShmWrapper::PutLog(enum LogLevel level, const char *fmt, ...)
{
	if (m_init_flag == FAILED || m_init_flag == INIT)
	{
		return;
	}

	if (level < m_eLevel)
	{
		return;
	}

	struct timeval tv = { 0 };
	int ret = gettimeofday(&tv, NULL);
	if (-1 == ret)
	{
		printf("call gettimeofday failure, err:[%d] %s.\n", errno, strerror(errno));
		return;
	}
    struct tm now = {0};
    struct tm *pNow = localtime_r(&tv.tv_sec, &now);
	if (NULL == pNow)
	{
		printf("call localtime_r failure, err:[%d] %s.\n", errno, strerror(errno));
		return;
	}
	char szTitle[256] =  "";

	sprintf(szTitle, "[%02d:%02d:%02d.%03d] <%s:%d> ",
			now.tm_hour, now.tm_min, now.tm_sec, (int32_t)(tv.tv_usec / 1000), m_process_name, getpid());

	char szContent[2048] = "";
	bool bHasColor = true;
	switch (level)
	{
	case LogShmWrapper::levelNone:
		strcat(szContent, "NONE: ");
		strcat(szContent, szTitle);
		bHasColor = false;
		break;
	case LogShmWrapper::levelTrace:
		strcat(szContent, "TRACE:");
		strcat(szContent, szTitle);
		bHasColor = false;
		break;
	case LogShmWrapper::levelDebug:
		strcat(szContent, "DEBUG:");
		strcat(szContent, szTitle);
		bHasColor = false;
		break;
	case LogShmWrapper::levelInfo:
		strcat(szContent, "INFOR:");
		strcat(szContent, szTitle);
		bHasColor = false;
		break;
	case LogShmWrapper::levelImportant:
		strcat(szContent, "IMPOT:");
		strcat(szContent, szTitle);
		strcat(szContent, "\033[1;40;32m");		// GREEN
		break;
	case LogShmWrapper::levelWarn:
		strcat(szContent, "WARNG:");
		strcat(szContent, szTitle);
		strcat(szContent, "\033[1;40;33m");	// YELLOW
		break;
	case LogShmWrapper::levelError:
		strcat(szContent, "ERROR:");
		strcat(szContent, szTitle);
		strcat(szContent, "\033[1;40;31m");	// RED
		break;
	case LogShmWrapper::levelFatalError:
		strcat(szContent, "FATAL:");
		strcat(szContent, szTitle);
		strcat(szContent, "\033[5;40;31m");	// Flicker RED
		break;
	default:
		break;
	}
	//strcat(szContent, szTitle);

	char *pc = &szContent[strlen(szContent)];
	size_t len = sizeof(szContent)-strlen(szContent);
	va_list args;
	va_start(args, fmt);
	//int ret = vsnprintf(pc, len, fmt, args);
	vsnprintf(pc, len, fmt, args);
	va_end(args);
	if (bHasColor)
	{
		strcat(szContent, "\033[0m");
	}
		
	strcat(szContent, "\n");

	m_shm_data.putData(szContent, strlen(szContent));
}

int LogShmWrapper::GetLog(vector<char> &buff)
{
	return m_shm_data.getData(buff);
}

int LogShmWrapper::GetProcessName()
{
	int count = 0;
	int nIndex = 0;
	char chPath[CFGMNG_TASK_NAME_LEN] = "";
	char cParam[100] = "";
	char *cTem = chPath;
	size_t len = sizeof(m_process_name);
	int32_t tmp_len;

	pid_t pId = getpid();
	sprintf(cParam, "/proc/%d/exe", pId);
	count = readlink(cParam, chPath, CFGMNG_TASK_NAME_LEN);
	if (count <0 || count >= CFGMNG_TASK_NAME_LEN)
	{
		printf("Current system not support proc.\n");
		return -1;
	}
	nIndex = count - 1;
	for (; nIndex >= 0; nIndex--)
	{
		if (chPath[nIndex] == '/')
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

	if ((int32_t)len <= tmp_len + 1)
	{
		printf("len (%lu) is less than taskname(%s)'s len.\n", len, cTem);
		return -1;
	}

	strcpy(m_process_name, cTem);
	return 0;
}

