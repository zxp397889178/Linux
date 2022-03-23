/*
 * =====================================================================================
 *
 *       Filename:  StringFunc.cpp
 *
 *    Description:  string functions
 *
 *        Version:  1.0
 *        Created:  2014/11/09 16时03分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifdef WIN32
#include <cstdarg>
#else
#include <stdarg.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "StringFunc.h"
#include "ScopedBuffer.hpp"

int32_t StrToLong(const char *str, uint8_t base)
{
	if (NULL == str)
	{
		THROW_STD(invalid_argument,"long string");
		return 0;
	}
	char *endstring;
	return (int32_t)strtol(str, &endstring, base);
}
int32_t StrToLong(const char *str)
{
	return StrToLong(str, 0);
}

double StrToDouble(const char *str)
{
	if (NULL == str)
	{
		THROW_STD(invalid_argument,"double string");
		return 0.0;
	}
	char *endstring;
	return strtod(str, &endstring);
}

uint32_t StrToUint32(const char *str, uint8_t base)
{
	if (NULL == str)
	{
		THROW_STD(invalid_argument,"unsigned long string");
		return 0;
	}
	char *endstring;
	return (uint32_t)strtoul(str, &endstring, base);
}
uint32_t StrToUint32(const char *str)
{
	return StrToUint32(str, 0);
}

int64_t StrToInt64(const char *str, uint8_t base)
{
	if (NULL == str)
	{
		THROW_STD(invalid_argument,"long long string");
		return 0;
	}
	char *endstring;
	return (int64_t)strtoll(str, &endstring, base);
}
int64_t StrToInt64(const char *str)
{
	return StrToInt64(str, 0);
}

uint64_t StrToUint64(const char *str, uint8_t base)
{
	if (NULL == str)
	{  
		THROW_STD(invalid_argument,"unsigned long long string");
		return 0;
	}
	char *endstring;
	return (uint64_t)strtoull(str, &endstring, base);
}
uint64_t StrToUint64(const char *str)
{
	return StrToUint64(str, 0);
}

bool StrToBoolean(const char *str)
{
	if (NULL==str || '\0'==str[0])
	{
		THROW_STD(invalid_argument,"boolean string");
		return false;
	}
	if (!strncasecmp(str, "true", 4) ||
		!strncasecmp(str, "yes", 3) ||
		!strncasecmp(str, "on", 2) ||
		!strncasecmp(str, "1", 1))
	{
		return true;
	}
	else if (!strncasecmp(str, "false", 5) ||
			 !strncasecmp(str, "no", 2) ||
			 !strncasecmp(str, "off", 3) ||
			 !strncasecmp(str, "0", 1))
	{
		return false;
	}
	else
	{
		THROW_STD(invalid_argument,"boolean string");
		return false;
	}
}

#if !((defined WIN32) || (defined WIN64))
const char* _itoa(int32_t val, char *szText, size_t cbText)
{
	snprintf(szText, cbText, "%d", val);
	return szText;
}

const char* _i64toa(int64_t val, char *szText, size_t cbText)
{
	snprintf(szText, cbText, "%ld", val);
	return szText;
}

const char* _ultoa(uint32_t val, char *szText, size_t cbText)
{
	snprintf(szText, cbText, "%u", val);
	return szText;
}

const char* _ui64toa(uint64_t val, char *szText, size_t cbText)
{
	snprintf(szText, cbText, "%lu", val);
	return szText;
}

int32_t stricmp(const char *s1, const char *s2)
{
	return strcasecmp(s1, s2);
}

int32_t strnicmp(const char *s1, const char *s2, size_t n)
{
	return strncasecmp(s1, s2, n);
}

const char* _strupr(char *s)
{
	if (NULL==s || '\0'==s[0])
		return s;
	size_t cbLen = strlen(s);
	for(size_t i=0; i<cbLen; ++i)
	{
		byte_t b = s[i];
		s[i] = toupper(b);
	}
	return s;
}

const char* _strlwr(char *s)
{
	if (NULL==s || '\0'==s[0])
		return s;
	size_t cbLen = strlen(s);
	for(size_t i=0; i<cbLen; ++i)
	{
		byte_t b = s[i];
		s[i] = tolower(b);
	}
	return s;
}

#endif // WIN32 || WIN64

int32_t safe_sprintf(char *buffer, size_t cbLen, const char *fmt, ...)
{
	if (NULL == buffer || cbLen<=1 || NULL == fmt)
	{
		THROW_STD(invalid_argument,"sprintf argument");
		return -1;
	}

	int32_t nLen(-1);
	va_list args;
	try
	{
		va_start(args, fmt);
		nLen = vsnprintf(buffer, cbLen, fmt, args);
		va_end(args);
	}
	catch(...)
	{
		THROW_STD(invalid_argument,"format string");
		return -1;
	}

	if (nLen<0 || nLen >= (int32_t)cbLen)
	{
		THROW_STD(overflow_error,"output buffer is too small");
		return 0;
	}
	return nLen;
}

int32_t safe_strcpy(char *buffer, size_t cbLen, const char *source)
{
	if (NULL==buffer || cbLen<=1 || NULL==source)
	{
		THROW_STD(invalid_argument,"strcpy argument");
		return -1;
	}

	size_t cbSource = strlen(source);
	if (cbLen <= cbSource)
	{
		strncpy(buffer, source, cbLen-1);
		buffer[cbLen - 1] = '\0';
//		THROW_STD(overflow_error, "output buffer is too small");
		return 0;
	}

	strcpy(buffer, source);
	return (int)strlen(buffer);
}

int32_t safe_strncpy(char *buffer, size_t cbLen, const char *source, size_t cbCount)
{
	if (NULL==buffer || cbLen<=1 || NULL==source || cbLen==0)
	{
		THROW_STD(invalid_argument,"strncpy argument");
		return -1;
	}
	memset(buffer, 0, cbLen);
	size_t cbSource = strlen(source);
	size_t cbNeed = std::min(cbSource, cbCount);
	if (cbLen <= cbNeed)
	{
		strncpy(buffer, source, cbLen-1);
		THROW_STD(overflow_error,"output buffer is too small");
		return 0;
	}
	strncpy(buffer, source, cbNeed);
	return (int)strlen(buffer);
}

int32_t safe_strcat(char *buffer, size_t cbLen, const char *source)
{
	if (NULL==buffer || cbLen<=1 || NULL==source)
	{
		THROW_STD(invalid_argument,"strcat argument");
		return -1;
	}
	size_t cbData = strlen(buffer);
	if (cbLen <= cbData+1)
	{
		THROW_STD(overflow_error,"output buffer is too small");
		return 0;
	}
	int32_t nLeft = (int)(cbLen - cbData);
	char *pc = &buffer[cbData];
	int32_t ret = safe_strcpy(pc, (size_t)nLeft, source);
	return ret;
}

int32_t safe_strcat_ex(char *buffer, size_t cbLen, const char *fmt, ...)
{
	if (NULL==buffer || cbLen<=1 || NULL==fmt)
	{
		THROW_STD(invalid_argument,"strcat_ex argument");
		return -1;
	}

	int32_t cbUsed = (int32_t)strlen(buffer);
	int32_t cbLeft = (int32_t)cbLen - cbUsed;
	if (cbLen <= 0)
	{
		THROW_STD(overflow_error, "output buffer not enough");
		return 0;
	}

	char buf[cbLeft+1];
	va_list args;
	int32_t nLen(-1);
	try
	{
		va_start(args, fmt);
		nLen = vsnprintf(buf, cbLeft, fmt, args);
		va_end(args);
	}
	catch(...)
	{
		THROW_STD(invalid_argument,"format string");
		return -1;
	}

	if (nLen<0 || nLen>=cbLeft)
	{
		THROW_STD(overflow_error, "output buffer not enouth");
		return 0;
	}

	char *pc = &buffer[cbUsed];
	memcpy(pc, buf, nLen);

	return nLen;
}

int32_t safe_strtok(const char *buffer, std::vector<std::string> &array
		, const char *delimit)
{
	if (NULL==buffer || '\0'==buffer[0] || NULL==delimit || '\0'==delimit[0])
	{
		THROW_STD(invalid_argument,"strtok arguments");
		return -1;
	}

	size_t cbBuffer = strlen(buffer);
	CTempBuffer tmpBuf(cbBuffer+10);
	char *buf = tmpBuf.data();
	safe_strcpy(buf, cbBuffer+10, buffer);
	char *context_string;
	char *pc = strtok_r(buf, delimit, &context_string);
	while(pc != NULL)
	{
#if 0
		if ('\0' != pc[0])
		{
			array.push_back(std::string(pc));
		}
#else
		 array.push_back(std::string(pc));
#endif
		pc = strtok_r(NULL, delimit, &context_string);
	}

	return (int)array.size();
}

int32_t safe_strtok(const char *buffer, std::vector<uint32_t> &array, 
		const char *delimit) 
{
	if (NULL==buffer || '\0'==buffer[0] || NULL==delimit || '\0'==delimit[0])
	{
		THROW_STD(invalid_argument,"strtok arguments");
		return -1;
	}

	size_t cbBuffer = strlen(buffer);
	CTempBuffer tmpBuf(cbBuffer+10);
	char *buf = tmpBuf.data();
	safe_strcpy(buf, cbBuffer+10, buffer);
	char *context_string;
	char *pc = strtok_r(buf, delimit, &context_string);
	while(pc != NULL)
	{
		if ('\0' != pc[0])
		{
			//printf("val = [%s]\n", pc);
			uint32_t val = StrToUint32(pc);
			array.push_back(val);
		}
		pc = strtok_r(NULL, delimit, &context_string);
	}
	return (int)array.size();
}

int32_t safe_strtok(const char *buffer, std::vector<KvItem> &array, 
		const char *delimit, const char *delimitNext)
{
	if (NULL==buffer || '\0'==buffer[0] 
			|| NULL==delimit || '\0'==delimit[0]
			|| NULL==delimitNext || '\0'==delimitNext[0] )
	{
		THROW_STD(invalid_argument,"strtok arguments");
		return -1;
	}

	size_t cbBuffer = strlen(buffer);
	CTempBuffer tmpBuf(cbBuffer+10);
	char *buf = tmpBuf.data();
	safe_strcpy(buf, cbBuffer+10, buffer);
	char *context_string;
	char *pc = strtok_r(buf, delimit, &context_string);
	while(pc != NULL)
	{
		if ('\0' != pc[0])
		{
			//printf("kvstr=[%s]\n", pc);
			std::vector<uint32_t> kv;
			int32_t cnt = safe_strtok(pc, kv, delimitNext);
			if (cnt == 0)
			{
				THROW_STD(invalid_argument, pc);
				// 格式不匹配
				return -2;
			}
			KvItem item;
			item.key = kv[0];
			if (cnt == 2) {
				item.value = kv[1];
			}
			array.push_back(item);
		}
		pc = strtok_r(NULL, delimit, &context_string);
	}
	return (int)array.size();
}

int32_t safe_strtok(const char *buffer, std::vector<KvstrItem> &array, 
		const char *delimit, const char *delimitNext)
{
	if (NULL==buffer || '\0'==buffer[0] 
			|| NULL==delimit || '\0'==delimit[0]
			|| NULL==delimitNext || '\0'==delimitNext[0] )
	{
		THROW_STD(invalid_argument,"strtok arguments");
		return -1;
	}

	size_t cbBuffer = strlen(buffer);
	CTempBuffer tmpBuf(cbBuffer+10);
	char *buf = tmpBuf.data();
	safe_strcpy(buf, cbBuffer+10, buffer);
	char *context_string;
	char *pc = strtok_r(buf, delimit, &context_string);
	while(pc != NULL)
	{
		if ('\0' != pc[0])
		{
			std::vector<std::string> kv;
			int32_t cnt = safe_strtok(pc, kv, delimitNext);
			if (cnt == 0)
			{
				THROW_STD(invalid_argument, pc);
				// 格式不匹配
				return -2;
			}
			KvstrItem item;
			item.key = kv[0];
			if (cnt == 2) {
				item.value = kv[1];
			}
			array.push_back(item);
		}
		pc = strtok_r(NULL, delimit, &context_string);
	}
	return (int)array.size();
}


int32_t safe_strsplit(const char *buffer, std::vector<std::string> &array, const char *delimit)
{
	if (NULL == buffer || '\0' == buffer[0]
		|| NULL == delimit || '\0' == delimit[0])
	{
		THROW_STD(invalid_argument, "strtok arguments");
		return -1;
	}

	size_t cbBuffer = strlen(buffer);
	CTempBuffer tmpBuf(cbBuffer + 10);
	char *buf = tmpBuf.data();
	safe_strcpy(buf, cbBuffer + 10, buffer);
	int nPos = 0;
	int nBegin = 0;
	int splitLen = strlen(delimit) + 2; // "\r\n"
	char *pc = strstr(buf, delimit);
    if (pc == NULL)
    {
        array.push_back(buf);
    }
    else
    {
	    while (pc != NULL && nBegin < (int)cbBuffer)
	    {
		    nPos = pc - buf;
		    if ('\0' != pc[0] && nPos>0)
		    {
			    std::string tmp;
			    tmp.append(buf + nBegin, nPos - nBegin);
			    array.push_back(tmp);
		    }

		    nBegin = nPos + splitLen;
		    pc = strstr(buf + nBegin, delimit);		
	    }
    }
	return (int)array.size();
}

const char* format_uint32(uint32_t val, char *szText, size_t cbText)
{
	return format_uint64(val, szText, cbText);
}

const char* format_uint64(uint64_t val, char *szText, size_t cbText)
{
    //4294967295			(0xFFFFFFFF)
    //4,294,967,295
	//
	//18446744073709551615 	(0xFFFFFFFFFFFFFF)
	//18,446,744,073,709,551,615
	//
    safe_sprintf(szText, cbText, "%llu", val);

    char szTemp[8][5] = {"",};
	const int32_t nMaxSect = sizeof(szTemp)/sizeof(szTemp[0]);
    int32_t nLen = strlen(szText);
    int32_t nIndex=nMaxSect-1;
    while(nLen>0 && nIndex>=0)
    {   
        if (nLen > 3)
        {   
            safe_strncpy(szTemp[nIndex], sizeof(szTemp[nIndex]), &szText[nLen-3], 3);
        }   
        else
        {   
            safe_strncpy(szTemp[nIndex], sizeof(szTemp[nIndex]), &szText[0], nLen);
        }   
        --nIndex;
        nLen -= 3;
    }   

    memset(szText, 0, cbText);
    for(int32_t i=0; i<nMaxSect; ++i)
    {   
        if ('\0' == szTemp[i][0]) continue;
        for(int32_t j=i; j<nMaxSect; ++j)
        {   
            safe_strcat(szText, cbText, szTemp[j]);
            if (j != nMaxSect-1)
            {   
                safe_strcat(szText, cbText, ",");
            }   
        }   
        break;
    }   

    return (const char*)szText;
}

const char* format_int32(int32_t val, char *szText, size_t cbText)
{
	return format_int64(val, szText, cbText);
}

const char* format_int64(int64_t val, char *szText, size_t cbText)
{
    bool bNegative = false;
    if (val < 0)
    {
        bNegative = true;
        val = abs(val);
    }

    safe_sprintf(szText, cbText, "%llu", val);
    char szTemp[8][4] = {"",};
	const int32_t nMaxSect = sizeof(szTemp)/sizeof(szTemp[0]);
    int32_t nLen = strlen(szText);
    int32_t nIndex=nMaxSect-1;
    while(nLen>0 && nIndex>=0)
    {
        if (nLen > 3)
        {
            safe_strncpy(szTemp[nIndex], sizeof(szTemp[nIndex]), &szText[nLen-3], 3);
        }
        else
        {
            safe_strncpy(szTemp[nIndex], sizeof(szTemp[nIndex]), &szText[0], nLen);
        }
        --nIndex;
        nLen -= 3;
    }

    memset(szText, 0, cbText);
    if (bNegative)
    {
        szText[0] = '-';
    }
    for(int32_t i=0; i<nMaxSect; ++i)
    {
        if ('\0' == szTemp[i][0]) continue;
        for(int32_t j=i; j<nMaxSect; ++j)
        {
            safe_strcat(szText, cbText, szTemp[j]);
            if (j != nMaxSect-1)
            {
                safe_strcat(szText, cbText, ",");
            }
        }
        break;
    }

    return (const char*)szText;
}

static const char* _rtrim(char *pBuf, size_t cbBuf)
{
    if (NULL != pBuf)
    {
        int32_t nLen = (int)strlen(pBuf);
        nLen = std::min((size_t)nLen, cbBuf);
        unsigned char c;
        do
        {
            c = (unsigned char)pBuf[nLen-1];
            if ( !isspace(c) )
            {
                break;
            }
            --nLen;
        }while(nLen > 0);
        pBuf[nLen] = '\0';
    }
    return (const char*)pBuf;
}

const char *rtrim(char *pBuf)
{
    return _rtrim(pBuf, (size_t)-1);
}

static bool is_crlf(unsigned char c)
{
    if (c == 0x0D || c == 0x0A)
        return true;
    return false;
}

static const char* _rtail(char *pBuf, size_t cbBuf)
{
    if (NULL != pBuf)
    {
        int32_t nLen = (int)strlen(pBuf);
        nLen = std::min((size_t)nLen, cbBuf);
        unsigned char c;
        do
        {
            c = (unsigned char)pBuf[nLen-1];
            if (isspace(c) || is_crlf(c))
            {
                --nLen;
            }
            else
            {
                break;
            }
        }while(nLen > 0);
        pBuf[nLen] = '\0';
    }
    return (const char*)pBuf;
}

const char* rtail(char *pBuf)
{
    return _rtail(pBuf, (size_t)-1);
}

static const char* _ltrim(char *pBuf, size_t cbBuf)
{
    if (NULL != pBuf)
    {
        int32_t nLen = (int)strlen(pBuf);
        nLen = std::min((size_t)nLen, cbBuf);
        int32_t nIndex = 0;
        unsigned char c;
        do {
            c = (unsigned char)pBuf[nIndex];
            if ( !isspace(c) )
            {
                break;
            }
            ++nIndex;
        } while(nIndex < (nLen - 1) );

        if (nIndex > 0)
        {
            //óD?°μ?????
            memmove(&pBuf[0], &pBuf[nIndex], (size_t)(nLen-nIndex) );
            pBuf[nLen-nIndex] = '\0';
        }
    }
    return (const char*)pBuf;
}

const char* ltrim(char *pBuf)
{
    return _ltrim(pBuf, (size_t)-1);
}

const char* trim(char *pBuf)
{
	ltrim(pBuf);
	rtrim(pBuf);
	return pBuf;
}

void StrToHex(const char *buffer, size_t nLen, byte_t *data, size_t &nDataLen)
{
    if (NULL == buffer || NULL == data)
    {
        return;
    }

    const size_t nSize = nDataLen;
    nDataLen = 0;
    char c[3] = "";
    char *topstring = NULL;
    for(size_t i=0; i<std::min(nLen, nSize*2); i+=2)
    {
        c[0] = buffer[i];
        if (i < nLen-1)
        {
            c[1] = buffer[i+1];
        }
        else
        {
            c[1] = '\0';
        }
        data[nDataLen++] = (byte_t)strtoul(c, &topstring, 16);
    }
}

const char* HexToStr(const byte_t *buffer, size_t nLen, char *data, size_t nDataLen)
{
    if (NULL == buffer || NULL == data)
    {
        return "";
    }

    unsigned char c;
    for(size_t i=0; i<std::min(nLen, nDataLen/2); ++i)
    {
        c = buffer[i];
        safe_sprintf(&data[i*2], 3, "%02X", c);
    }
    return (const char*)data;
}

bool StrReplace(char *buffer, size_t nLen, const char *source, const char *target)
{
    if (NULL==buffer || NULL==source || NULL==target)
    {
        return false;
    }

    size_t nDataLen = strlen(buffer);
    const size_t nSrcLen = strlen(source);
    std::string strData;
    char *top = buffer;
    do {
        char *pc = strstr(top, source);
        if (NULL == pc)
        {
            strData.append(top);
            break;
        }
        int32_t nCpyLen = (int)(pc - top);
        strData.append(top, nCpyLen);
        strData.append(target);
        top = (pc + nSrcLen);
        if (nDataLen <= (nCpyLen + nSrcLen) ) break;
        nDataLen -= (nCpyLen + nSrcLen);
    } while(nDataLen > 0);

    if (nLen < strData.length())
    {
        return false;
    }
    return (safe_strcpy(buffer, nLen, strData.c_str()) > 0);
}


bool StrToTwinsInt(const char *buf, TwinsInt &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[2][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]));
#else
    int cnt = sscanf(buf, "%s %s", szVal[0], szVal[1]);
#endif
    if (cnt!= 2)
    {
        return false;
    }

    char *stopstring;
    result.x = (int)strtol(szVal[0], &stopstring, 0);
    result.y = (int)strtol(szVal[1], &stopstring, 0);
    return true;
}

bool StrToTriplesInt(const char *buf, TriplesInt &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[3][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]),
        szVal[2], sizeof(szVal[2]));
#else
    int cnt = sscanf(buf, "%s %s %s",
        szVal[0], szVal[1], szVal[2]);
#endif
    if(cnt!=3)
    {
        return false;
    }

    char *stopstring;
    result.x = (int)strtol(szVal[0], &stopstring, 0);
    result.y = (int)strtol(szVal[1], &stopstring, 0);
    result.z = (int)strtol(szVal[2], &stopstring, 0);
    return true;
}

bool StrToQuadInt(const char *buf, QuadInt &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[4][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s %s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]),
        szVal[2], sizeof(szVal[2]),
        szVal[3], sizeof(szVal[3]));
#else
    int cnt = sscanf(buf, "%s %s %s %s",
        szVal[0], szVal[1], szVal[2], szVal[3]);
#endif
    if(cnt!=4)
    {
        return false;
    }

    char *stopstring;
    result.x = (int)strtol(szVal[0], &stopstring, 0);
    result.y = (int)strtol(szVal[1], &stopstring, 0);
    result.z = (int)strtol(szVal[2], &stopstring, 0);
    result.a = (int)strtol(szVal[3], &stopstring, 0);
    return true;
}

bool StrToTwinsNumeric(const char *buf, TwinsNumeric &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[2][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]));
#else
    int cnt = sscanf(buf, "%s %s", szVal[0], szVal[1]);
#endif
    if (cnt!=2)
    {
        return false;
    }

    char *stopstring;
    result.x = strtod(szVal[0], &stopstring);
    result.y = strtod(szVal[1], &stopstring);
    return true;
}

bool StrToTriplesNumeric(const char *buf, TriplesNumeric &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[3][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]),
        szVal[2], sizeof(szVal[2]));
#else
    int cnt = sscanf(buf, "%s %s %s",
        szVal[0], szVal[1], szVal[2]);
#endif
    if(cnt!=3)
    {
        return false;
    }

    char *stopstring;
    result.x = strtod(szVal[0], &stopstring);
    result.y = strtod(szVal[1], &stopstring);
    result.z = strtod(szVal[2], &stopstring);
    return true;
}

bool StrToQuadNumeric(const char *buf, QuadNumeric &result)
{
    if (EMPTY_STR(buf))
    {
        return false;
    }

    char szVal[4][128] = {"", };
#if (defined WIN32 && _MSC_VER>=1400)
    int cnt = sscanf_s(buf, "%s %s %s %s",
        szVal[0], sizeof(szVal[0]),
        szVal[1], sizeof(szVal[1]),
        szVal[2], sizeof(szVal[2]),
        szVal[3], sizeof(szVal[3]));
#else
    int cnt = sscanf(buf, "%s %s %s %s",
        szVal[0], szVal[1], szVal[2], szVal[3]);
#endif
    if(cnt!=4)
    {
        return false;
    }

    char *stopstring;
    result.x = strtod(szVal[0], &stopstring);
    result.y = strtod(szVal[1], &stopstring);
    result.z = strtod(szVal[2], &stopstring);
    result.a = strtod(szVal[3], &stopstring);
    return true;
}

////////////////////////////////////////////////////////////////////////////
//
inline int timestr_getvalue(const char *timestr, size_t txtlen, const char *fmt, const char *timefmt, size_t fmtlen)
{
    const char *pc = strstr(fmt, timefmt);
    if (pc != NULL)
    {
        int nIndex = (int)(pc - fmt);
        if (nIndex < (int)txtlen)
        {
            char temp[16] = "";
			safe_strncpy(temp, sizeof(temp), &timestr[nIndex], fmtlen);
            return atol(temp);
        }
    }
    return -1;
}

bool DecodeTimeStr(const char *strTime, int &year, int &month, int &day,
    int &hour, int &minute, int &second, const char *fmt)
{
    if (EMPTY_STR(strTime) || EMPTY_STR(fmt))
    {
        return false;
    }

    int nDecodeCount(0);
    size_t txtlen = strlen(strTime);
    //获取日期
    int nValue = timestr_getvalue(strTime, txtlen, fmt, "yyyy", 4);
    if (nValue != -1)
    {
        year = nValue;
        ++nDecodeCount;
    }
    else
    {
        nValue = timestr_getvalue(strTime, txtlen, fmt, "yy", 2);
        if (nValue != -1)
        {
            year = 1900 + nValue;
            ++nDecodeCount;
        }
    }
    nValue = timestr_getvalue(strTime, txtlen, fmt, "mm", 2);
    if (nValue != -1)
    {
        month = nValue;
        ++nDecodeCount;
    }
    nValue = timestr_getvalue(strTime, txtlen, fmt, "dd", 2);
    if (nValue != -1)
    {
        day = nValue;
        ++nDecodeCount;
    }

    //获取时间
    nValue = timestr_getvalue(strTime, txtlen, fmt, "HH", 2);
    if (nValue != -1)
    {
        hour = nValue;
        ++nDecodeCount;
    }
    nValue = timestr_getvalue(strTime, txtlen, fmt, "MM", 2);
    if (nValue != -1)
    {
        minute = nValue;
        ++nDecodeCount;
    }
    nValue = timestr_getvalue(strTime, txtlen, fmt, "SS", 2);
    if (nValue != -1)
    {
        second = nValue;
        ++nDecodeCount;
    }
    return (nDecodeCount>0);
}

time_t StrToTime(const char *strTime, const char *fmt)
{
    if ( EMPTY_STR(strTime) || EMPTY_STR(fmt) )
    {
        return (time_t)0;
    }

    int nYear = 1970;
    int nMonth = 1;
    int nDay = 1;
    int nHour = 0;
    int nMinute = 0;
    int nSecond = 0;
    if (!DecodeTimeStr(strTime, nYear, nMonth, nDay, nHour, nMinute, nSecond, fmt))
    {
        return (time_t)0;
    }

    //转换成时间
    //对 0000-00-00 00:00:00 进行特殊处理
    if (nYear==nMonth && nMonth==nDay && nDay==nHour 
            && nHour==nMinute && nMinute==nSecond && nSecond==0)
    {
        return (time_t)0;
    }
    if (nYear==0 && nMonth==0 && nDay==1 &&
            nHour==8 && nMinute==0 && nSecond==0)
    {
        return (time_t)0;
    }

   // struct tm {
   //     int tm_sec;     // 秒 – 取值区间为[0,59] 
   //     int tm_min;     // 分 - 取值区间为[0,59] 
   //     int tm_hour;    // 时 - 取值区间为[0,23] 
   //     int tm_mday;    // 一个月中的日期 - 取值区间为[1,31] 
   //     int tm_mon;     // 月份（从一月开始，0代表一月） - 取值区间为[0,11] 
   //     int tm_year;    // 年份，其值等于实际年份减去1900 
   //     int tm_wday;    // 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 
   //     int tm_yday;    // 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 
   //     int tm_isdst;   // 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。
   // };
   ///
    struct tm tmValue = {nSecond, nMinute, nHour, nDay, nMonth-1, nYear-1900, 0, 0, 0};
    time_t tValue = mktime(&tmValue);
    return tValue;
}

int32_t Time2Int(const char *strTime, const char *fmt)
{
    if ( EMPTY_STR(strTime) || EMPTY_STR(fmt) )
    {
        return 0;
    }

    int nYear = 1970;
    int nMonth = 1;
    int nDay = 1;
    int nHour = 0;
    int nMinute = 0;
    int nSecond = 0;
    if (!DecodeTimeStr(strTime, nYear, nMonth, nDay, nHour, nMinute, nSecond, fmt))
    {
        return (time_t)0;
    }

    int32_t val = nHour*3600 + nMinute*60 + nSecond;
    return val;
}

int32_t TimeToStr(time_t tTime, char *szTime, size_t cbTime, const char *fmt)
{
    if (NULL==szTime || 0==cbTime || EMPTY_STR(fmt))
    {
        return -1;
    }

    //tTime==0 表示输入'0000-00-00 00:00:00'
    struct tm tmTime = {0};
    if (0 != tTime)
    {
		struct tm *tv = localtime_r(&tTime, &tmTime);	
		if (NULL == tv)
        {
            return -2;
        }
    }

    char szYear[32] = "";
    char szYear2[16] = "";
    char szMonth[16] = "";
    char szDay[16] = "";
    char szHour[16] = "";
    char szMinute[16] = "";
    char szSecond[16] = "";

    if (0 == tTime)
    {
        safe_strcpy(szYear,    5, "0000");
        safe_strcpy(szYear2,   3, "00");
        safe_strcpy(szMonth,   3, "00");
        safe_strcpy(szDay,     3, "00");
        safe_strcpy(szHour,    3, "00");
        safe_strcpy(szMinute,  3, "00");
        safe_strcpy(szSecond,  3, "00");
    }
    else
    {
        safe_sprintf(szYear,   5, "%04d", tmTime.tm_year+1900);
        safe_sprintf(szYear2,  3, "%02d", (tmTime.tm_year+1900) % 100);
        safe_sprintf(szMonth,  3, "%02d", tmTime.tm_mon+1);
        safe_sprintf(szDay,    3, "%02d", tmTime.tm_mday);
        safe_sprintf(szHour,   3, "%02d", tmTime.tm_hour);
        safe_sprintf(szMinute, 3, "%02d", tmTime.tm_min);
        safe_sprintf(szSecond, 3, "%02d", tmTime.tm_sec);
        //safe_sprintf(szTime, cbTime, fmt);
    }
    safe_strcpy(szTime, cbTime, fmt);

    size_t nLen = cbTime;
    StrReplace(szTime, nLen, "yyyy", szYear);
    StrReplace(szTime, nLen, "mm",   szMonth);
    StrReplace(szTime, nLen, "dd",   szDay);
    StrReplace(szTime, nLen, "HH",   szHour);
    StrReplace(szTime, nLen, "MM",   szMinute);
    StrReplace(szTime, nLen, "SS",   szSecond);
    StrReplace(szTime, nLen, "yy",   szYear2);

    return 0;

}

const char * EncodeTimeStr(const char *str, char *szOutput, size_t cbOutput)
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);

	struct tm tmValue;
	struct tm *now = localtime_r(&tv.tv_sec, &tmValue);
	if (NULL == now)
	{
		return "";
	}
	char year[6] = "";
	char year2[4] = "";
	char month[4] = "";
	char day[4] = "";
	char hour[4] = "";
	char minute[4] = "";
	char second[4] = "";
	SAFE_SPRINTF(year,   "%4d",  now->tm_year+1900);
	SAFE_SPRINTF(year2,  "%02d", (now->tm_year+1900)%100);
	SAFE_SPRINTF(month,  "%02d", now->tm_mon+1);
	SAFE_SPRINTF(day,    "%02d", now->tm_mday);
	SAFE_SPRINTF(hour,   "%02d", now->tm_hour);
	SAFE_SPRINTF(minute, "%02d", now->tm_min);
	SAFE_SPRINTF(second, "%02d", now->tm_sec);

	safe_strcpy(szOutput, cbOutput, str);
	StrReplace(szOutput, cbOutput, "yyyy", year);
	StrReplace(szOutput, cbOutput, "mm",   month);
	StrReplace(szOutput, cbOutput, "dd",   day);
	StrReplace(szOutput, cbOutput, "HH",   hour);
	StrReplace(szOutput, cbOutput, "MM",   minute);
	StrReplace(szOutput, cbOutput, "SS",   second);
	StrReplace(szOutput, cbOutput, "yy",   year2);

	return szOutput;
}

const char* FormatNumber(int64_t val, CMyString &str)
{
	str.clear();
	int64_t val2 = llabs(val);
	if (val < 0)
	{
		str = "-";
	}
	std::vector<int32_t> arVal;
	do {
		int32_t v1 = val2 % 1000;
		val2 /= 1000;
		arVal.push_back(v1);
	}while(val2 > 0);
	for(std::vector<int32_t>::reverse_iterator it(arVal.rbegin());
			it != arVal.rend(); ++it)
	{
		str.Append("%d,", *it);
	}
	str.erase(str.length()-1);
	return str.c_str();
}
const char* FormatNumber(int32_t val, CMyString &str)
{
	return FormatNumber((int64_t)val, str);
}

const char* FormatNumber(uint64_t val, CMyString &str)
{
	str.clear();
	std::vector<int32_t> arVal;
	do {
		int32_t v1 = val % 1000;
		val /= 1000;
		arVal.push_back(v1);
	}while(val > 0);
	for(std::vector<int32_t>::reverse_iterator it(arVal.rbegin());
			it != arVal.rend(); ++it)
	{
		str.Append("%d,", *it);
	}
	str.erase(str.length()-1);
	return str.c_str();
}
const char* FormatNumber(uint32_t val, CMyString &str)
{
	return FormatNumber((uint64_t)val, str);
}

const char* FormatNumber(double val, CMyString &str)
{
	str.clear();
	int64_t val2 = (int64_t)val;
	double val3 = fabs(val - val2);
	FormatNumber(val2, str);
	if (val3 > 0.000001)
	{
		CMyString str2("%f", val3);
		str2.erase(0, 1);
		str+=str2;

		int32_t index(str.length()-1);
		bool bTrim(false);
		for(int32_t i=str.length()-1; i>=0; --i)
		{
			if (str[i]!='0')
			{
				if (bTrim)
				{
					str.erase(index);
				}
				break;
			}
			index = i;
			bTrim = true;
		}
	}
	return str.c_str();
}

void decode_double(double val, int &precision, int &scale)
{
    val += 0.0000005f;    // 防止出现 4.9999999 的情况
    CMyString strData("%lf", val);
    size_t pos = strData.find(".");
    if (pos == std::string::npos)
    {
        // 类似 1234
        //printf("debug>1\n");
        scale = 0;
        precision = strData.length();
        return;
    }

    precision = pos;
    if (pos+1 == strData.length())
    {
        // 类似 1234.
        //printf("debug>2\n");
        scale = 0;
        return;
    }

    size_t pos1 = strData.find_first_of("0", pos+1);
    if (pos1==std::string::npos)
    {
        // 类似：1.2345
        //printf("debug>3 pos=%u\n", pos);
        scale = strData.length() - pos - 1;
        return;
    }

    size_t pos2 = strData.find_last_of("0");
    if (pos1 == pos2)
    {
        // 只有一个0的情况
        if (pos2+1 == strData.length())
        {
            // 尾部的0: 1.2340
            //printf("debug>4, pos=%u, pos1=%u\n", pos, pos1);
            scale = pos1 - pos - 1;
        }
        else
        {
            // 中间的0,不能移除: 1.1023
            //printf("debug>4-1, pos=%u, pos1=%u\n", pos, pos1);
            scale = strData.length() - pos - 1;
        }
        return;
    }
    
    // 存在多个0的情况，如： 
    // 1.23056007       => 1.23056   
    // 1.2304050609001  => 1.2304050609   
    // 1.23040500000123 => 1.230405
    while(pos2>pos1)
    {
        uint8_t c = (uint8_t)strData[pos2];
        if (c != '0')
        {
            break;
        }
        --pos2;
    }
    if (pos1==pos2)
        scale = pos2-pos-1;
    else
        scale = pos2-pos;
    //printf("debug>5, pos=%u, pos1=%u, pos2=%u\n", pos, pos1, pos2);
}

CMyString double_to_str(double val, int precision)
{
    int32_t precision2(0), scale(0);
    decode_double(val, precision2, scale);
    scale = std::min(scale, precision);
    CMyString strFormat("%%.%dlf", scale);
    CMyString strData(strFormat.c_str(), val);
    return strData;
}

