/*
 * =====================================================================================
 *
 *       Filename:  MyTime.h
 *
 *    Description:  time wrapper
 *
 *        Version:  1.0
 *        Created:  2014/11/28 21时31分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYTIMEDELTA_H_
#define _MYLIB_BASECODE_MYTIMEDELTA_H_

#include <iostream>
#include "myconf.h"
#include "mydef.h"
#include "IException.h"

class CMyDateTime;
class CMyTimeDelta
{
	friend class CMyDateTime;
public:
	CMyTimeDelta();
	//dwTime ==> HHMMSS
	//dwTime ==> ddHHMMSS
	explicit CMyTimeDelta(int32_t second, int32_t usec=0);
	CMyTimeDelta(uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t usec=0);
	explicit CMyTimeDelta(const char *time_str, const char *fmt="%H:%M:%S");
	CMyTimeDelta(const CMyTimeDelta &rhs);
	CMyTimeDelta& operator=(const CMyTimeDelta &rhs);

public:
	void replace(int32_t second, int32_t usec=0);
	void replace(uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t usec=0);
	void replace(const char *time_str, const char *fmt="%H:%M:%S");

	int64_t seconds(void) 		const { return m_second; }
	int64_t microseconds(void)  const { return m_second*E6+m_usec; }
	int64_t minutes(void)		const { return m_second/60; }
	int64_t hours(void)			const { return m_second/3600; }
	int32_t usec(void)			const { return m_usec; }

	// 支持部份格式输出
	// %d 天数
	// %H 小时
	// %M 分钟
	// %S 秒
	// %Q 毫秒
	// %q 微秒
	const char* c_str(const char *fmt = "%H:%M:%S") const;

    CMyTimeDelta& operator+=(const int32_t nSeconds);
    CMyTimeDelta& operator+=(const CMyTimeDelta &rhs);
    CMyTimeDelta& operator-=(const int32_t nSeconds);
    CMyTimeDelta& operator-=(const CMyTimeDelta &rhs);

	
private:
	int64_t	m_second;		// total seconds
	int32_t	m_usec;			// addition microseconds
};


///////////////////////////////////////////////////////////////////////////////////////////
//inline function
inline std::ostream& operator<<(std::ostream& os, const CMyTimeDelta& rhs)
{
	return os << rhs.c_str("%d days %H:%M:%S.%Q");
}

inline bool operator==(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
	return ((lhs.seconds()==rhs.seconds()) && (lhs.usec()==rhs.usec()));
}

inline bool operator!=(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
    return (!(rhs==rhs));
}

inline bool operator<(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
	return ((lhs.seconds()<rhs.seconds()) || 
		    ((lhs.seconds()==rhs.seconds()) && (lhs.usec()<rhs.usec()))
		   );	
}

inline bool operator>(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
	return ((lhs.seconds()>rhs.seconds()) || 
		    ((lhs.seconds()==rhs.seconds()) && (lhs.usec()>rhs.usec()))
		   );	
}

inline bool operator<=(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
    return (!(rhs>rhs));
}

inline bool operator>=(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
    return (!(rhs<rhs));
}


CMyTimeDelta operator-(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs);
CMyTimeDelta operator-(const CMyTimeDelta &lhs, const int32_t nSeconds);
CMyTimeDelta operator+(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs);
CMyTimeDelta operator+(const CMyTimeDelta &lhs, const int32_t nSeconds);

#endif // _MYLIB_BASECODE_MYTIMEDELTA_H_


