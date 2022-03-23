#include "iRichLog.h"
#include <string>

using std::string;

namespace Comm
{

static __thread char staRichLogBuff[MAX_RICHLOG_LINE + 8] = {0};

void FatalLogicErr(const char *sFormat, ...)
{
	string newFormat = "\033[41;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogErr("[FatalLogicErr]%s", staRichLogBuff);
	printf("%s\n", staRichLogBuff);
}

void FatalSysErr(const char *sFormat, ...)
{
	string newFormat = "\033[44;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogErr("[FatalSysError]%s", staRichLogBuff);
}

void FatalSvrErr(const char *sFormat, ...)
{
	string newFormat = "\033[45;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogErr("[FatalSvrError]%s", staRichLogBuff);
}

void FatalInfo(const char *sFormat, ...)
{
	string newFormat = "\033[41;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogInfo("[FatalInfo]%s", staRichLogBuff);
}

void RichLogHead(const char * sFormat, ...)
{
	string newFormat = "\033[44;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogDebug("%s",staRichLogBuff);
}

void RichLogErr(const char * sFormat, ...)
{
	// 黑底亮红字
	string newFormat = "\033[40;31;1m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogErr("%s", staRichLogBuff);
}

void RichLogWarn(const char * sFormat, ...)
{
	// 黑底亮黄字
	string newFormat = "\033[40;33;1m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogErr("%s", staRichLogBuff);
}

void RichLogImpt(const char * sFormat, ...)
{
	string newFormat = "\033[45;37m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogImpt("%s", staRichLogBuff);
}

void RichLogFunc(const char * sFormat, ...)
{
	string newFormat = "\033[44;30m " + string(sFormat) + " \033[0m";
	va_list args;
	va_start(args, sFormat);
	vsnprintf(staRichLogBuff, MAX_RICHLOG_LINE, newFormat.c_str(), args);
	va_end(args);

	LogDebug("%s",staRichLogBuff);
}

}
