/*
 * =====================================================================================
 *
 *       Filename:  MyException.cpp
 *
 *    Description:  implement of IException
 *
 *        Version:  1.0
 *        Created:  2014年12月04日 14时59分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include <errno.h>
#include <string.h>
#include "IException.h"
#include "StringFunc.h"
#include "MyString.h"


////////////////////////////////////////////////////////////////////////////
//CException
CException::CException(const CException &e) throw() 
	: std::exception(e)
	, what_(e.what_)
{
}

CException& CException::operator=(const CException& rhs) throw()
{
	what_ = rhs.what_;
	return *this;
}

CException::~CException() throw()
{
}

const char* CException::what() const throw()
{
	return what_.c_str();
}

CException::CException(const char *what) throw()
	: std::exception()
	, what_(what)
{
}

CException::CException() throw()
	: std::exception()
	, what_()
{
	int err = errno;
	if (err != 0)
	{
		what_.Format("[%d] %s", err, strerror(err));
	}
}

CException::CException(const std::string &what) throw()
	: std::exception()
	, what_(what)
{
}

CException::CException(int err) throw()
	: std::exception()
	, what_()  
{
	what_.Format("[%d] %s", err, strerror(err));
}

////////////////////////////////////////////////////////////////////////////
//Bad_date
Bad_date::Bad_date() throw()
	: CException("")
{
}

Bad_date::Bad_date(time_t tv) throw()
	: CException("")
{
	what_.Format("the time_t value [%ld] invalid", tv);
}

Bad_date::Bad_date(const char *date_str) throw()
	: CException("")
{
	if (NULL == date_str)
		what_.Format("datetime string can not be NULL.");
	else
		what_.Format("bad datetime string:[%s]", date_str);
}

////////////////////////////////////////////////////////////////////////////
//Bad_year 
Bad_year::Bad_year() throw() 
	: Bad_date()
{
	what_ = "invalid year value";
}

Bad_year::Bad_year(int32_t year) throw() 
	: Bad_date()
{
	what_.Format("bad year value:[%d]", year); 
}

////////////////////////////////////////////////////////////////////////////
//Bad_month 
Bad_month::Bad_month() throw() 
	: Bad_date()
{
	what_ = "invalid month value";
}

Bad_month::Bad_month(int32_t mon) throw() 
	: Bad_date()
{
	what_.Format("bad month value:[%d]", mon);
}


////////////////////////////////////////////////////////////////////////////
//Bad_day
Bad_day::Bad_day() throw() 
	: Bad_date()
{
	what_ = "invalid day value";
}

Bad_day::Bad_day(int32_t day) throw() 
	: Bad_date()
{
	what_.Format("bad day value:[%d], maybe datetime invalid", day);
}

////////////////////////////////////////////////////////////////////////////
//Bad_hour
Bad_hour::Bad_hour() throw() 
	: Bad_date()
{
	what_ = "invalid hour value";
}

Bad_hour::Bad_hour(int32_t hour) throw() 
	: Bad_date()
{
	what_.Format("bad hour value:[%d]", hour);
}


////////////////////////////////////////////////////////////////////////////
//Bad_minute
Bad_minute::Bad_minute() throw() 
	: Bad_date()
{
	what_ = "invalid minute value";
}

Bad_minute::Bad_minute(int32_t minute) throw() 
	: Bad_date()
{
	what_.Format("bad minute value:[%d]", minute);
}

////////////////////////////////////////////////////////////////////////////
//Bad_second
Bad_second::Bad_second() throw() 
	: Bad_date()
{
	what_ = "invalid second value";
}

Bad_second::Bad_second(int32_t second) throw() 
	: Bad_date()
{
	what_.Format("bad second value:[%d]", second);
}

////////////////////////////////////////////////////////////////////////////
//Bad_usec
Bad_usec::Bad_usec() throw() 
	: Bad_date()
{
	what_ = "invalid microsecond value";
}

Bad_usec::Bad_usec(int32_t usec) throw() 
	: Bad_date()
{
	what_.Format("bad microsecond value:[%d]", usec);
}

////////////////////////////////////////////////////////////////////////////
Bad_Config::Bad_Config(const char *pszSection, const char *pszKey, int32_t ret) throw()
	: CException("")
{
	CMyString strNote("invalid config, section[%s], key[%s], result=%d",
			(pszSection==NULL ? "N/A" : pszSection),
			(pszKey==NULL ? "N/A" : pszKey), ret );
	what_ = strNote.c_str();
}

