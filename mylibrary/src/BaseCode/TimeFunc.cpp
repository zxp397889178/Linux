/*
 * =====================================================================================
 *
 *       Filename:  TimeFunc.cpp
 *
 *    Description:  some function of time
 *
 *        Version:  1.0
 *        Created:  2014/11/17 22时43分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "TimeFunc.h"
#include "ILock.h"


uint64_t Now(bool bReturnMicroSecond)
{
	struct timeval tv = {0};
	//struct timezome tz;
	gettimeofday(&tv, NULL);
	uint64_t val(0);
	if (bReturnMicroSecond)
		val = (uint64_t)(tv.tv_sec*1e6) + tv.tv_usec;
	else
		val = (uint64_t)(tv.tv_sec*1000) + (tv.tv_usec/1000);
	return val;
}

uint64_t GetBeginOfToday()
{
	time_t t1 = time(0);
    return GetDayBeginOfTime(t1)*1000;
}

time_t GetDayBeginOfTime(time_t v)
{
	struct tm t2;
	localtime_r(&v, &t2);
	t2.tm_hour = 0;
	t2.tm_min = 0;
	t2.tm_sec = 0;
	time_t t3 = mktime(&t2);
	return t3;
}

uint64_t GetTimeOfToday(uint32_t secondsOfToday)
{
	uint64_t t = GetBeginOfToday();
	t += secondsOfToday*1000;
	return t;
}

uint32_t GetTimepassOfToday(bool bReturnMilliSecond)
{
	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	struct tm t1;
	localtime_r(&tv.tv_sec, &t1);
	uint32_t timePass = t1.tm_hour*3600 + t1.tm_min*60 + t1.tm_sec*60;
	if (bReturnMilliSecond)
	{
		return timePass * 1000 + tv.tv_usec/1000;
	}
	return timePass;
}

const char* _week_name[]  = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char* _month_name[] = { "Jan", "Feb", "Mar", "May", "Apr", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

std::string getGMTDate(const time_t &t1)
{
	char buf[512] = "";
	// struct tm *gt = gmtime(&t1);
	struct tm gt1 = {0};
	struct tm *gt = gmtime_r(&t1, &gt1);
	if (NULL != gt)
	{
		if (gt1.tm_wday <0 || gt1.tm_wday >= 7)
		{
			gt1.tm_wday = 2;
		}
		if (gt1.tm_mon <0 || gt1.tm_mon >= 12)
		{
			gt1.tm_mon = 11;
		}

		snprintf(buf, sizeof(buf)-1, "%s, %02d-%s-%04d %02d:%02d:%02d GMT",
				_week_name[gt1.tm_wday],
				gt1.tm_mday, _month_name[gt1.tm_mon], gt1.tm_year+1900, 
				gt1.tm_hour, gt1.tm_min, gt1.tm_sec);
	}
	return std::string(buf);
}

void nsleep(uint64_t dwTimeout)
{
	struct timespec timeout;
	timeout.tv_sec = dwTimeout/1000;
	timeout.tv_nsec = (dwTimeout%1000)*1000000;
	nanosleep(&timeout, NULL);
}

void nsleep_ex(uint64_t dwTimeout)
{
	struct timespec timeout;
	timeout.tv_sec = dwTimeout/1000000;
	timeout.tv_nsec = (dwTimeout%1000000)*1000;
	nanosleep(&timeout, NULL);
}

int32_t PthreadSleep(uint32_t dwTimeout, ICondition *&pCond)
{
#if 0
    struct timespec timeout;
	timeout.tv_sec = dwTimeout/1E3;
	timeout.tv_nsec = (dwTimeout%1000)*1E6;

	pthread_cond_t myCond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&myMutex);
	int rv = pthread_cond_timedwait(&myCond, &myMutex, &timeout);
	switch(rv)
	{
	case ETIMEDOUT:
		// Handle timeout
		break;
	case EINTR:
		// interupted by signal
		break;
	case 0:
		// condition received a condition signal
		break;
	case EBUSY:
	default:
		// Handle errors
		break;
	}
	pthread_mutex_unlock(&myMutex);

#else
	if (NULL == pCond)
	{
		pCond = OpenCondition();
		if (NULL == pCond)
		{
			return -1;
		}
	}
	pCond->Lock();
	pCond->Wait(dwTimeout);
	pCond->Unlock();
	return 0;
#endif	
}

/* 
void PthreadSleep(double dblTimeout)
{
    struct timespec timeout;
    timeout.tv_sec = (time_t)(dblTimeout/1000);
    timeout.tv_nsec = (long)((dblTimeout - timeout.tv_sec*1000)*1000*1000);
    int ret = pthread_delay_np(&timeout);
    assert(ret==0);
}
*/



