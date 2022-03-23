/*
 * =====================================================================================
 *
 *       Filename:  MyTimer.cpp
 *
 *    Description:  timer handler
 *
 *        Version:  1.0
 *        Created:  2014年11月28日 18时07分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include "MyTimer.h"
#include "TimeFunc.h"
#include <iostream>



CMyTimer::CMyTimer(uint32_t nInterval/* = 1000*/, bool bAutoActive/* = true */)
    : m_i64Startup(0)
    , m_i64Last(0)
    , m_nInterval(nInterval)
    , m_bActive(bAutoActive)
{
    m_i64Startup = Now(false);
    m_i64Last = m_i64Startup + m_nInterval;
	// std::cout << m_i64Startup << "->" << m_i64Last;
	// std::cout << ", active:" << (m_bActive?"True" : "False") << std::endl;
}

CMyTimer::~CMyTimer()
{

}

void CMyTimer::Startup()
{
    m_bActive = true;
    m_i64Startup = Now(false);
    m_i64Last = m_i64Startup;
    this->Update();
}

void CMyTimer::Update()
{
    if (m_bActive)
    {
        m_i64Last += m_nInterval;
    }
}

bool CMyTimer::IsTimeout(uint32_t nInterval/* = 0 */)
{
    if (!m_bActive)
    {
        return false;
    }

	bool IsTrigger = this->IsTrigger();
	bool IsTimeout(false);
    if (0==nInterval)
    {
        IsTimeout = (Now(false)>=m_i64Last);
    }
	else
	{
		IsTimeout = (Now(false)>=(m_i64Last - m_nInterval + nInterval) );
	}
	return (IsTimeout || IsTrigger);
}

bool CMyTimer::ToNextTime(uint32_t nInterval /* = 0 */)
{
    if (!m_bActive)
    {
        return false;
    }
    if (this->IsTimeout(nInterval))
    {
        if (nInterval>0)
        {
            m_i64Last = Now(false);    //主要是为了防止nInterval!=0的情况
        }
        this->Update();
        return true;
    }
    return false;
}

void CMyTimer::SetInterval(uint32_t nInterval) 
{
	m_nInterval = nInterval;
}

uint32_t CMyTimer::GetInterval(void) const
{
	return m_nInterval;
}

uint64_t CMyTimer::GetNextTime(void) const
{
	return m_i64Last;
}

void CMyTimer::AddTrigger(time_t t)
{
	CMyDateTime dt((time_t)t, 0);
	this->AddTrigger(dt);
}

void CMyTimer::AddTrigger(CMyDateTime dt)
{
	uint64_t val = dt.GetMilliSeconds();
	if (val >= Now(false))
	{
		m_setTrigger.insert(val);
	}
}

void CMyTimer::AddTrigger(uint32_t seconds)
{
	CMyTimeDelta delta(seconds, 0);
	this->AddTrigger(delta);
}

void CMyTimer::AddTrigger(CMyTimeDelta delta)
{
	CMyDateTime dt;
	dt += delta;
	this->AddTrigger(dt);
}

bool CMyTimer::IsTrigger(void)
{
	if (m_setTrigger.empty())
	{
		return false;
	}

	TRIGGER_SET::iterator it(m_setTrigger.begin());
	uint64_t val = *it;
	if (Now(false) >= val)
	{
		m_setTrigger.erase(it);
		//std::cout << "trigger open" << std::endl;
		return true;
	}

	return false;
}

