/*
 * =====================================================================================
 *
 *       Filename:  MyString.h
 *
 *    Description:  string class base on std::string
 *
 *        Version:  1.0
 *        Created:  2014/11/09 15时11分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYSTRING_H_
#define _MYLIB_BASECODE_MYSTRING_H_

#include <string>
#include "mydef.h"

#define BASE_STRING std::basic_string< char, std::char_traits<char>, std::allocator<char> >

class CMyString : public BASE_STRING
{
public:
    CMyString(void);
    CMyString(const std::string &rhs);
    CMyString(const CMyString &rhs);
    CMyString(const char *fmt, ...);
    ~CMyString();

    CMyString& operator=(const CMyString &rhs);
    CMyString& operator=(const std::string &rhs);
    CMyString& operator=(const char *rhs);
    bool operator==(const CMyString &rhs);
    bool operator==(const std::string &rhs);
    bool operator==(const char *rhs);
    bool operator!=(const CMyString &rhs);
    bool operator!=(const std::string &rhs);
    bool operator!=(const char *rhs);

public:
    bool Format(const char *fmt, ...);
    void Format(int32_t  val, bool bFormatted=false);
    void Format(int64_t  val, bool bFormatted=false);
    void Format(uint32_t val, bool bFormatted=false);
    void Format(uint64_t val, bool bFormatted=false);
    bool Append(const char *fmt, ...);

    void ltrim(void);
    void rtrim(void);
	void trim(void);
    void trim_endl(void);

    CMyString left(size_t cbSize);
    CMyString right(size_t cbSize);
    CMyString substring(int32_t nPos, int32_t nSize);

	// find [pszToFind] in string
	// return npos	not find
	// 		  >=0	the pos first apper
	CMyString::size_type Find(const char *pszToFind, bool bCaseSensive = true);

    // 返回值：替换的次数
    int32_t Replace(char cOld, char cNew);
    int32_t Replace(const CMyString &strOld, const CMyString &strNew, bool bCaseSensive = true);

    // 删除指定的数据，循环10次后强行退出
    void Erase(const CMyString &tobe_erase, int32_t maxLoopCount=10);

	// _strrev
	void MakeRev(void);
	// _strupr
    void MakeUpper(void);
	// _strlwr
    void MakeLower(void);

public:
    int32_t asInt(void);
    uint32_t asUint(void);
    int64_t asInt64(void);
    uint64_t asUint64(void);
    double asDouble(void);
};

void ltrim(std::string &str);
void rtrim(std::string &str);
void trim(std::string &str);
void trim_endl(std::string &str);

#endif // _MYLIB_BASECODE_MYSTRING_H_




