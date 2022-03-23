/*
 * =====================================================================================
 *
 *       Filename:  TimeRange.cpp
 *
 *    Description:  时间范围检查
 *
 *        Version:  1.0
 *        Created:  02/07/2020 01:59:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "TimeRange.h"

CTimeRange::CTimeRange()
{

}

CTimeRange::~CTimeRange()
{

}

bool CTimeRange::Init(const CMyString &str)
{
    m_vecTime.clear();

    int32_t ret(0);
    std::vector<std::string> vecRange;
    try
    {
        ret = safe_strsplit(str.c_str(), vecRange, ";");
        if (ret<=0)
        {
            LogFatal("[%s:%s:%d]decode time_string [%s] failure", __FILE__, __func__, __LINE__, str.c_str());
            return false;
        }

        for(std::vector<std::string>::iterator it(vecRange.begin());
                it != vecRange.end(); ++it)
        {
            std::string &range = *it;
            size_t pos = range.find("-");
            if (pos == std::string::npos)
            {
                continue;
            }

            CMyString str1 = range.substr(0, pos);
            CMyString str2 = range.substr(pos+1);
            str1.trim();
            str2.trim();

            int32_t t1 = Time2Seconds(str1.c_str(), "HH:MM:SS");
            int32_t t2 = Time2Seconds(str2.c_str(), "HH:MM:SS");
            if (t1 == t2)
            {
                continue;
            }
            struct TimeRange tr1;
            tr1.begin = (t1>t2) ? t2 : t1;
            tr1.end = (t1>t2) ? t1 : t2;
            m_vecTime.push_back(tr1);
        }
    }
    catch(CException &e)
    {
        LogFatal("[%s:%s:%d]decode timestring [%s] failure, error: %s",
                __FILE__, __func__, __LINE__, str.c_str(), e.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s:%s:%d]decode timestring [%s] failure",
                __FILE__, __func__, __LINE__, str.c_str());
        return false;
    }

    if (m_vecTime.empty())
    {
        LogFatal("[%s:%s:%d]decode time_string [%s] failure",
                __FILE__, __func__, __LINE__, str.c_str());
        return false;
    }

    return true;
}

bool CTimeRange::Valid(time_t &t1)
{
    time_t tStart = GetDayBeginOfTime(t1);
    
    std::vector<TimeRange>::iterator it(m_vecTime.begin());
    for(; it != m_vecTime.end(); ++it)
    {
        struct TimeRange &tr1 = *it;
        time_t tBegin = tr1.begin + tStart;
        time_t tEnd = tr1.end + tStart;
        if (t1 >= tBegin && t1 <= tEnd)
        {
            return true;
        }
    }

    return false;
}


