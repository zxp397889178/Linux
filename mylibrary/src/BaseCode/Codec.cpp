#include <stdio.h>
#include <stdlib.h>
#include <string>
#define USING_STATIC_LIBICONV   0
#define LIBICONV_PLUG
#include <iconv.h>
#include "StringFunc.h"
#include "Misc.h"
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string.h>


/*!
对字符串进行语言编码转换
参数:
    fromCharset 原始编码，比如"GB2312",的按照iconv支持的写
    toCharset   转换的目的编码
    toStr       转换后的数据保存到这个指针里，需要在外部分配内存
    cbToStr     存储转换后数据的内存大小
    fromStr     原始需要转换的字符串
    cbFromStr   原始字符串长度
返回值：
    0           转换成功
    -1          参数错误
    -2          不支持字符串转换
    -3          toStr的空间不足
    -4          fromStr有不可转换的字符，转换失败
    -5          其他错误
*/
int Convert(const char *fromCharset, const char *toCharset, 
    char *toStr, size_t &cbToStr, const char *fromStr, const size_t cbFromStr)
{
#define ICONV_CONST

    if (EMPTY_STR(fromCharset) || EMPTY_STR(toCharset) || EMPTY_STR(fromStr) || cbFromStr<1 ||
        NULL==toStr || cbToStr<1)
    {
        return -1;
    }

    iconv_t cd = iconv_open(toCharset, fromCharset);
    if (cd == (iconv_t)-1)
    {
        return -2;
    }

    int result(0);
    size_t cbLeft(cbToStr);
    size_t ret = iconv(cd, (ICONV_CONST char**)&fromStr, (size_t*)&cbFromStr,
        (char**)&toStr, (size_t*)&cbLeft);
    if ( ret==(size_t)-1 )
    {
        int nError = errno;
        switch(nError)
        {
        case EINVAL:
            //EINVAL  An  incomplete  multibyte sequence has been encoun­tered in the input.
            //We'll just truncate it and ignore it.
            break;
        case E2BIG:
            //E2BIG   There is not sufficient room at *outbuf.
            result = -3;
            break;
        case EILSEQ:
            // EILSEQ An invalid multibyte sequence has been encountered in the input.
            // Bad input, we can't really recover from this. 
            result = -4;
            break;
        default:
            //其他错误
            result = -5;
            break;
        }
    }
    cbToStr -= cbLeft;
    iconv_close(cd);
    return result;
}

int utf8_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size)
{
    return Convert("UTF-8", "GBK", to_string, to_size, from_string, from_size);
}

int gbk_to_utf8(char *to_string, size_t &to_size, const char *from_string, const size_t from_size)
{
    return Convert("GBK", "UTF-8", to_string, to_size, from_string, from_size);
}

int utf16_to_gbk(char *to_string, size_t &to_size, const char *from_string, const size_t from_size)
{
    return Convert("UTF-16", "GBK", to_string, to_size, from_string, from_size);
}

int gbk_to_utf16(char *to_string, size_t &to_size, const char *from_string, const size_t from_size)
{
    return Convert("GBK", "UTF-16", to_string, to_size, from_string, from_size);
}

//1.如果所有字符都是ascii码,则判断为ascii而非utf8编码(ascii字符占1个字节，从1-127, 最高位为0)
//2.当为多字节时，先判断第一个字节，从最高位开始，直到出现第一个0,位上为1的个数必须>=2&&<=6，检测剩下的字节，必须是10xx xxxx的形式。
//http://www.cppblog.com/izualzhy/archive/2012/12/03/195933.html
//http://www.cppblog.com/izualzhy/archive/2012/02/27/166633.html
bool IsUTF8String(const char* str, int32_t length) 
{ 
    int i = 0; 
    int nBytes = 0;//UTF8可用1-6个字节编码,ASCII用一个字节 
    unsigned char chr = 0; 
    bool bAllAscii = true;//如果全部都是ASCII,说明不是UTF-8 

    while (i < length) 
    { 
        chr = *(str + i); 
        if ((chr & 0x80) != 0) 
            bAllAscii = false; 
        if (nBytes == 0)//计算字节数 
        { 
            if ((chr & 0x80) != 0) 
            { 
                while ((chr & 0x80) != 0) 
                { 
                    chr <<= 1; 
                    nBytes++; 
                } 
                if (nBytes < 2 || nBytes > 6) 
                    return false;//第一个字节最少为110x xxxx 
                nBytes--;//减去自身占的一个字节 
            } 
        } 
        else//多字节除了第一个字节外剩下的字节 
        { 
            if ((chr & 0xc0) != 0x80) 
                return false;//剩下的字节都是10xx xxxx的形式 
            nBytes--; 
        } 
        ++i; 
    } 
    if (bAllAscii) 
        return false; 
    return nBytes == 0; 
} 

