/*
 * =====================================================================================
 *
 *       Filename:  MyString.cpp
 *
 *    Description:  string library base on std::string
 *
 *        Version:  1.0
 *        Created:  2014/11/09 15时30分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "MyString.h"
#ifdef WIN32
#include <cstarg>
#else
#include <stdarg.h>
#endif
#include <algorithm>
#include <functional>
#include <iterator>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "StringFunc.h"

// using namespace std;

//安全的格式化字符串函数
//param:	fmt			[IN] 	 Format定义
//			buffer		[IN]  	 初始的buffer空间，一般为1024大小的字符数组
//			pc			[IN/OUT] char*指针，用于指向buffer之用
//			assign		[IN/OUT] 是否有分配内存，若buffer空间不足时会触发分配内存操作
//			len			[IN/OUT] pc的实际长度，有可能超过buffer空间大小
//note:		调用这个宏之后，必须使用pc这个参数，并判断assign是否有分配内存
#define SAFE_FORMAT(fmt,buffer,pc,assign,len)	{ 	int32_t _bufLen = (int32_t)sizeof(buffer); \
											 		int32_t ret(0); pc=&buffer[0]; assign=false; va_list args;\
												 	try{ do { \
														va_start(args, fmt); ret = vsnprintf(pc,_bufLen, fmt,args); va_end(args); \
														if (ret>-1&&ret<_bufLen) { len=ret; break;} \
														if (ret>-1) {_bufLen = ret+10;} else{_bufLen*=2;} \
														if (assign){ free(pc);assign=false;} \
														pc=(char*)calloc(sizeof(char),_bufLen);\
														if (NULL==pc) {break;} assign=true; \
													}while(true);} catch(...) { pc=NULL; } \
												}



int32_t my_isspace(char c)
{
	return ::isspace((int)c);
}

int32_t my_isendl(char c)
{
    if (c==0x0D || c==0x0A)
    {
        return true;
    }
    return false;
}

int32_t toreverse(int32_t c)
{
	if (!isalpha(c))
	{
		return c;
	}
	if (isupper(c))
		return tolower(c);
	return toupper(c);
}

CMyString::CMyString(void)
    : BASE_STRING()
{
}


CMyString::CMyString(const char *fmt, ...)
{
    if (NULL != fmt)
    {
        if (strchr(fmt, '%') != NULL)
        {
			char buffer[MAX_BUFFER_SIZE] = "";
			char *pc = &buffer[0];
			bool bAssign(false);
			int32_t len(0);
            //va_list args;
            //va_start(args, fmt);
            //vsnprintf(pc, MAX_BUFFER_SIZE-1, fmt, args);
            //va_end(args);
			SAFE_FORMAT(fmt,buffer,pc,bAssign,len);
            //printf("===[%s]===\n", pc);
			if (pc!=NULL) {
				this->assign(pc, len);
				if (bAssign) {
					free(pc); pc=NULL;
				}
				pc = NULL;
			}
        }
        else
        {
            // not arguments need
            this->assign(fmt);
        }
    }
}

CMyString::CMyString(const std::string &rhs)
    : BASE_STRING()
{
    this->assign(rhs.c_str());
}

CMyString::CMyString(const CMyString &rhs)
    : BASE_STRING()
{
    if (this != &rhs)
    {
        this->assign(rhs.c_str());
    }
}

CMyString::~CMyString()
{
}

CMyString& CMyString::operator=(const CMyString &rhs)
{
    if (this != &rhs)
    {
        this->assign(rhs.c_str());
    }
    return *this;
}

CMyString& CMyString::operator=(const std::string &rhs)
{
    this->assign(rhs.c_str());
    return *this;
}

CMyString& CMyString::operator=(const char *rhs)
{
    if (rhs != NULL)
    {
        this->assign(rhs);
    }
    return *this;
}

bool CMyString::operator==(const CMyString &rhs)
{
    return ( 0==this->compare(rhs.c_str()) );
}

bool CMyString::operator==(const std::string &rhs)
{
    return ( 0==this->compare(rhs.c_str()) );
}

bool CMyString::operator==(const char *rhs)
{
    if (NULL == rhs)
    {
        return false;
    }
    return ( 0==this->compare(rhs) );
}

bool CMyString::operator!=(const CMyString &rhs)
{
    return !(this->operator==(rhs));
}

bool CMyString::operator!=(const std::string &rhs)
{
    return !(this->operator==(rhs));
}

bool CMyString::operator!=(const char *rhs)
{
    if (NULL==rhs)
    {
        return false;
    }
    return !(this->operator==(rhs));
}

bool CMyString::Format(const char *fmt, ...)
{
    if (NULL == fmt)
    {
        return false;
    }

	char buffer[MAX_BUFFER_SIZE] = "";
	char *pc = &buffer[0];
	int32_t len(0);
	bool assign(false);
	SAFE_FORMAT(fmt, buffer,pc,assign,len);
	if (pc!=NULL) {
		this->assign(pc, len);
		if (assign){
			free(pc); pc = NULL;
		}
	}


/*     
    char buffer[MAX_BUFFER_SIZE] = "";
    va_list args;
    int32_t nLen(0);
    try
    {
        va_start(args, fmt);
        nLen = vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
    }
    catch(...)
    {
        return false;
    }
    if (nLen <= 0)
    {
        return false;
    }
    this->assign(buffer, nLen);
*/
    return true;
}

