/*
 * =====================================================================================
 *
 *       Filename:  LogWrapper.h
 *
 *    Description:  log wrapper
 *
 *        Version:  1.0
 *        Created:  2014年12月15日 18时46分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_P2PWARPPER_MYLOG_H_
#define _MYLIB_P2PWARPPER_MYLOG_H_

#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>


class CLogWrapper
{
public:
	enum LogLevel
	{
		levelNone			= 0, // 0 default is not record
		levelTrace,				 // 1 trace, sometimes same as levelNone
		levelDebug,				 // 2 debug msg
		levelInfo,				 // 3 normal information
		levelImportant,			 // 4 important information
		levelWarn,				 // 5 warnning, but not error
		levelError,				 // 6 error message
		levelFatalError,		 // 7 Fatal error, system abort
	};
	~CLogWrapper();
	static CLogWrapper& Instance();

	void LogSave(enum LogLevel level, const char *fmt, ...);
	enum LogLevel GetLogLevel(void)      { return m_eLevel; }
	void SetLogLevel(enum LogLevel eLvl) { m_eLevel = eLvl; }
	void SetConsoleOutput(bool val) 	 { m_bConsoleOutput = val; }
	void SetLogWithTitle(bool val)  	 { m_bWithTitle = val; }

protected:
	CLogWrapper(enum LogLevel = levelDebug, bool bHeldOpen = false);
	bool Open(const char *pszFile);

private:
	bool IsSameDay(void);

private:
	enum LogLevel	m_eLevel;
	int32_t			m_file;
	time_t			m_opentime;
	bool			m_bConsoleOutput;
	bool			m_bWithTitle;
	bool			m_bHeldOpen;	//held open the file, not write and close
};



/* 
#define LogNone(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelNone,      fmt, ## __VA_ARGS__)
#define LogTrace(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelTrace,     fmt, ## __VA_ARGS__)
#define Log(fmt, ...)				CLogWrapper::Instance().LogSave(CLogWrapper::levelInfo,      fmt, ## __VA_ARGS__)
#define LogDebug(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelDebug,     fmt, ## __VA_ARGS__)
#define LogWarn(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelWarn,	   fmt, ## __VA_ARGS__)
#define LogImpt(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelImportant, fmt, ## __VA_ARGS__)
#define LogError(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelError,	   fmt, ## __VA_ARGS__)
#define LogFatal(fmt, ...)			CLogWrapper::Instance().LogSave(CLogWrapper::levelFatalError,fmt, ## __VA_ARGS__)

#define SETLOGLEVEL(lvl)			CLogWrapper::Instance().SetLogLevel( static_cast<enum CLogWrapper::LogLevel>(lvl) )
#define SETLOGCONSOLEOUTPUT(val)	CLogWrapper::Instance().SetConsoleOutput(val)
#define SETLOGWITHTITLE(val)		CLogWrapper::Instance().SetLogWithTitle(val)

#define IS_TRACE_LOG				(CLogWrapper::Instance().GetLogLevel()>=CLogWrapper::levelTrace )
#define IS_DEBUG_LOG				(CLogWrapper::Instance().GetLogLevel()>=CLogWrapper::levelDebug )
*/

#endif	// _MYLIB_P2PWARPPER_MYLOG_H_