static int getFullPath(const char* pszFile, char* pszBuffer, size_t cbBuffer)
{
    char buffer[1024] = "";
	if (NULL == GetFullPathName(buffer, sizeof(buffer), pszFile))
    {
        return -1;
    }

	safe_sprintf(pszBuffer, cbBuffer, "%s/%s", buffer, pszFile);
	return 0;
}


int FileConvert(const char *fromCharset, const char *toCharset,
    const char* fromFile, const char* toFile, const char* lineEnding)
{
    if (EMPTY_STR(fromCharset) || EMPTY_STR(toCharset))
    {
        return -1;
    }

    //打开输入文件
    std::ifstream ifile;
    ifile.open(fromFile, std::ios::in);
    if (!ifile.is_open())
    {
        return -2;
    }

    //打开临时的输出文件
    char szPath[1024] = "";
    if (0 != getFullPath(toFile, szPath, sizeof(szPath)))
    {
        return -3;
    }
    char szFile[256] = "";
	safe_strcpy(szFile, sizeof(szFile), "fileConvert_tempXXXXXX");
	mkstemp(szFile);
    char szOutput[256] = "";
	safe_sprintf(szOutput, sizeof(szOutput), "%s/%s", szPath, szFile);
    FILE *fp = fopen(szOutput, "wb, ccs=UTF-8");
    if (fp == NULL)
    {
        return -4;
    }

    //行结束符转换
    unsigned char szLineEnding[16] = {0};
    size_t cbLineEnding=sizeof(szLineEnding);
    size_t cbInbuf = strlen(lineEnding);
    int ret = Convert(fromCharset, toCharset, (char*)szLineEnding, cbLineEnding, lineEnding, cbInbuf);
    if (ret<0)
    {
        return -5;
    }

    //逐行转码
    bool bFailure(false);
    char fromBuff[1024] = "";
    size_t cbFrom = sizeof(fromBuff);
    unsigned char toBuff[1024] = {0};
    size_t cbTo = sizeof(toBuff);
    while(!ifile.eof())
    {
        cbFrom = sizeof(fromBuff);
        cbTo = sizeof(toBuff);
        memset(fromBuff, 0, cbFrom);
        ifile.getline(fromBuff, cbFrom);
        cbFrom = strlen(fromBuff);

        if (cbFrom > 0)
        {
            ret = Convert(fromCharset, toCharset, (char*)toBuff, cbTo, fromBuff, cbFrom);
            if (ret<0)
            {
                bFailure = true;
                break;
            }
            fwrite(toBuff, 1, cbTo, fp);
        }
        fwrite(szLineEnding, 1, cbLineEnding, fp);
    }
    ifile.close();
    fclose(fp);
    if (bFailure)
    {
        return -6;
    }

    if (0 != remove(toFile))
    {
        std::cout << "Warn: remove file " << toFile << " failure, errno=" << errno << std::endl;
    }
    if (0 != rename(szOutput, toFile))
    {
        std::cout << "Err: rename file to " << toFile << " failure, errno=" << errno << std::endl;
        return -7;
    }

    return 0;
}

// http://blog.chinaunix.net/uid-14348211-id-2821150.html
static bool isUtf8SpecialByte(uint8_t c)
{
	static uint8_t s_specialByte = 0x02; // binary 00000010
	return ((c>>6) == s_specialByte);
}

/* return -1 error, not utf8 coding
 * 		   0 utf8 char less 10%
 * 		   1 utf8 char great 90%
 * */
