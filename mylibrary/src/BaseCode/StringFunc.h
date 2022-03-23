/*
 * =====================================================================================
 *
 *       Filename:  StringFunc.h
 *
 *    Description:  string function 
 *
 *        Version:  1.0
 *        Created:  2014/11/11 00时15分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *	 url  http://www.jb51.net/article/37410.htm
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_STRINGFUNC_H_
#define _MYLIB_BASECODE_STRINGFUNC_H_

#include <string>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include "myconf.h"
#include "MyString.h"

#define MAX_BUFFER_SIZE		4096
#define __min(a, b)			((a)>(b)?(b):(a))

template<typename T>
struct TwinsParam
{
    T x, y;
    TwinsParam()
        : x(T(0)), y(T(0))
    {
    }
};
typedef TwinsParam<int32_t>     TwinsInt;
typedef TwinsParam<float>       TwinsFloat;
typedef TwinsParam<double>      TwinsNumeric;

template<typename T>
struct TriplesParam
{
    T x, y, z;
    TriplesParam()
        : x(T(0)), y(T(0)), z(T(0))
    {
    }
};
typedef TriplesParam<int32_t>   TriplesInt;
typedef TriplesParam<float>     TriplesFloat;
typedef TriplesParam<double>    TriplesNumeric;

template<typename T>
struct QuadParam
{
    T x, y, z, a;
    QuadParam()
        : x(T(0)), y(T(0)), z(T(0)), a(T(0))
    {
    }
};
typedef QuadParam<int32_t>      QuadInt;
typedef QuadParam<float>        QuadFloat;
typedef QuadParam<double>       QuadNumeric;

												
/* string convert to other type function
 * */
int32_t  StrToLong   (const char *str);
int32_t  StrToLong   (const char *str, uint8_t base);
#define StrToInt32	StrToLong
uint32_t StrToUint32 (const char *str);
uint32_t StrToUint32 (const char *str, uint8_t base);
int64_t  StrToInt64  (const char *str);
int64_t  StrToInt64  (const char *str, uint8_t base);
uint64_t StrToUint64 (const char *str);
uint64_t StrToUint64 (const char *str, uint8_t base);
double   StrToDouble (const char *str);
#define StrToFloat	StrToDouble
bool     StrToBoolean(const char *str);
time_t StrToTime(const char *strTime, const char *fmt = "yyyy-mm-dd HH:MM:SS");
int32_t TimeToStr(time_t tTime, char *szTime, size_t cbTime, const char *fmt="yyyy-mm-dd HH:MM:SS");
int32_t Time2Int(const char *strTime, const char *fmt = "HH:MM:SS");
#define Time2Seconds   Time2Int

// 格式化输出整数
const char* FormatNumber(int64_t val, CMyString &str);
const char* FormatNumber(int32_t val, CMyString &str);
const char* FormatNumber(uint64_t val, CMyString &str);
const char* FormatNumber(uint32_t val, CMyString &str);
const char* FormatNumber(double val, CMyString &str);


/* 以下函数为将字符串转换二元、三元、四元结构的值，暂时只支持整型和浮点型转换
*/
bool   StrToTwinsInt(const char *buf, TwinsInt &result);
bool   StrToTriplesInt(const char *buf, TriplesInt &result);
bool   StrToQuadInt(const char *buf, QuadInt &result);
bool   StrToTwinsNumeric(const char *buf, TwinsNumeric &result);
bool   StrToTriplesNumeric(const char *buf, TriplesNumeric &result);
bool   StrToQuadNumeric(const char *buf, QuadNumeric &result);

/* numeric conver to string function
 * */
const char* format_int32(int32_t val, char *szText, size_t cbText);
const char* format_int64 (int64_t val, char *szText, size_t cbText);
const char* format_uint32(uint32_t val, char *szText, size_t cbText);
const char* format_uint64(uint64_t val, char *szText, size_t cbText);
#if !((defined WIN32) || (defined WIN64))
const char* _itoa   (int32_t  val, char *szText, size_t cbText);
const char* _i64toa (int64_t  val, char *szText, size_t cbText);
const char* _ultoa  (uint32_t val, char *szText, size_t cbText);
const char* _ui64toa(uint64_t val, char *szText, size_t cbText);

int32_t stricmp(const char *s1, const char *s2);
int32_t strnicmp(const char *s1, const char *s2, size_t n);

const char* _strupr(char *s);
const char* _strlwr(char *s);

