/*
* =====================================================================================
*
*       Filename:  LogShmWrapper.h
*
*    Description:  �����ڴ���־��װ
*
*        Version:  1.0
*        Created:  2015��4��24�� 9ʱ0��0��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _LogShmWrapper_H_
#define _LogShmWrapper_H_
#include "ShmDataQueue.h"
#include <vector>
using namespace std;

class LogShmWrapper
{
public:
	enum INIT_FLAG		//��ʼ����־
	{	
		INIT = 0,		//δ��ʼ��
		SUCCESS = 2,	//��ʼ�����
		FAILED = 9		//��ʼ��ʧ��
	};

	~LogShmWrapper();

	enum LogLevel
	{
		levelNone = 0,			// 0 default is not record
		levelTrace,				 // 1 trace, sometimes same as levelNone
		levelDebug,				 // 2 debug msg
		levelInfo,				 // 3 normal information
		levelImportant,			 // 4 important information
		levelWarn,				 // 5 warnning, but not error
		levelError,				 // 6 error message
		levelFatalError,		 // 7 Fatal error, system abort
	};

	static LogShmWrapper &Instance();

	bool Init(const char *xml_file, const char *type);

	void PutLog(enum LogLevel level, const char *fmt, ...);

	int GetLog(vector<char> &buff);

public:
	enum LogLevel GetLogLevel()
	{
		return m_eLevel;
	}
	void SetLogLevel(enum LogLevel eLvl)
	{
		m_eLevel = eLvl;
	}

protected:
	LogShmWrapper();
	
	int GetProcessName();
private:
	ShmDataQueue m_shm_data;
	enum LogLevel m_eLevel;
	char m_process_name[128];
	int m_init_flag;
};

#ifdef LOG_WITH_SHMPROC
// �����ڴ�ӳ��+��־���ȳ���ʽ��д��־

#define LogNone(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelNone,      fmt, ## __VA_ARGS__)
#define LogTrace(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelTrace,     fmt, ## __VA_ARGS__)
#define Log(fmt, ...)				LogShmWrapper::Instance().PutLog(LogShmWrapper::levelInfo,      fmt, ## __VA_ARGS__)
#define LogDebug(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelDebug,     fmt, ## __VA_ARGS__)
#define LogWarn(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelWarn,	   fmt, ## __VA_ARGS__)
#define LogImpt(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelImportant, fmt, ## __VA_ARGS__)
#define LogError(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelError,	   fmt, ## __VA_ARGS__)
#define LogFatal(fmt, ...)			LogShmWrapper::Instance().PutLog(LogShmWrapper::levelFatalError,fmt, ## __VA_ARGS__)

#define SETLOGLEVEL(lvl)			LogShmWrapper::Instance().SetLogLevel( static_cast<enum LogShmWrapper::LogLevel>(lvl) )
#define LOGINIT(xml_file,type,arg)	LogShmWrapper::Instance().Init( static_cast<const char *>(xml_file), static_cast<const char *>(type) )

#endif


#endif