int32_t IsUtf8Code(const std::string &str, int32_t &length)
{
	static uint8_t s_oneByte   = 0x00;	// binary 00000000
	static uint8_t s_twoByte   = 0x06;	// binary 00000110
	static uint8_t s_threeByte = 0x0E;	// binary 00001110
	static uint8_t s_fourByte  = 0x1E;	// binary 00011110
	static uint8_t s_fiveByte  = 0x3E;	// binary 00111110
	static uint8_t s_sixByte   = 0x7E;  // binary 01111110

	int32_t utf8_yes(0), utf8_no(0);
	int32_t result(0);
	int32_t ascii_yes(0);
	int32_t str_length = (int32_t)str.length();
	for(int32_t index(0); index<str_length; )
	{
		uint8_t c = (uint8_t)str[index];
		if ((c>>7) == s_oneByte)
		{
			++index;
			++ascii_yes;
			continue;
		}
		else if ((c>>5)==s_twoByte)
		{
			if (index+1 >= str_length) { result =-1;  break; }
			uint8_t k = (uint8_t)str[index+1];
			if (isUtf8SpecialByte(k))
			{
				++utf8_yes;
				index += 2;
				continue;
			}
		}
		else if ((c>>4)==s_threeByte)
		{
			if (index+2 >= str_length)  { result = -1; break; }
			uint8_t m = (uint8_t)str[index+1];
			uint8_t n = (uint8_t)str[index+2];
			if (isUtf8SpecialByte(m) && isUtf8SpecialByte(n))
			{
				++utf8_yes;
				index += 3;
				continue;
			}
		}
		else if ((c>>3)==s_fourByte)
		{
			if (index+3 >= str_length)  { result = -1; break; }
			uint8_t k = (uint8_t)str[index+1];
			uint8_t m = (uint8_t)str[index+2];
			uint8_t n = (uint8_t)str[index+3];
			if (isUtf8SpecialByte(k) && isUtf8SpecialByte(m) &&
					isUtf8SpecialByte(n))
			{
				++utf8_yes;
				index+=4;
				continue;
			}
		}
		else if ((c>>2)==s_fiveByte)
		{
			if (index+4 >= str_length)  { result = -1; break; }
			uint8_t k = (uint8_t)str[index+1];
			uint8_t m = (uint8_t)str[index+2];
			uint8_t n = (uint8_t)str[index+3];
			uint8_t p = (uint8_t)str[index+4];
			if (isUtf8SpecialByte(k) && isUtf8SpecialByte(m) &&
					isUtf8SpecialByte(n) && isUtf8SpecialByte(p))
			{
				++utf8_yes;
				index+=5;
				continue;
			}
		}
		else if ((c>>1)==s_sixByte)
		{
			if (index+5 >= str_length)  { result = -1; break; }
			uint8_t k = (uint8_t)str[index+1];
			uint8_t m = (uint8_t)str[index+2];
			uint8_t n = (uint8_t)str[index+3];
			uint8_t p = (uint8_t)str[index+4];
			uint8_t q = (uint8_t)str[index+5];
			if (isUtf8SpecialByte(k) && isUtf8SpecialByte(m) &&
					isUtf8SpecialByte(n) && isUtf8SpecialByte(p) &&
					isUtf8SpecialByte(q) )
			{
				++utf8_yes;
				index+=6;
				continue;
			}
		}
		result = 0;
		++utf8_no;
		++index;
	}

	if (-1 == result) return -1;

	length = ascii_yes + utf8_yes + utf8_no;
	return utf8_yes;
}

int32_t IsGB2312Code(const std::string &str, int32_t &length)
{
	static uint8_t s_oneByte = 0x00;	// binary 00000000

	int32_t gb2312_yes(0), gb2312_no(0), ascii_yes(0);
	int32_t str_len = (int32_t)str.length();
	int32_t result(0);
	for(int32_t index(0); index < str_len; )
	{
		uint8_t c = (uint8_t)str[index];
		if ((c>>7)==s_oneByte)
		{
			++index;
			++ascii_yes;
			continue;
		}
		else if (c>=0xA1 && c<= 0xF7)
		{
			if (index+1 >= str_len)
			{
				result = -1;
				break;
			}
			uint8_t k = (uint8_t)str[index+1];
			if (k>=0xA1 && k<0xFE)
			{
				++gb2312_yes;
				index += 2;
				continue;
			}
		}
		++gb2312_no;
		index += 2;
	}

	if (-1 == result) return -1;
	length = ascii_yes + gb2312_yes + gb2312_no; 
	return gb2312_yes;
}


