/*
 * =====================================================================================
 *
 *       Filename:  MyDateTime.cpp
 *
 *    Description:  wrapper of datetime
 *
 *        Version:  1.0
 *        Created:  11/29/2014 12:23:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include "MyDateTime.h"
#include <stdio.h>
#include <string.h>
#include "StringFunc.h"



CMyDateTime::CMyDateTime()
	: m_time(0)
	, m_year(-1)
	, m_mon(-1)
	, m_day(-1)
	, m_hour(-1)
	, m_min(-1)
	, m_sec(-1)
	, m_usec(0)  
{
	this->now();
}

CMyDateTime::CMyDateTime(int32_t year, int32_t month, int32_t day, 
		int32_t hour, int32_t minute, int32_t second, uint32_t usec)
{
	this->replace(year, month, day, hour, minute, second, usec);
}

CMyDateTime::CMyDateTime(time_t tv, uint32_t usec)
{
	this->replace(tv, usec);
}

CMyDateTime::CMyDateTime(uint64_t nDateTime, uint32_t usec)
{
	this->replace(nDateTime, usec);
}

CMyDateTime::CMyDateTime(const char *datetime_str, const char *fmt)
{
	this->replace(datetime_str, fmt);
}

CMyDateTime::CMyDateTime(const CMyDateTime &rhs)
{
	if (this != &rhs)
	{
		this->m_time = rhs.m_time;
		this->m_usec = rhs.m_usec;
		this->parse();
	}
}

CMyDateTime& CMyDateTime::operator=(const CMyDateTime &rhs)
{
	if (this != &rhs)
	{
		this->m_time = rhs.m_time;
		this->m_usec = rhs.m_usec;
		this->parse();
	}

	return (*this);
}

void CMyDateTime::now()
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	this->m_time = tv.tv_sec;
	this->m_usec = tv.tv_usec;
	this->parse();
}

void CMyDateTime::parse(void)
{
	struct tm tv = {0};
	struct tm *val = localtime_r(&m_time, &tv);
	if (NULL==val)
	{
		THROW_EX(Bad_date((time_t)m_time));
		return;
	}
	m_year = tv.tm_year+1900;
	m_mon  = tv.tm_mon+1;
	m_day = tv.tm_mday;
	m_hour = tv.tm_hour;
	m_min = tv.tm_min;
	m_sec = tv.tm_sec;
	//printf("[%04d-%02d-%02d %02d:%02d:%02d]\n", m_year, m_mon, m_day, m_hour,m_min, m_sec);
}

void CMyDateTime::replace(int32_t year, int32_t month, int32_t day, 
		int32_t hour, int32_t minute, int32_t second, uint32_t usec)
{
	if (year<1970)
	{
		THROW_EX(Bad_year(year));
		return;
	}
	if (month>12 || month<1)
	{
		THROW_EX(Bad_month(month));
		return;
	}
	if (day>31 || day<1)
	{
		THROW_EX(Bad_day(day));
		return;
	}
	if (hour>23 || hour<0)
	{
		THROW_EX(Bad_hour(hour));
		return;
	}
	if (minute>59 || minute<0)
	{
		THROW_EX(Bad_minute(minute));
		return;
	}
	if (second>59 || second<0)
	{
		THROW_EX(Bad_second(second));
		return;
	}
	if (usec>=(uint32_t)1E6)
	{
		THROW_EX(Bad_usec(usec));
		return;
	}

	struct tm tv = { second, minute, hour, day, month-1, year-1900, 0, 0, 0 };
	time_t t1 = mktime(&tv);
	if (t1 == (time_t)-1)
	{
		THROW_EX(Bad_date((time_t)t1));
		return;
	}
	m_time = t1;
	m_usec = usec;
	this->parse();
}

void CMyDateTime::replace(time_t tv, uint32_t usec)
{
	if (tv == (time_t)-1)
	{
		THROW_EX(Bad_date((time_t)tv));
		return;
	}
	if (usec >=(uint32_t)1E6)
	{
		THROW_EX(Bad_usec(usec));
	}

	m_time = tv;
	m_usec = usec;
	this->parse();
}

void CMyDateTime::replace(uint64_t nDateTime, uint32_t usec)
{
	// nDateTime = yyyymmddHHMMSS
	//
	uint32_t year   =  nDateTime/(int32_t)(1E10);
	uint32_t month  = ((uint64_t)(nDateTime/(int32_t)(1E8)))%100;
	uint32_t day    = ((uint64_t)(nDateTime/(int32_t)(1E6)))%100;
	uint32_t hour   = ((uint64_t)(nDateTime/(int32_t)(1E4)))%100;
	uint32_t minute = ((uint64_t)(nDateTime/(int32_t)(1E2)))%100;
	uint32_t second =  nDateTime%100;
	this->replace(year, month, day, hour, minute, second, usec);
}

void CMyDateTime::replace(const char *datetime_str, const char *fmt)
{
	struct tm tv = {0};
	char *pc = strptime(datetime_str, fmt, &tv);
	if (NULL == pc)
	{
		THROW_EX(Bad_date(datetime_str));
		return;
	}
	this->m_time = mktime(&tv);
	this->m_usec = 0;
	this->parse();
}

const char* CMyDateTime::c_str(const char *fmt) const
{
	static char s_buffer[256] = "";

	struct tm tv;
	struct tm *t1 = localtime_r(&m_time, &tv);
	if (t1==NULL)
	{
		return "(null)";
	}
	size_t ret = strftime(s_buffer, sizeof(s_buffer), fmt, t1);
	//printf("1>c_str() ==> [%s]\n", s_buffer);
	if (0==ret)
		return "";
	if ( strcasestr(s_buffer, "%Q")!=NULL )
	{
		char szText[128] = "";
		_ultoa(m_usec/1000, szText, sizeof(szText));
		StrReplace(s_buffer, sizeof(s_buffer), "%Q", szText);

		_ultoa(m_usec, szText, sizeof(szText));
		StrReplace(s_buffer, sizeof(s_buffer), "%q", szText);

		//printf("2>c_str() ==> [%s]\n", s_buffer);
		return s_buffer;
	}
	return s_buffer;
}

int16_t CMyDateTime::dayofyear(void) const
{
	struct tm tv = {0};
	struct tm *time1 = localtime_r(&m_time, &tv);
	if (NULL == time1)
	{
		return -1;
	}
	return tv.tm_yday + 1;
}

int16_t CMyDateTime::weekday(void) const
{
	struct tm tv = {0};
	struct tm *time1 = localtime_r(&m_time, &tv);
    if (NULL == time1)
    {
        return -1;
    }
    return tv.tm_wday;
}

CMyDateTime& CMyDateTime::operator+=(const int32_t nDays)
{
	return this->operator+=(CMyTimeDelta(nDays*24*3600));
}

CMyDateTime& CMyDateTime::operator-=(const int32_t nDays)
{
	return this->operator-=(CMyTimeDelta(nDays*24*3600));
}

CMyDateTime& CMyDateTime::operator+=(const CMyTimeDelta &delta)
{
	this->m_time += delta.m_second;
	this->m_usec += delta.m_usec;
	if (this->m_time <= 0)
	{
		THROW_EX(Bad_date());
	}
	if (m_usec < 0)
	{
		m_usec = (int32_t)1E6 + m_usec;
		--m_time;
	}

	return (*this);
}

CMyDateTime& CMyDateTime::operator-=(const CMyTimeDelta &delta)
{
	this->m_time -= delta.m_second;
	this->m_usec -= delta.m_usec;
	if (this->m_time <= 0)
	{
		THROW_EX(Bad_date());
	}
	if (m_usec < 0)
	{
		m_usec = (int32_t)1E6 + m_usec;
		--m_time;
	}

	return (*this);
}

////////////////////////////////////////////////////////////////////////////
//inline function

CMyTimeDelta operator-(const CMyDateTime &lhs, const CMyDateTime &rhs)
{
	time_t  t1   = lhs.time() - rhs.time();
	int64_t usec = lhs.usecond() - rhs.usecond();
	if (t1>0 && usec<0)
	{
		--t1;
		usec = (int32_t)1E6 + usec;
	}
	else if (t1<0 && usec>0)
	{
		++t1;
		usec -= (int32_t)1E6;
	}
	return CMyTimeDelta(t1, usec);
}

CMyDateTime  operator-(const CMyDateTime &lhs, const int32_t nDays)
{
	CMyDateTime tmp(lhs);
	tmp-=nDays;
	return tmp;
}

CMyDateTime  operator+(const CMyDateTime &lhs, const int32_t nDays)
{
	CMyDateTime tmp(lhs);
	tmp += nDays;
	return tmp;
}

CMyDateTime  operator-(const CMyDateTime &lhs, const CMyTimeDelta &rhs)
{
	CMyDateTime tmp(lhs);
	tmp -= rhs;
	return tmp;
}

CMyDateTime  operator+(const CMyDateTime &lhs, const CMyTimeDelta &rhs)
{
	CMyDateTime tmp(lhs);
	tmp += rhs;
	return tmp;
}

