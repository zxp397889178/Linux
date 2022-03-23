/*
 * =====================================================================================
 *
 *       Filename:  TimeFunc.h
 *
 *    Description:  some function of time
 *
 *        Version:  1.0
 *        Created:  2014/11/17 22时47分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_TIME_FUNCTION_H_
#define _MYLIB_BASECODE_TIME_FUNCTION_H_

#include "myconf.h"

// 获得从 00:00:00 开始经过的时间，
// bReturnMilliSecond=true, 返回毫秒数，否则返回秒数
uint32_t GetTimepassOfToday(bool bReturnMilliSecond);

// 获得今天00:00:00 时的毫秒值
// 返回值 /1000 => time_t
uint64_t GetBeginOfToday();

// 获得time当天00:00:00 时的毫秒值
time_t GetDayBeginOfTime(time_t v);

// 获得今天经过了seconds之后的毫秒值
// 返回值 /1000 => time_t
uint64_t GetTimeOfToday(uint32_t secondsOfToday);

// return the millisecond or microsecond of now
uint64_t Now(bool bReturnMicroSecond = false);

std::string getGMTDate(const time_t &t1);

// return -1 failure
class ICondition;
int32_t PthreadSleep(uint32_t dwTimeout, ICondition *&pCond);

// dwTimeout ==> ms
void nsleep(uint64_t dwTimeout);
// dwTimeout ==> us (1/1000 ms)
void nsleep_ex(uint64_t dwTimeout);

/*
//用于统计某个函数或方法经过的时间
   long nTimeCost(0);
   TIME_BEGIN(nTimeCost);
   ...
   TIME_END
   此时获得nTimeCost就是经过的毫秒数
*/
class CTimeStat
{
public:
    explicit CTimeStat(int32_t &nValueOut)
        : m_tBegin(Now())
        , m_nStatTime(nValueOut)
    {
    }
    ~CTimeStat()
    {
        m_nStatTime = (int32_t)(Now() - m_tBegin);
    }

private:
    CTimeStat(const CTimeStat& rhs);
    CTimeStat& operator=(const CTimeStat& rhs);

private:
    uint64_t    m_tBegin;
    int32_t&    m_nStatTime;
};

#define TIME_BEGIN(var)     { CTimeStat timeBegin(var);
#define TIME_END            }


//返回两个timeval相差的毫秒值 lhs-rhs
inline 
double operator-(struct timeval &lhs, struct timeval &rhs)
{
    return ((lhs.tv_sec*1000.0+lhs.tv_usec/1000.0) - (rhs.tv_sec*1000.0+rhs.tv_usec/1000.0));
}


#endif // _MYLIB_BASECODE_TIME_FUNCTION_H_

