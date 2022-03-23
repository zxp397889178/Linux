/*
 * =====================================================================================
 *
 *       Filename:  Misc.h
 *
 *    Description:  Misc functions
 *
 *        Version:  1.0
 *        Created:  04/10/2015 02:43:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_BASECODE_MISC_H_
#define _GFX_BASECODE_MISC_H_

#include <stdint.h>
#include <sys/types.h>
#include "mydef.h"
#include "MyString.h"

class NonCopyable
{
protected:
	NonCopyable() {}
	virtual ~NonCopyable() {}

private:
	NonCopyable(const NonCopyable &rhs);
	NonCopyable& operator=(const NonCopyable &rhs);
};


///////////////////////////////////////////////////////////////////////////////////
//external function


const char* ConsoleColor(int8_t cBackgroundColor, int8_t cForegroundColor, 
	   						    int8_t cConsoleAttr = CON_ATTR_DEFAULT );
#define CONSOLE_COLOR(attr, bgColor, fgColor)	ConsoleColor(bgColor, fgColor, attr)
#define CONSOLE_RESET							"\033[0m"

#define NON_COPYABLE(X)		private:\
								X(const X&rhs);\
								X& operator=(const X &rhs);

pid_t gettid(void);
const char* GetThreadUniqueName(char *szText, size_t cbText);

int32_t GetCurrentDirectoryEx(char *szDir, size_t cbDir);
int32_t GetCurrentDirectory(char *szDir, size_t cbDir);

const char* GetFullPathName(char *buf, size_t size, const char *path);

const char* GetFullPath(char *buf, size_t size, const char *file);

/* 创建目录，支持连续创建目录
 * return	0  成功
 * 			-1 创建目录失败
 * */
int32_t CreateDir(const char *pszDir);

uint32_t PHP_Hash(char *arKey, uint32_t nKeyLength);
uint32_t OpenSSL_Hash(const char *c);
uint32_t Simple_Hash(char *str);
uint32_t MySQL_Hash(const byte_t *key, uint32_t len);
uint32_t MySQL_Hash_caseup(const byte_t *key, uint32_t len);

//获得随机数
uint32_t RandGet(bool bSeedReset = false);
//获得指定范围的随机数
uint32_t RandGet(uint32_t uMin, uint32_t uMax, bool bSeedReset = false);
//获得双精度的随机数,返回的值范围在 -dRange ~ +dRange 之间
double RandGet(double dRange, bool bSeedReset = false);


void dumpbin(void *pData, size_t cbData, CMyString &to);

#endif // _GFX_BASECODE_MISC_H_


