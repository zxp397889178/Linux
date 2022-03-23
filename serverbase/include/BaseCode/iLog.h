#ifndef _QW_BASELIB_SHMLOG_LOG_H_
#define _QW_BASELIB_SHMLOG_LOG_H_
#pragma once

#include "ShmLog/iLog.h"
#include "ShmLog/iLogShm.h"
#include "ShmLog/iLogShmErr.h"
#include "ShmLog/iShmLogAgent.h"
#include "ShmLog/iAttrLog.h"
#include "ShmLog/iRichLog.h"
#include "MyString.h"

#ifdef LOG_WITH_SHMFILE
// 采用内存映射文件方式的写日志

#define LogNone(fmt, ...)
// log level=1
#define LogInfo(fmt, ...)					Comm::LogInfo(fmt, ## __VA_ARGS__)			// info/ 
#define LogImpt(fmt, ...)					Comm::RichLogImpt(fmt, ## __VA_ARGS__)		// error/	紫底白字
#define LogInit(fmt, ...)					Comm::LogInit(fmt, ## __VA_ARGS__ )			// error/
#define LogFatal(fmt, ...)					Comm::FatalLogicErr(fmt, ## __VA_ARGS__)	// error/	红底白字
// log level=2
#define LogMon(fmt, ...)					Comm::LogMonitor(fmt, ## __VA_ARGS__)		// monitor/
#define LogMsg(fmt, ...)					Comm::LogMsg(fmt, ## __VA_ARGS__)			// error/
#define LogErr(fmt, ...)					Comm::RichLogErr(fmt, ## __VA_ARGS__)		// error/ 	黑底亮红字
#define LogWarn(fmt, ...)					Comm::RichLogWarn(fmt, ## __VA_ARGS__)		// error/ 	黑底亮黄字
// log level=3
#define LogDebug(fmt, ...)					Comm::LogDebug(fmt, ## __VA_ARGS__)			// error/
// log level=4
#define LogTrace(fmt, ...)					Comm::LogTrace(fmt, ## __VA_ARGS__)			// error/

#define SETLOGLEVEL(lvl)					Comm::SetLogLevel( (lvl) )
#define LOGINIT(mod,lvl,path)				Comm::OpenLog((mod), (lvl), (path))


// log level=1
#define LogImpt2(moudle,uin,fmt, ...)	LogImpt( "[IMPT] [%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)			// error/	紫底白字
#define LogFatal2(moudle,uin,fmt, ...)	LogFatal( "[FATAL][%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)		// error/	红底白字
// log level=2
#define LogMsg2(moudle,uin,fmt, ...)	LogMsg( "[MSG]  [%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)			// error/
#define LogErr2(moudle,uin,fmt, ...)	LogErr( "[ERROR][%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)			// error/ 	黑底亮红字
#define LogWarn2(moudle,uin,fmt, ...)	LogWarn( "[WARN] [%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)			// error/ 	黑底亮黄字
// log level=3
#define LogDebug2(moudle,uin,fmt, ...)	LogDebug( "[DEBUG][%s][%u] %s: " fmt, moudle, uin, __func__, ## __VA_ARGS__)		// error/

class CLogDaemon
{
public:
	CLogDaemon(const char *module, int level, const char *path)
	{
		int ret = Comm::OpenLog(module, level, path);
		if (ret != 0)
		{
			printf("\033[41;37mOpenLog failure, module [%s] path[%s] ret=%d\033[0m\n",
					module, path, ret);
		}
	}
	~CLogDaemon()
	{
		Comm::CloseLog();
	}
};

#endif // LOG_WITH_SHMFILE


#define _color_black            "\033[22;30m"
#define _color_red              "\033[22;31m"
#define _color_green            "\033[22;32m"
#define _color_brown            "\033[22;33m"
#define _color_blue             "\033[22;34m"
#define _color_magenta          "\033[22;35m"       // 洋红色
#define _color_cyan             "\033[22;36m"       // 青色
#define _color_gray             "\033[22;37m"
#define _color_darkgray         "\033[01;30m"
#define _color_lightred         "\033[01;31m"
#define _color_lightgreen       "\033[01;32m"
#define _color_yellow           "\033[01;33m"
#define _color_lightblue        "\033[01;34m"
#define _color_lightmagenta     "\033[01;35m"
#define _color_lightcyan        "\033[01;36m"
#define _color_white            "\033[01;37m"
#define _color_end              "\033[0m"

const char* color_text(const char* color, const char *str, CMyString &str1);
const char* color_int(const char* color, int32_t val, CMyString &str1);
const char* color_uint(const char* color, uint32_t val, CMyString &str1);
const char* color_int64(const char* color, int64_t val, CMyString &str1);
const char* color_uint64(const char* color, uint64_t val, CMyString &str1);
const char* color_double(const char* color, double val, CMyString &str1);

#endif // _QW_BASELIB_SHMLOG_LOG_H_

