/*
 * =====================================================================================
 *
 *       Filename:  MyException.h
 *
 *    Description:  exception base on std::exception
 *
 *        Version:  1.0
 *        Created:  2014/11/10 21时27分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYEXCEPTION_H_
#define _MYLIB_BASECODE_MYEXCEPTION_H_

#include <exception>
#include "MyString.h"


class CException : public std::exception
{
public:
	CException(const CException &e) throw();
	CException& operator=(const CException& rhs) throw();
	virtual ~CException() throw();
	virtual const char* what() const throw();

protected:
	CException() throw();
	explicit CException(const char *what) throw();
	explicit CException(const std::string &what) throw();
	explicit CException(int err) throw();

	CMyString what_;
};

////////////////////////////////////////////////////////////////////////////
//datetime exceptions
class Bad_date : public CException
{
public:
	Bad_date() throw();
	explicit Bad_date(time_t tv) throw();
	explicit Bad_date(const char *date_str) throw();
};
class Bad_year : public Bad_date
{
public:
	Bad_year() throw();
	explicit Bad_year(int32_t year) throw();
};
class Bad_month : public Bad_date
{
public:
	Bad_month() throw();
	explicit Bad_month(int32_t mon) throw();
};
class Bad_day : public Bad_date 
{
public:
	Bad_day() throw();
	explicit Bad_day(int32_t day) throw();
};

class Bad_hour : public Bad_date 
{
public:
	Bad_hour() throw();
	explicit Bad_hour(int32_t hour) throw();
};
class Bad_minute : public Bad_date
{
public:	
	Bad_minute() throw();
	explicit Bad_minute(int32_t min) throw();
};

class Bad_second : public Bad_date 
{
public:
	Bad_second() throw();
	explicit Bad_second(int32_t sec) throw();
};

class Bad_usec : public Bad_date
{
public:
	Bad_usec() throw();
	explicit Bad_usec(int32_t usec) throw();
};


////////////////////////////////////////////////////////////////////////////
//Memory File exception
class Bad_MemFile : public CException
{
public:
	Bad_MemFile(const char _Message) throw()
		: CException(_Message)
	{
	}
	Bad_MemFile(int err) throw()
		: CException(err)
	{
	}
};

////////////////////////////////////////////////////////////////////////////
//Share Memory exception
class Bad_ShareMem : public CException
{
public:
	Bad_ShareMem(const char * _Message) throw()
		: CException(_Message)
	{
	}
	Bad_ShareMem(int err) throw()
		: CException(err)
	{
	}
};

////////////////////////////////////////////////////////////////////////////
//lock object exception
class Bad_lockobj : public CException
{
public:
	Bad_lockobj(const char *_Message)
		: CException(_Message) 
	{
	}
	Bad_lockobj(int err)
		: CException(err)
	{
	}
};


////////////////////////////////////////////////////////////////////////////
//MyThread concurrent object exception
class Bad_ConcurrentObj : public CException
{
public:
	Bad_ConcurrentObj(const char *_Message)
		: CException(_Message)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
//
class Bad_Config : public CException
{
public:
	Bad_Config(const char *pszSection, const char *pszKey, int ret) throw();
};

#endif	//_MYLIB_BASECODE_MYEXCEPTION_H_