void CMyString::Format(int32_t val, bool bFormatted)
{
	char szText[128] = "";
	if (bFormatted)
	{
		format_int32(val, szText, sizeof(szText));
	}
	else
	{
		_itoa(val, szText, sizeof(szText));
	}
	this->assign(szText);
}

void CMyString::Format(int64_t val, bool bFormatted)
{
	char szText[128] = "";
	if (bFormatted)
	{
		format_int64(val, szText, sizeof(szText));
	}
	else
	{
		_i64toa(val, szText, sizeof(szText));
	}
	this->assign(szText);
}

void CMyString::Format(uint32_t val, bool bFormatted)
{
	char szText[128] = "";
	if (bFormatted)
	{
		format_uint32(val, szText, sizeof(szText));
	}
	else
	{
		_ultoa(val, szText, sizeof(szText));
	}
	this->assign(szText);
}

void CMyString::Format(uint64_t val, bool bFormatted)
{
	char szText[128] = "";
	if (bFormatted)
	{
		format_uint64(val, szText, sizeof(szText));
	}
	else
	{
		_ui64toa(val, szText, sizeof(szText));
	}
	this->assign(szText);
}

bool CMyString::Append(const char *fmt, ...)
{
    if (NULL == fmt)
    {
        return false;
    }

	char buffer[MAX_BUFFER_SIZE]= "";
	char *pc = &buffer[0];
	int32_t len = sizeof(buffer);
	bool bAssign(false);
	SAFE_FORMAT(fmt, buffer, pc, bAssign, len);
	if (pc != NULL) {
		this->append(pc, len);
		if (bAssign) {
			free(pc); pc=NULL;
		}
	}
    
/* 	
    char buffer[MAX_BUFFER_SIZE] = "";
    va_list args;
    int32_t nLen(0);
    try
    {
        va_start(args, fmt);
        nLen = vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
    }
    catch(...)
    {
        return false;
    }
    if (nLen <= 0)
    {
        return false;
    }

	this->append(buffer, nLen);
*/	
	return true;
}

void CMyString::MakeUpper(void)
{
	if (this->empty())
		return;

	transform(begin(), end(),
			begin(),
			toupper);	
}

void CMyString::MakeLower(void)
{
	if (this->empty())
		return;

	transform(begin(), end(),
			begin(),
			tolower);
}

void CMyString::MakeRev(void)
{
	if (this->empty())
		return;
	transform(begin(), end(),
			begin(),
			toreverse);
}

void CMyString::rtrim(void)
{
    reverse_iterator p = find_if(rbegin(), rend(), 
			std::not1(std::ptr_fun(my_isspace)));
    erase(p.base(), end());
}

void CMyString::ltrim(void)
{
    iterator p = find_if(begin(), end(), 
			std::not1(std::ptr_fun(my_isspace)));
    erase(begin(), p);
}

void CMyString::trim(void)
{
	this->ltrim();
	this->rtrim();
}

void CMyString::trim_endl(void)
{
    reverse_iterator p = rbegin();
    do {
        uint8_t c = *p;
        if (c!=0x0D && c!=0x0A)
        {
            break;
        }
        ++p;
    }while(p!=rend());
    erase(p.base(), end());
}

CMyString CMyString::left(size_t cbSize)
{
	return CMyString(substr(0, cbSize));
}