int32_t IsBig5Code(const std::string &str, int32_t &length)
{
	static uint8_t s_oneByte = 0x00;		//binary 00000000
	
	int32_t big5_yes(0), big5_no(0), ascii_yes(0);
	int32_t str_len = (int32_t)str.length();
	int32_t result(0);
	for(int32_t index(0); index < str_len; )
	{
		uint8_t c = (uint8_t)str[index];
		if ((c>>7)==s_oneByte)
		{
			++index;
			++ascii_yes;
			continue;
		}
		else if (c>=0xA1 && c<= 0xF9)
		{
			if (index+1 >= str_len)
			{
				result = -1;
				break;
			}
			uint8_t k = (uint8_t)str[index+1];
			if (k>=0x40 && k<0xFE)
			{
				++big5_yes;
				index += 2;
				continue;
			}
		}
		++big5_no;
		index += 2;
	}

	if (-1 == result) return -1;
	length = ascii_yes + big5_yes + big5_no; 
	return big5_yes;
}

int32_t IsGBKCode(const std::string &str, int32_t &length)
{
	static uint8_t s_oneByte = 0x00;		//binary 00000000
	
	int32_t gbk_yes(0), gbk_no(0), ascii_yes(0);
	int32_t str_len = (int32_t)str.length();
	int32_t result(0);
	for(int32_t index(0); index < str_len; )
	{
		uint8_t c = (uint8_t)str[index];
		if ((c>>7)==s_oneByte)
		{
			++index;
			++ascii_yes;
			continue;
		}
		else if (c>=0x81 && c<= 0xFE)
		{
			if (index+1 >= str_len)
			{
				result = -1;
				break;
			}
			uint8_t k = (uint8_t)str[index+1];
			if (k>=0x40 && k<0xFE)
			{
				++gbk_yes;
				index += 2;
				continue;
			}
		}
		++gbk_no;
		index += 2;
	}

	if (-1 == result) return -1;
	length = ascii_yes + gbk_yes + gbk_no; 
	return gbk_yes;
}


//////////////////////////////////////////////////////////////////////////
//
struct CharSetMap
{
    const char* mysql_charset;
    const char* iconv_charset;
};

CharSetMap g_mapCharset[]={
    {"big5",    "BIG5"},
    {"latin1",  "CP1252"},
    {"ascii",   "ASCII"},
    {"ujis",    "EUC−JP"},
    {"gb2312",  "GB2312"},
    {"gbk",     "GBK"},
    {"utf8",    "UTF-8"},
    {"ucs2",    "UCS-2"},
    {"cp1250",  "CP1250"},
    {"cp850",   "CP850"},
    {"euckr",   "ECU-KR"},
    {"koi8u",   "KOI8-U"},
    {"cp932",   "CP932"},
    {"eucjpms", "EUC−JP"},
};

const char* GetIconvCharset(const char *mysql_charset)
{
    size_t nCount = sizeof(g_mapCharset)/sizeof(g_mapCharset[0]);
    for(size_t i=0; i<nCount; ++i)
    {
        if (!stricmp(mysql_charset, g_mapCharset[i].mysql_charset))
        {
            return g_mapCharset[i].iconv_charset;
        }
    }
    return NULL;
}

const char* GetMysqlCharset(const char *iconv_charset)
{
    size_t nCount = sizeof(g_mapCharset)/sizeof(g_mapCharset[0]);
    for(size_t i=0; i<nCount; ++i)
    {
        if (!stricmp(iconv_charset, g_mapCharset[i].iconv_charset))
        {
            return g_mapCharset[i].mysql_charset;
        }
    }
    return NULL;
}

