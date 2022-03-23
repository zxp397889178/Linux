#pragma once

#include "iLog.h"

namespace Comm
{

#define MAX_RICHLOG_LINE 8192 

void RichLogErr(const char *sFormat, ...);
void RichLogHead(const char *sFormat, ...);
void RichLogImpt(const char *sFormat, ...);
void RichLogFunc(const char *sFormat, ...);
void RichLogWarn(const char *sFormat, ...);

void FatalLogicErr(const char *sFormat, ...);
void FatalSysErr(const char *sFormat, ...);
void FatalSvrErr(const char *sFormat, ...);

void FatalInfo(const char *sFormat, ...);

}
