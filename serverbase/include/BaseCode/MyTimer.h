/*
 * =====================================================================================
 *
 *       Filename:  MyTimer.h
 *
 *    Description:  timer handler
 *
 *        Version:  1.0
 *        Created:  2014年11月28日 18时05分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYTIMER_H_
#define _MYLIB_BASECODE_MYTIMER_H_

#include "myconf.h"
#include "MyTimeDelta.h"
#include "MyDateTime.h"
#include <set>

class CMyTimer
{
	typedef std::set<uint64_t>	TRIGGER_SET;
public:
    //计时器类，缺省是自动开始的
	//nInterval ==> ms
    CMyTimer(uint32_t nInterval = 1000, bool bAutoActive = true);
    ~CMyTimer();

    //开始计时器
    void Startup();
    //计时器是否到达，参数＝0表示以设置的时间间隔为准
    bool IsTimeout(uint32_t nInterval = 0);
    //判断计时器是否到达，若已到达，则置下一个计时点
    //参数＝0表示以设置的时间间隔为准
    bool ToNextTime(uint32_t nInterval = 0);

    //停止计时器
    void Stop()                           { m_bActive = false; }
    bool IsActive(void) const             { return m_bActive; }
    void SetInterval(uint32_t nInterval);
    uint32_t GetInterval(void) const;
    //获得下一次抵达时间
    uint64_t GetNextTime(void) const;

public:
	//当作触发器之用
	void AddTrigger(time_t t);
	void AddTrigger(CMyDateTime dt);
	// Now() + seconds ==> trigger
	void AddTrigger(uint32_t seconds);
	// Now() + delta ==> trigger
	void AddTrigger(CMyTimeDelta delta);
	bool HasTrigger(void) { return !m_setTrigger.empty(); }
	bool IsTrigger(void);

    //更新计时器
    void Update();

private:
    uint64_t    m_i64Startup;
    uint64_t    m_i64Last;
    uint32_t    m_nInterval;
    bool        m_bActive;

private:
	TRIGGER_SET	m_setTrigger;
};



#endif //_MYLIB_BASECODE_MYTIMER_H_



