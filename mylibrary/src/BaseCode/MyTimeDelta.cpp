/*
 * =====================================================================================
 *
 *       Filename:  MyTime.cpp
 *
 *    Description:  wrapper of time
 *
 *        Version:  1.0
 *        Created:  2014/11/28 23时08分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include <algorithm>
#include <sys/time.h>
#include "MyTimeDelta.h"
#include "MyString.h"
#include "StringFunc.h"



//////////////////////////////////////////////////////////////////////////
//CMyTimeDelta
CMyTimeDelta::CMyTimeDelta()
	: m_second(0)
	, m_usec(0)
{
}

CMyTimeDelta::CMyTimeDelta(uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t usec)
	: m_second(0)
    , m_usec(usec)
{
	this->replace(day, hour, minute, second, usec);
}

CMyTimeDelta::CMyTimeDelta(int32_t second, int32_t usec)
{
	this->replace(second, usec);
}

CMyTimeDelta::CMyTimeDelta(const char *time_str, const char *fmt)
{
	this->replace(time_str, fmt);
}

CMyTimeDelta::CMyTimeDelta(const CMyTimeDelta &rhs)
    : m_second(rhs.m_second)
	, m_usec(rhs.m_usec)
{
}

CMyTimeDelta& CMyTimeDelta::operator=(const CMyTimeDelta &rhs)
{
    if (this == &rhs)
    {
        return (*this);
    }
	this->m_second = rhs.m_second;
    this->m_usec = rhs.m_usec;
    return (*this);
}

void CMyTimeDelta::replace(int32_t second, int32_t usec)
{
	m_second = second;
	m_usec = usec;
}

void CMyTimeDelta::replace(uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t usec)
{
    if (hour >=24)
	{
        THROW_EX(Bad_hour(hour));
		return;
	}
    if (minute >= 60)
	{
        THROW_EX(Bad_minute(minute));
		return;
	}
    if (second >= 60)
	{
        THROW_EX(Bad_second(second));
		return;
	}

	m_second = (uint64_t)(((day*24+hour)*60+minute)*60+second);
	m_usec = usec;
}

void CMyTimeDelta::replace(const char *time_str, const char *fmt)
{
	struct tm tv = {0};
	char *pc = strptime(time_str, fmt, &tv);
	if (NULL==pc)
	{
		THROW_EX(Bad_date());
		return;
	}
	this->replace(tv.tm_mday, tv.tm_min, tv.tm_sec, 0);
}

CMyTimeDelta& CMyTimeDelta::operator+=(const int32_t nSeconds)
{
    //检查秒数值的合法性
    if (nSeconds<0)
    {
        return this->operator-=(abs(nSeconds));
    }
    else if (0 == nSeconds)
    {
        return *this;
    }

	int64_t val = this->microseconds() + nSeconds*E6; //1E6
	m_second = val/E6;  //1E6
	m_usec = val%E6; // 1E6

	return (*this);
}

CMyTimeDelta& CMyTimeDelta::operator+=(const CMyTimeDelta &rhs)
{
	int64_t val = this->microseconds() + rhs.microseconds();
	m_second = val/1000000;
	m_usec = val%1000000;

	return (*this);
}


CMyTimeDelta& CMyTimeDelta::operator-=(const int32_t nSeconds)
{
    //检查天数的合法性
    if (nSeconds < 0)
    {
        return this->operator+=(abs(nSeconds));
    }
    else if (0 == nSeconds)
    {
        return *this;
    }

	int64_t val = this->microseconds() - nSeconds*E6;
	m_second = val/E6;
	m_usec = val%E6;

	return (*this);
}

CMyTimeDelta& CMyTimeDelta::operator-=(const CMyTimeDelta &rhs)
{
	int64_t val = this->microseconds() - rhs.microseconds();
	m_second = val/E6;
	m_usec = val%E6;

	return (*this);
}


const char* CMyTimeDelta::c_str(const char *fmt) const
{
	static CMyString s_strTime;

	bool bNegative(false);
	if (m_second<0 || m_usec<0)
	{
		bNegative = true;
	}

	int32_t days = abs(m_second/(24*3600));
	int32_t time = abs(m_second - days*24*3600);
	int16_t hour = abs(time/3600);
	int16_t minute = abs((time-hour*3600)/60);
	//int16_t second = abs(time-hour*3600-minute*60);
	int16_t second = abs(time%60);

	s_strTime=fmt;
	char szText[128] = "";
	s_strTime.Replace("%d", _itoa(days, szText, sizeof(szText)) );
	safe_sprintf(szText, sizeof(szText), "%02d", hour);
	s_strTime.Replace("%H", szText);
	safe_sprintf(szText, sizeof(szText), "%02d", minute);
	s_strTime.Replace("%M", szText);
	safe_sprintf(szText, sizeof(szText), "%02d", second);
	s_strTime.Replace("%S", szText);
	s_strTime.Replace("%Q", _itoa(abs(m_usec/1000), szText, sizeof(szText)) );	// milliseconds
	s_strTime.Replace("%q", _itoa(abs(m_usec), szText, sizeof(szText)) );		// microseconds
	if (bNegative)
	{
		s_strTime.insert(0, "-");
	}

	return s_strTime.c_str();
}


//////////////////////////////////////////////////////////////////////////
//通用函数
CMyTimeDelta operator+(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{

	int64_t sec(0);
	int32_t usec(0);
	sec = lhs.seconds() + rhs.seconds();
	usec = lhs.usec() + rhs.usec();
	if (sec>0 && usec<0)
	{
		--sec;
		usec = E6 + usec;
	}
	else if (sec<0 && usec>0)
	{
		++sec;
		usec -= E6;
	}

	return CMyTimeDelta((int32_t)sec, (int32_t)usec);
}

CMyTimeDelta operator-(const CMyTimeDelta& lhs, const CMyTimeDelta& rhs)
{
	int64_t sec(0);
	int32_t usec(0);
	sec = lhs.seconds() - rhs.seconds();
	usec = lhs.usec() - rhs.usec();
	if (sec>0 && usec<0)
	{
		--sec;
		usec = E6 + usec;
	}
	else if (sec<0 && usec>0)
	{
		++sec;
		usec -= E6;
	}

	return CMyTimeDelta((int32_t)sec, (int32_t)usec);
}

CMyTimeDelta operator+(const CMyTimeDelta& lhs, const int32_t nSeconds)
{
	CMyTimeDelta rhs(nSeconds, 0);
	return (lhs+rhs);
}

CMyTimeDelta operator-(const CMyTimeDelta &lhs, const int32_t nSeconds)
{
	CMyTimeDelta rhs(nSeconds, 0);
	return (lhs-rhs);
}



