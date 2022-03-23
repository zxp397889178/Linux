/*
 * =====================================================================================
 *
 *       Filename:  MyDate.cpp
 *
 *    Description:  date wrapper
 *
 *        Version:  1.0
 *        Created:  2014年11月28日 17时41分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include "MyDate.h"
#include <algorithm>
#include <sys/time.h>



//判断是否为闰年
inline bool IsLeapYear(int32_t year)
{
    return ((year % 100==0) ? (year % 400==0) : (year % 4==0));
}

//获取当年的总天数
inline int32_t GetDaysOfYear(int32_t year)
{
    return (365+IsLeapYear(year));
}

//获取本年、本月的总天数
inline int32_t GetDaysOfMonth(int32_t month, int32_t year)
{
    switch(month) {
    case 1: 
    case 3: 
    case 5: 
    case 7: 
    case 8: 
    case 10:
    case 12:
        return 31;
    case 4: 
    case 6: 
    case 9: 
    case 11: 
        return 30;
    case 2: 
        return 28 + IsLeapYear(year);
    default:
        break;
    }
    THROW_EX(Bad_month());
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//CMyDate
CMyDate::CMyDate()
{
	this->today();
}

CMyDate::CMyDate(int32_t year, int32_t month, int32_t day)
{
	this->replace(year, month, day);
}

CMyDate::CMyDate(time_t tTime)
{
	this->replace(tTime);
}

CMyDate::CMyDate(uint32_t nDate)
{
	this->replace(nDate);
}

CMyDate::CMyDate(const char *date_str, const char *fmt)
{
	this->replace(date_str, fmt);
}

CMyDate::CMyDate(const CMyDate &rhs)
    : m_nYear(rhs.m_nYear)
    , m_nMonth(rhs.m_nMonth)
    , m_nDay(rhs.m_nDay)
{
}

CMyDate& CMyDate::operator=(const CMyDate &rhs)
{
    if (this == &rhs)
    {
        return (*this);
    }
    this->m_nYear = rhs.year();
    this->m_nMonth = rhs.month();
    this->m_nDay = rhs.day();
    return (*this);
}

void CMyDate::today()
{
    time_t t = ::time(NULL);
	struct tm tmNow;
	struct tm *tv = localtime_r(&t, &tmNow);
	if (NULL==tv)
    {
        THROW_EX(Bad_date());
		return;
    }
    this->m_nYear = tv->tm_year+1900;
    this->m_nMonth = tv->tm_mon+1;
    this->m_nDay = tv->tm_mday;
}

void CMyDate::replace(int32_t year, int32_t month, int32_t day)
{
    if (year <= 0)
	{
        THROW_EX(Bad_year());
		return;
	}
    if (month < 1 || month > 12)
	{
        THROW_EX(Bad_month());
		return;
	}
    if (day < 1 || GetDaysOfMonth(month,year) < day)
	{
        THROW_EX(Bad_day());
		return;
	}

	this->m_nYear = year;
	this->m_nMonth = month;
	this->m_nDay = day;
}

void CMyDate::replace(const char *date_str, const char *fmt)
{
	struct tm tv = {0};
	strptime(date_str, fmt, &tv);

	this->m_nYear = tv.tm_year + 1900;
	this->m_nMonth = tv.tm_mon + 1;
	this->m_nDay = tv.tm_mday;
}

void CMyDate::replace(uint32_t nDate)
{
    this->m_nYear = nDate/10000;
    this->m_nMonth = ((nDate/100) % 100);
    this->m_nDay = (nDate % 100);
    if (m_nYear <= 0)
	{
        THROW_EX(Bad_year());
		return;
	}
    if (m_nMonth < 1 || m_nMonth > 12)
	{
        THROW_EX(Bad_month());
		return;
	}
    if (m_nDay < 1 || GetDaysOfMonth(m_nMonth,m_nDay) < m_nDay)
	{
        THROW_EX(Bad_day());
		return;
	}
}

void CMyDate::replace(time_t tTime)
{
	struct tm tmValue;
	struct tm *tmTime = localtime_r(&tTime, &tmValue);
	if (NULL==tmTime)
    {
        THROW_EX(Bad_date());
		return;
    }
    this->m_nYear = (tmTime->tm_year+1900);
    this->m_nMonth = (tmTime->tm_mon + 1);
    this->m_nDay = tmTime->tm_mday;
}

int32_t CMyDate::GetPassdayOfYear() const
{
    int32_t nPass = 0;
    for(int32_t i=1; i != m_nMonth; ++i)
    {
        nPass += GetDaysOfMonth(i, m_nYear);
    }
    return (nPass + m_nDay);
}

CMyDate& CMyDate::operator+=(const int32_t nDays)
{
    //检查天数值的合法性
    if (nDays<0)
    {
        return this->operator-=(abs(nDays));
    }
    else if (0 == nDays)
    {
        return *this;
    }

    //判断日期是否在一个月以内
    m_nDay += nDays;
    int32_t nDaysOfMonth = GetDaysOfMonth(m_nMonth, m_nYear);
    if (m_nDay <= nDaysOfMonth)
    {
        return *this;
    }

    //循环处理日期，直至一个合法的时间
    do{
        m_nDay -= nDaysOfMonth;
        ++m_nMonth;
        if (m_nMonth>12)
        {
            ++m_nYear;
            m_nMonth = 1;
        }
        nDaysOfMonth = GetDaysOfMonth(m_nMonth, m_nYear);
    }while(m_nDay > nDaysOfMonth);

    return *this;
}

CMyDate& CMyDate::operator-=(const int32_t nDays)
{
    //检查天数的合法性
    if (nDays < 0)
    {
        return this->operator+=(abs(nDays));
    }
    else if (0 == nDays)
    {
        return *this;
    }

    //判断日期是否在一个月以内
    m_nDay -= nDays;
    if (m_nDay>0)
    {
        return *this;
    }

    //循环处理日期，直至一个合法的时间
    int32_t nDaysOfMonth(0);
    do{
        --m_nMonth;
        if (m_nMonth<=0)
        {
            --m_nYear;
            m_nMonth = 12;
        }
        nDaysOfMonth = GetDaysOfMonth(m_nMonth, m_nYear);
        m_nDay += nDaysOfMonth;
    }while(m_nDay<=0);

    return *this;
}

time_t CMyDate::time(void) const
{
	struct tm tv = { 0, 0, 0, m_nDay, m_nMonth-1, m_nYear-1900, 0, 0, 0};
	return mktime(&tv);
}

int32_t CMyDate::weekday(void) const
{
	struct tm tmValue;
    time_t tv = this->time();
	struct tm *time1 = localtime_r(&tv, &tmValue);
    if (NULL == time1)
    {
        return -1;
    }
    return time1->tm_wday;
}

int32_t CMyDate::dayofyear(void) const
{
	struct tm tmValue;
	time_t tv = this->time();
	struct tm *time1 = localtime_r(&tv, &tmValue);
	if (NULL == time1)
	{
		return -1;
	}
	return time1->tm_yday + 1;
}

const char* CMyDate::c_str(const char *fmt)
{
	static char s_buffer[256] = "";

	time_t tv = this->time();
	struct tm tmValue;
	struct tm *t1 = localtime_r(&tv, &tmValue);
	if (NULL==t1)
		return "";
	size_t ret = strftime(s_buffer, sizeof(s_buffer), fmt, t1);
	if (0==ret)
		return "";
	return s_buffer;
}


//////////////////////////////////////////////////////////////////////////
//通用函数

int32_t operator-(const CMyDate& lhs, const CMyDate& rhs)
{
    int32_t nDistance = 0;
    const int32_t lowyear  = std::min(lhs.year(), rhs.year());
    const int32_t highyear = std::max(lhs.year(), rhs.year());
    for (int32_t yn = lowyear; yn != highyear; ++yn)
    {
        nDistance += GetDaysOfYear(yn);
    }

    CMyDate dateMin = std::min(lhs, rhs);
    CMyDate dateMax = std::max(lhs, rhs);
    int32_t val = nDistance - dateMin.GetPassdayOfYear() + dateMax.GetPassdayOfYear();
    return val;
}

CMyDate operator-(const CMyDate &lhs, const int32_t nDays)
{
    CMyDate tmp(lhs);
    tmp -= nDays;
    return tmp;
}

CMyDate operator+(const CMyDate &lhs, const int32_t nDays)
{
    CMyDate tmp(lhs);
    tmp += nDays;
    return tmp;
}