CMyString CMyString::right(size_t cbSize)
{
	int32_t nLen = size();
	int32_t nFirst = nLen - (int32_t)cbSize;
	nFirst = (nFirst<0) ? 0 : nFirst;
	return CMyString(substr(nFirst));
}

CMyString CMyString::substring(int32_t nPos, int32_t nSize)
{
	return CMyString(substr(nPos, nSize));
}

CMyString::size_type CMyString::Find(const char *pszToFind, bool bCaseSensive)
{
	if (NULL==pszToFind || '\0'==pszToFind[0])
	{
		return npos;
	}

	if (bCaseSensive)
	{
		return find(pszToFind);
	}

	const char *pc = strcasestr(c_str(), pszToFind );
	if (pc == NULL)
		return npos;
	return ( pc - c_str() );
}

int32_t CMyString::Replace(const CMyString &strOld, const CMyString &strNew, bool bCaseSensive)
{
    if (strOld.empty())
    {
        return 0;
    }
    int32_t oldLen = (int32_t)strOld.length();
    int32_t newLen = (int32_t)strNew.length();

    size_t pos1(0);
    size_t pos2(0);
    int32_t cnt(0);
    if (bCaseSensive)
    {
        // 大小写敏感的查找
        pos2 = this->find(strOld, pos1);
        do {
            if (pos2 == std::string::npos)
            {
                break;
            }
            this->replace(pos2, oldLen, strNew);
            pos1 = pos2 + newLen;
            pos2 = this->find(strOld, pos1);
            ++cnt;
        }while(true);
    }
    else
    {
        // 大小写不敏感时，则用临时变量，转换成小写来查找
        CMyString newVal(this->c_str());
        newVal.MakeLower();
        CMyString tobeFind(strOld);
        tobeFind.MakeLower();
        pos2 = newVal.find(tobeFind, pos1);
        do {
            if (pos2 == std::string::npos)
            {
                break;
            }
            newVal.replace(pos2, oldLen, strNew);
            this->replace(pos2, oldLen, strNew);
            pos1 = pos2 + newLen;
            pos2 = newVal.find(tobeFind, pos1);
            ++cnt;
        }while(true);
    }
    return cnt;
}

int32_t CMyString::Replace(char cOld, char cNew)
{
    int32_t cnt(0);
    for(iterator it=begin(); it !=end(); ++it)
    {
        if (*it==cOld)
        {
            *it=cNew;
            ++cnt;
        }
    }
    return cnt;
}

int32_t CMyString::asInt(void)
{
    if (empty()) return 0;
    return StrToInt32(c_str());
}

uint32_t CMyString::asUint(void)
{
    if (empty()) return 0;
    return StrToUint32(c_str());
}

int64_t CMyString::asInt64(void)
{
    if (empty()) return 0;
    return StrToInt64(c_str());
}

uint64_t CMyString::asUint64(void)
{
    if (empty()) return 0;
    return StrToUint64(c_str());
}

double CMyString::asDouble(void)
{
    if (empty()) return 0;
    return StrToDouble(c_str());
}

void CMyString::Erase(const CMyString &tobe_erase, int32_t maxLoopCount)
{
    do
    {
        size_type pos = find(tobe_erase.c_str());
        if (pos == std::string::npos)
        {
            break;
        }
        erase(pos, tobe_erase.size());
    }while(--maxLoopCount>0);
}
/////////////////////////////////////////////////////////////////////
//
void rtrim(std::string &str)
{
    std::string::reverse_iterator p = std::find_if(str.rbegin(), str.rend(), 
			std::not1(std::ptr_fun(my_isspace)));
    str.erase(p.base(), str.end());
}
void ltrim(std::string &str)
{
    std::string::iterator p = std::find_if(str.begin(), str.end(), 
			std::not1(std::ptr_fun(my_isspace)));
    str.erase(str.begin(), p);
}

void trim(std::string &str)
{
    ltrim(str);
    rtrim(str);
}

void trim_endl(std::string &str)
{
    std::string::reverse_iterator p = std::find_if(str.rbegin(), str.rend(), 
			std::not1(std::ptr_fun(my_isendl)));
    str.erase(p.base(), str.end());
}
/*
void trim_endl(std::string &str)
{
    std::string::reverse_iterator p = str.rbegin();
    do {
        uint8_t c = *p;
        if (c!=0x0D && c!=0x0A)
        {
            break;
        }
        ++p;
    }while(p!=str.rend());
    str.erase(p.base(), str.end());
}
*/
