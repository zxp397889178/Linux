/*
 * =====================================================================================
 *
 *       Filename:  sysinfo.cpp
 *
 *    Description:  get linux system info
 *
 *        Version:  1.0
 *        Created:  09/11/2019 10:55:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 * URL： http://code.google.com/p/mooon/source/browse/trunk/common_library/src/sys/info.cpp 
 * =====================================================================================
 */

#include "BaseCode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CMyString getOsInfo(void)
{
    FILE *fp = fopen("/prov/version", "r");
    if (NULL == fp)
    {
        return "";
    }

    CMyString result;
    char buf[1024] = "";
    while(!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, fp);
        result.Append("%s\n", buf);
    }
    fclose(fp);
    return result;
}

CMyString getCpuInfo(void)
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (NULL == fp)
    {
        return "";
    }

    CMyString result;
    char buf[1024] = "";
    while(!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, fp);
        result.Append("%s\n", buf);
    }
    fclose(fp);
    return result;
}

CMyString getMemoryInfo(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (NULL == fp)
    {
        return "";
    }

    CMyString result;
    char buf[1024] = "";
    while(!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, sizeof(buf)-1, fp);
        result.Append("%s\n", buf);
    }
    fclose(fp);
    return result;
}
