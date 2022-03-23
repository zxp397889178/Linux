/*
 * =====================================================================================
 *
 *       Filename:  MyDate.h
 *
 *    Description:  date wrapper
 *
 *        Version:  1.0
 *        Created:  2014年11月28日 17时40分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYDATE_H_
#define _MYLIB_BASECODE_MYDATE_H_

#include <iostream>
#include "myconf.h"
#include "IException.h"


//class Bad_date
//class Bad_year
//class Bad_month
//class Bad_day
class CMyDate
{
public:
    CMyDate();  //当前日期
    CMyDate(int32_t year, int32_t month, int32_t day);
    explicit CMyDate(const char *date_str, const char *fmt="%Y.%m.%d");
    explicit CMyDate(uint32_t nDate);           //yyyymmdd 格式
    explicit CMyDate(time_t tTime);
    CMyDate(const CMyDate &rhs);
    CMyDate& operator=(const CMyDate &rhs);

	// 设置指定的日期
	void today();
	void replace(int32_t year, int32_t month, int32_t day);
	void replace(const char *date_str, const char *fmt="%Y.%m.%d");
	void replace(uint32_t nDate);
	void replace(time_t tTime);

    int32_t year() const    { return m_nYear; }
    int32_t month() const   { return m_nMonth; }
    int32_t day() const     { return m_nDay; }
	int32_t dayofyear() const;
    int32_t weekday() const;
	const char* c_str(const char *fmt = "%Y.%m.%d");

    time_t time(void) const;

    //获取当前日期是本年度的第几天
    int32_t GetPassdayOfYear() const;

    CMyDate& operator+=(const int32_t nDays);
    CMyDate& operator-=(const int32_t nDays);

private:
    int32_t     m_nYear;
    int32_t     m_nMonth;
    int32_t     m_nDay;
};


////////////////////////////////////////////////////////////////////////////////////////
//inline functions

inline std::ostream& operator<<(std::ostream& os, const CMyDate& rhs)
{
    return (os << rhs.year() << "-" << rhs.month() << "-" << rhs.day());
}

inline bool operator==(const CMyDate& lhs, const CMyDate& rhs)
{
    return (lhs.year()==rhs.year() && lhs.month()==rhs.month() && lhs.day()== rhs.day() );
}

inline bool operator!=(const CMyDate& lhs, const CMyDate& rhs)
{
    return !(lhs==rhs);
}

inline bool operator<(const CMyDate& lhs, const CMyDate& rhs)
{
    // compare year
    if (lhs.year() < rhs.year())
    {
        return true;
    }
    else if (lhs.year() == rhs.year())
    {
        // compare month
        if (lhs.month() < rhs.month())
        {
            return true;
        }
        else if (lhs.month() == rhs.month())
        {
            // compare day
            if (lhs.day() < rhs.day())
                return true;
            else
                return false;
        }
        return false;
    }
    return false;
}

inline bool operator>(const CMyDate& lhs, const CMyDate& rhs)
{
    // compare year
    if (lhs.year() > rhs.year())
    {
        return true;
    }
    else if (lhs.year() == rhs.year())
    {
        // compare month
        if (lhs.month() > rhs.month())
        {
            return true;
        }
        else if (lhs.month() == rhs.month())
        {
            // compare day
            if (lhs.day() > rhs.day())
                return true;
            else
                return false;
        }
        return false;
    }
    return false;
}

inline bool operator<=(const CMyDate& lhs, const CMyDate& rhs)
{
    return (!(lhs>rhs));
}

inline bool operator>=(const CMyDate& lhs, const CMyDate& rhs)
{
    return (!(lhs<rhs));
}

int32_t operator-(const CMyDate& lhs, const CMyDate& rhs);
CMyDate operator-(const CMyDate &lhs, const int32_t nDays);
CMyDate operator+(const CMyDate &lhs, const int32_t nDays);

//////////////////////////////////////////////////////////////////////////////////////////
//for namespace std
/*
namespace std {

inline bool operator==(const CMyDate& lhs, const CMyDate& rhs)
{
    return (lhs.year()==rhs.year() && lhs.month()==rhs.month() && lhs.day()== rhs.day() );
}

inline bool operator!=(const CMyDate& lhs, const CMyDate& rhs)
{
    return !(lhs==rhs);
}

inline bool operator<(const CMyDate& lhs, const CMyDate& rhs)
{
    // compare year
    if (lhs.year() < rhs.year())
    {
        return true;
    }
    else if (lhs.year() == rhs.year())
    {
        // compare month
        if (lhs.month() < rhs.month())
        {
            return true;
        }
        else if (lhs.month() == rhs.month())
        {
            // compare day
            if (lhs.day() < rhs.day())
                return true;
            else
                return false;
        }
        return false;
    }
    return false;
}

inline bool operator>(const CMyDate& lhs, const CMyDate& rhs)
{
    // compare year
    if (lhs.year() > rhs.year())
    {
        return true;
    }
    else if (lhs.year() == rhs.year())
    {
        // compare month
        if (lhs.month() > rhs.month())
        {
            return true;
        }
        else if (lhs.month() == rhs.month())
        {
            // compare day
            if (lhs.day() > rhs.day())
                return true;
            else
                return false;
        }
        return false;
    }
    return false;
}

inline bool operator<=(const CMyDate& lhs, const CMyDate& rhs)
{
    return (!(lhs>rhs));
}

inline bool operator>=(const CMyDate& lhs, const CMyDate& rhs)
{
    return (!(lhs<rhs));
}

} // namespace std

*/

#endif	// _MYLIB_BASECODE_MYDATE_H_

