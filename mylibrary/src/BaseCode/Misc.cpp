/*
 * =====================================================================================
 *
 *       Filename:  Misc.cpp
 *
 *    Description:  common function define here!
 *
 *        Version:  1.0
 *        Created:  2014年12月02日 17时26分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <dirent.h>

#include "Misc.h"
#include "StringFunc.h"
#include "MyString.h"
//#include "LogShmWrapper.h"

#define MAX_SIZE 	256

#ifndef PATH_MAX 
#	define PATH_MAX	256
#endif



const char* ConsoleColor(int8_t cBackgroundColor, 
						 int8_t cForegroundColor, 
						 int8_t cConsoleAttr)
{
	static CMyString strColor;
	strColor.Format("\033[%d;%d;%dm",cConsoleAttr, cBackgroundColor, cForegroundColor);

	return strColor.c_str();
}

pid_t gettid()
{
#ifdef OS_MAC
	pthread_t tid = pthread_self();
	void *v1 = (void*)tid;
	return *(pid_t*)v1;
#else
	return syscall(SYS_gettid);
//	return syscall(__NR_gettid);
#endif
}

const char* GetThreadUniqueName(char *szText, size_t cbText)
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	safe_sprintf(szText, cbText, "%ld-%ld-%llu-%lu",
				getpid(), gettid(), tv.tv_sec, tv.tv_usec);
	return szText;
}

// http://blog.chinaunix.net/uid-14214482-id-4299670.html
int32_t GetCurrentDirectoryEx(char *szDir, size_t cbDir)
{
	char current_absolute_path[MAX_SIZE] = "";

	//获取当前程序绝对路径 
	int32_t cnt = readlink("/proc/self/exe", current_absolute_path, MAX_SIZE);
	if (cnt <0 || cnt>= MAX_SIZE)
	{
		return -1;
	}
	//获取当前目录绝对路径，即去掉程序名
	for(int32_t i=cnt; i>=0; --i)
	{
		if (current_absolute_path[i] == '/')
		{
			current_absolute_path[i+1] = '\0';
			break;
		}
	}

	safe_strcpy(szDir, cbDir, current_absolute_path);
	return 0;
}

int32_t GetCurrentDirectory(char *szDir, size_t cbDir)
{
	char current_absolute_path[MAX_SIZE] = "";
	if (NULL == getcwd(current_absolute_path, MAX_SIZE))
	{
		return -1;
	}
	safe_strcpy(szDir, cbDir, current_absolute_path);
	return 0;
}

// http://www.tc5u.com/linux_unix/1531597.htm
const char* GetFullPathName(char *buf, size_t size, const char *path)
{
	char pathname[PATH_MAX+1] = "";
	char command[PATH_MAX+1] = "";
	FILE *fp;
	size_t len;

	if (!access(path, F_OK)) 
	{
		snprintf(command, sizeof(command), "cd `dirname %s` && pwd", path);
		fp = popen(command, "r");
		fgets(pathname, sizeof(pathname), fp);
		snprintf(buf, size, "%s", pathname);
		len = strlen(buf);
		if (len>=1 && (buf[len-1]=='\r' || buf[len-1]=='\n'))
			buf[len-1] = 0;
		if (len>=2 && (buf[len-2]=='\r' || buf[len-2]=='\n'))
			buf[len-2] = 0;
		pclose(fp);
	}
	else
	{
		buf[0] = 0;
	}

	return buf;
}

const char* GetFullPath(char *buf, size_t size, const char *file)
{
	char szDir[512] = "";
	safe_strcpy(szDir, sizeof(szDir), file);
	int32_t nLen = strlen(szDir);
	if (szDir[nLen-1] =='/')
	{
		safe_strcpy(buf, size, file);
		return file;
	}

	char szFile[256] = "";
	bool bFind(false);
	for(int32_t idx(nLen-1); idx>=0; --idx)
	{
		if (szDir[idx] == '/')
		{
			safe_strcpy(szFile, sizeof(szFile), &szDir[idx+1]);
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		GetCurrentDirectory(szDir, sizeof(szDir));
		safe_sprintf(buf, size, "%s/%s", szDir, file);
		return buf;
	}

	GetFullPathName(szDir, sizeof(szDir), file);
	safe_sprintf(buf, size, "%s/%s", szDir, szFile);
	return buf;
}

//http://blog.csdn.net/sunnylgz/article/details/7710892
int32_t CreateDir(const char *pszDir)
{
	char szDir[512] = "";
	safe_strcpy(szDir, sizeof(szDir), pszDir);

    DIR *dir = opendir(szDir);
    if (NULL != dir)
    {
        // already exist same directory.
        closedir(dir);
        return 0;
    }

	int32_t nLen = strlen(szDir);
	//在尾部加上'/'
	if (szDir[nLen-1] != '/')
	{
		szDir[nLen] = '/';
		szDir[nLen+1] = '\0';
		++nLen;
	}

	for(int32_t i=0; i<nLen; ++i)
	{
		if (szDir[i] != '/')
		{
			continue;
		}

		szDir[i] = '\0';
		if (strlen(szDir) ==0)
		{
			// BUG: 首部的 '/'不予创建
			szDir[i] = '/';
			continue;
		}

		int32_t ret = access(szDir, F_OK);
		if (ret != 0)
		{
			ret = mkdir(szDir, 0755);
			if (ret != 0)
			{
				printf(">>>>mkdir %s: errno(%d) %s", szDir, errno, strerror(errno));
				return -1;
			}
		}
		szDir[i] = '/';
	}

	return 0;
}

uint32_t PHP_Hash(char *arKey, uint32_t nKeyLength)
{
    if (NULL == arKey || '\0' == arKey[0])
    {
        return 0;
    }

    uint32_t h = 0, g;
    char *arEnd=arKey+nKeyLength;


    while (arKey < arEnd) {
        h = (h << 4) + *arKey++;
        if ((g = (h & 0xF0000000)) != 0) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h;
}

/* The following hash seems to work very well on normal text strings
* no collisions on /usr/dict/words and it distributes on %2^n quite
* well, not as good as MD5, but still good.
*/
uint32_t OpenSSL_Hash(const char *c)
{
    uint32_t ret=0;
    long n;
    uint32_t v;
    int r;

    if ((c == NULL) || (*c == '\0'))
        return(ret);
    /*
    byte_t b[16];
    MD5(c,strlen(c),b);
    return(b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24)); 
    */

    n=0x100;
    while (*c)
    {
        v=n|(*c);
        n+=0x100;
        r= (int)((v>>2)^v)&0x0f;
        ret=(ret<<r)|(ret>>(32-r));
        ret&=0xFFFFFFFFL;
        ret^=v*v;
        c++;
    }
    return((ret>>16)^ret);
}

