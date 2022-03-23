/*
 * =====================================================================================
 *
 *       Filename:  MyDateTime.h
 *
 *    Description:  wrapper of datetime
 *
 *        Version:  1.0
 *        Created:  11/29/2014 12:23:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MY_DATETIME_H_
#define _MYLIB_BASECODE_MY_DATETIME_H_

#include <iostream>
#include <sys/time.h>
#include "myconf.h"
#include "MyDate.h"
#include "MyTimeDelta.h"

class CMyDateTime
{
public:
	CMyDateTime();
	CMyDateTime(int32_t year, int32_t month, int32_t day, 
			int32_t hour, int32_t minute, int32_t second,
			uint32_t usec = 0);
	explicit CMyDateTime(time_t tv, uint32_t usec = 0);
	// nDateTime=20041202112302
	explicit CMyDateTime(uint64_t nDateTime, uint32_t usec=0);
	// datetime_str="2004-12-02 11:10:12"
	explicit CMyDateTime(const char *datetime_str, const char *fmt="%Y-%m-%d %H:%M:%S");
	CMyDateTime(const CMyDateTime &rhs);
	CMyDateTime& operator=(const CMyDateTime &rhs);

public:
	void now();
	void replace(int32_t year, int32_t month, int32_t day,
			int32_t hour, int32_t minute, int32_t second,
			uint32_t usec = 0);
	void replace(time_t tv, uint32_t usec=0);
	void replace(uint64_t nDateTime, uint32_t usec=0);
	void replace(const char *datetime_str, const char *fmt="%Y-%m-%d %H:%M:%S");

	int16_t year(void) const 	{ return m_year; }
	int16_t month(void) const 	{ return m_mon; }
	int16_t day(void) const 	{ return m_day; }
	int16_t hour(void) const 	{ return m_hour; }
	int16_t minute(void) const 	{ return m_min; }
	int16_t second(void) const 	{ return m_sec; }
	time_t   time(void) 		   const { return m_time; }
	uint32_t usecond(void) 		   const { return m_usec; }
	CMyDate Date(void) 			   const { return CMyDate(m_time); }
	uint64_t GetMilliSeconds(void) const { return m_time*E3+m_usec/E3; }
	uint64_t GetMicroSeconds(void) const { return m_time*E6+m_usec; }

	const char* c_str(const char *fmt = "%Y.%m.%d %H:%M:%S") const;
	int16_t  dayofyear() const;
	int16_t	 weekday() const;
	
	CMyDateTime& operator+=(const int32_t nDays);
	CMyDateTime& operator-=(const int32_t nDays);
	CMyDateTime& operator+=(const CMyTimeDelta &delta);
	CMyDateTime& operator-=(const CMyTimeDelta &delta);

private:
	void parse();

private:
	time_t		m_time;

	int16_t		m_year;
	int8_t		m_mon;
	int8_t		m_day;
	int8_t		m_hour;
	int8_t		m_min;
	int8_t		m_sec;
	int32_t		m_usec;		//microseconds
};



/////////////////////////////////////////////////////////////////////////////////////////////
//inline functions

inline bool operator==(const CMyDateTime &lhs, const CMyDateTime &rhs)                                                     
{
    return (lhs.time()==rhs.time() && lhs.usecond()==rhs.usecond());
}

inline bool operator!=(const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	return (!(lhs==rhs));
}

inline bool operator< (const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	return (lhs.time()<rhs.time()) || 
		   ((lhs.time()==rhs.time()) && (lhs.usecond()<rhs.usecond()));
}

inline bool operator> (const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	return (lhs.time()>rhs.time()) || 
		   ((lhs.time()==rhs.time()) && (lhs.usecond()>rhs.usecond()));
}

inline bool operator<=(const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	return (!(lhs>rhs));
}

inline bool operator>=(const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	return (!(lhs<rhs));
}

inline std::ostream& operator<<(std::ostream& os, const CMyDateTime& rhs)
{
	return os << rhs.c_str("%Y.%m.%d %H:%M:%S.%Q");
}

CMyTimeDelta operator-(const CMyDateTime &lhs, const CMyDateTime &rhs);
CMyDateTime  operator-(const CMyDateTime &lhs, const int32_t nDays);
CMyDateTime  operator+(const CMyDateTime &lhs, const int32_t nDays);
CMyDateTime  operator-(const CMyDateTime &lhs, const CMyTimeDelta &rhs);
CMyDateTime  operator+(const CMyDateTime &lhs, const CMyTimeDelta &rhs);


#endif // _MYLIB_BASECODE_MY_DATETIME_H_