#endif // WIN32 || WIN64

/*	return -1 parameter failure or fmt failure
 *		    0 buffer overflow
 *		   >0 copy string size 
 */
int32_t safe_sprintf(char *buffer, size_t cbLen, const char *fmt, ...);

/* 	return -1 parameter failure 
 *  	    0 buffer overflow
 *  	   >0 copy string size 
 */ 
int32_t safe_strcpy(char *buffer, size_t cbLen, const char *source);

/*	return -1 parameter failure
 *			0 buffer overflow
 *		   >0 copy string size	
 */
int32_t safe_strncpy(char *buffer, size_t cbLen, const char *source, size_t cbCount);

/*	return -1 parameter failure
 *			0 buffer overflow
 *		   >0 appended buffer size	
 */
int32_t safe_strcat(char *buffer, size_t cbLen, const char *source);

/* return -1 parameter failure
 * 		   0 buffer overflow
 * 		  >0 appended buffer size
 */
int32_t safe_strcat_ex(char *buffer, size_t cbLen, const char *fmt, ...);

/* return -1 parameter failure
 * 		 >=0 delimit count
 */
int32_t safe_strtok(const char *buffer, std::vector<std::string> &array, const char *delimit=";");
// 解析类似于 "1111;2222;3333;4444"
int32_t safe_strtok(const char *buffer, std::vector<uint32_t> &array, const char *delimit=";"); 
// 解析类似于 "1000:1;2000:2;3000:20"
struct KvItem
{
	uint32_t	key;
	uint32_t	value;
};
int32_t safe_strtok(const char *buffer, std::vector<KvItem> &array, 
		const char *delimit=";", const char *delimitNext=":");
struct KvstrItem
{
	std::string key;
	std::string value;
};
int32_t safe_strtok(const char *buffer, std::vector<KvstrItem> &array,
		const char *delimit="&", const char *delimitNext="=");

int32_t safe_strsplit(const char *buffer, std::vector<std::string> &array, const char *delimit = ";");

// trim right space
const char *rtrim(char *pBuf);
// trim right space and endl
const char* rtail(char *pBuf);
// trim left space
const char* ltrim(char *pBuf);
// trim left and right space
const char* trim(char *pBuf);

// convert a hex-string to binary pointer
void StrToHex(const char *buffer, size_t nLen, byte_t *data, size_t &nDataLen);
// convert binary pointer to hex-string
const char* HexToStr(const byte_t *buffer, size_t nLen, char *data, size_t nDataLen);
// replace source with target, the source and target not require same length
bool StrReplace(char *buffer, size_t nLen, const char *source, const char *target);


/* 解析日期字符串函数,并取出对应日期的元素
   若日期格式中缺失指定的日期元素,则不做任何转换,也不校验日期元素的有效性
        如 fmt 的格式为 "mm-dd HH:MM:SS",则不会取出year的值,也不会对year做任何处理
   参数:  strTime   日期字符串
          fmt       strTime的格式,支持的元素有 yyyy,yy,mm,dd,HH,MM,SS
                    除了日期元素外,所有内容必须与strTime完全一致,否则解析不正确
          year      年,若fmt中只有yy,则会自动+1900
          month     月
          day       日
          hour      时
          minute    分
          second    秒
    返回值 true 解析成功
*/
bool DecodeTimeStr(const char *strTime, int &year, int &month, int &day,
    int &hour, int &minute, int &second, const char *fmt);

// 将str中存在的时间元素替换成当的时间，支持 yyyy mm dd HH MM SS yy
const char* EncodeTimeStr(const char *str, char *szOutput, size_t cbOutput);


void decode_double(double val, int &precision, int &scale);
CMyString double_to_str(double val, int precision);


#ifndef EMPTY_STR
#	define EMPTY_STR(str)		((NULL==(str)) || ('\0'==(str[0])))
#endif

#ifndef SAFE_STR
#	define SAFE_STR(str)		((NULL==(str)) ? "(null)" : (str))
#endif

#ifndef SAFE_STRCPY
#	define	SAFE_STRCPY(dst, src)			safe_strcpy(dst, sizeof(dst), src)
#endif

#ifndef SAFE_SPRINTF
#	define SAFE_SPRINTF(dst, fmt, ...)		safe_sprintf(dst, sizeof(dst), fmt, ## __VA_ARGS__)
#endif


#endif	//_MYLIB_BASECODE_STRINGFUNC_H_