uint32_t Simple_Hash(char *str)
{
    if (NULL == str || '\0' == str[0])
    {
        return 0;
    }

    register uint32_t h;
    register byte_t *p;


    for(h=0, p = (byte_t *)str; *p ; p++)
        h = 31 * h + *p;


    return h;
}

/*
* Fowler/Noll/Vo hash
*
* The basis of the hash algorithm was taken from an idea sent by email to the
* IEEE Posix P1003.2 mailing list from Phong Vo (kpv@research.att.com) and
* Glenn Fowler (gsf@research.att.com). Landon Curt Noll (chongo@toad.com)
* later improved on their algorithm.
*
* The magic is in the interesting relationship between the special prime
* 16777619 (2^24 + 403) and 2^32 and 2^8.
*
* This hash produces the fewest collisions of any function that we've seen so
* far, and works well on both numbers and strings.
*/
uint32_t MySQL_Hash(const byte_t *key, uint32_t len)
{
    if (NULL == key || '\0' == key[0])
    {
        return 0;
    }

    const byte_t *end=key+len;
    uint32_t hash = 0;
    for (; key < end; key++)
    {
        hash *= 16777619;
        hash ^= (uint32_t) *(byte_t*) key;
    }
    return (hash);
}

uint32_t MySQL_Hash_caseup(const byte_t *key, uint32_t len)
{
    if (NULL == key || '\0' == key[0])
    {
        return 0;
    }

    const byte_t *end=key+len;
    uint32_t hash = 0;
    for (; key < end; key++)
    {
        hash *= 16777619;
        hash ^= (uint32_t) (byte_t) toupper(*key);
    }
    return (hash);
}


static bool s_bSeedInit = false;
//获得随机数
uint32_t RandGet(bool bSeedReset/* = false*/)
{
    if (!s_bSeedInit || bSeedReset)
    {
        //种子已能保证是相当程度上的唯一了,不需要两次srand
        struct timeval tv = {0};
        gettimeofday(&tv, (struct timezone *)0);
        uint32_t seed =  ( ( ((uint32_t)(tv.tv_sec & 0xFFF))<<20) | tv.tv_usec);
        srand(seed);
        s_bSeedInit = true;
    }
    return rand();
}

uint32_t RandGet(uint32_t uMin, uint32_t uMax, bool bSeedReset/* = false*/)
{
    if (uMax == uMin) return 0;
    uint32_t minv = std::min(uMin, uMax);
    uint32_t maxv = std::max(uMin, uMax);
    return (RandGet(bSeedReset) % (maxv-minv)) + minv;
}

double RandGet(double dRange, bool bSeedReset/* = false*/)
{
    static double PI=3.1415926;

    uint32_t nRandom = RandGet(0, 1000, bSeedReset);
    double a = cos(nRandom*PI/1000);  // 0 ~ PI 之间的值 (-1.0 ~1.0)
    double b = dRange * a;
    return b;
}

void dumpbin(void *pData, size_t cbData, CMyString &str)
{
    byte_t *ptr = (byte_t*)pData;
    for(size_t i=0; i<cbData; ++i)
    {
        uint8_t c = ptr[i];
        str.Append("%02x ", c);
        if ((i+1)%32 == 0)
        {
            str.Append("\n");
        }
    }
    if (!str.empty())
    {
        str.Append("\n");
    }
}


