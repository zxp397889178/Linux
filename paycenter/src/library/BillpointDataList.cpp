/*
 * =====================================================================================
 *
 *       Filename:  BillpointDataList.cpp
 *
 *    Description:  针对用户的计费点使用情况统计
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时55分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "BillpointDataList.h"
#include <algorithm>
#include <map>

CPeriodicStat::CPeriodicStat(int32_t cycleStyle)
    : m_cycle(static_cast<enum cycle_style>(cycleStyle) )
{
    m_tBegin = ::time(NULL);
    // LogFatal("init periodic date");
}

CPeriodicStat::~CPeriodicStat()
{

}

void CPeriodicStat::Maintance()
{
    time_t tNow = ::time(NULL);
    //LogDebug("maintance: begin:%ld, now:%ld", m_tBegin, tNow);
    struct tm tmBegin = {0};
    struct tm tmNow = {0};
    localtime_r(&m_tBegin, &tmBegin);
    localtime_r(&tNow, &tmNow);
 
    bool bNeedReset(false);
    switch(m_cycle)
    {
    case cycleDay:
        if (tmBegin.tm_yday != tmNow.tm_yday)
        {
            // LogFatal("date changed.");
            LogWarn("daily> maintance: year_day1:%d, year_day2:%d, need reset periodic stat", tmBegin.tm_yday, tmNow.tm_yday);
            bNeedReset = true;
        }
        break;
    case cycleWeek:
        if (tmBegin.tm_yday != tmNow.tm_yday)
        {
            // 每周日重置
            if (tmNow.tm_wday==0)
            {
                LogWarn("weekly> maintance: year_day1:%d, year_day2:%d, need reset periodic stat", tmBegin.tm_yday, tmNow.tm_yday);
                bNeedReset = true;
            }
        }
        break;
    case cycleMonth:
        if (tmBegin.tm_yday != tmNow.tm_yday)
        {
            if (tmBegin.tm_mon != tmNow.tm_mon)
            {
                LogWarn("monthly> maintance: year_day1:%d, year_day2:%d, need reset periodic stat", tmBegin.tm_yday, tmNow.tm_yday);
                bNeedReset = true;
            }
        }
        break;
    default:
        break;
    }

    if (bNeedReset)
    {
        m_tBegin = tNow;
        m_mapBpStat.clear();
        LogImpt("reset user billpoint stat, style:%d", m_cycle);
        // LogFatal("chang periodic date");
    }
}

void CPeriodicStat::Add(const CMyString &uin, int32_t bpId, int32_t cnt)
{
    CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
    std::map<CMyString, int32_t>::iterator it(m_mapBpStat.find(strKey));
    if (it == m_mapBpStat.end())
    {
        if (cnt>0)
        {
            m_mapBpStat.insert(std::make_pair(strKey, cnt));
        }
        else
        {
            m_mapBpStat.insert(std::make_pair(strKey, 0));
        }
    }
    else
    {
        int32_t val = it->second;
        if (cnt>=0)
        {
            it->second = val+cnt;
        }
        else
        {
            if (val >= -cnt)
            {
                it->second = val+cnt;
            }
            else
            {
                it->second = 0;
            }
        }
    }
}

void CPeriodicStat::clear()
{
    m_mapBpStat.clear();
}

int32_t CPeriodicStat::getCount(const CMyString &uin, int32_t bpId)
{
    CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
    std::map<CMyString, int32_t>::iterator it(m_mapBpStat.find(strKey));
    if (it == m_mapBpStat.end())
    {
        return 0;
    }
    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
CBPDataList::CBPDataList()
    : m_dailyStat(CPeriodicStat::cycleDay)
    , m_weeklyStat(CPeriodicStat::cycleWeek)
    , m_monthlyStat(CPeriodicStat::cycleMonth)
{

}

CBPDataList::~CBPDataList()
{

}

CBPDataList& CBPDataList::InstanceGet()
{
    static CBPDataList s_obj;
    return s_obj;
}

int32_t CBPDataList::Load(ISimDB *pDB)
{
    m_mapBillData.clear();
    m_dailyStat.clear();
    m_weeklyStat.clear();
    m_monthlyStat.clear();

    // uin:<uin>;bpId:<uin>
    //pDB->Cmd("SELECT `uin`,`bpId`,`dealCount` FROM `t_billpoint_stat` ORDER BY `uin`");
    pDB->Cmd("call pUserBillpointStat_v3(NOW(), 0)");
    while(pDB->More())
    {
        CMyString uin = pDB->GetString(0);
        int32_t bpId = pDB->GetInt(1);
        int32_t count = pDB->GetInt(2);
        CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
        m_mapBillData[strKey] = count;
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_billpoint_stat data failure, error %s", szError);
        return -1;
    }

    int32_t ret = this->LoadDailyInfo(pDB);
    if (ret < 0)
    {
        return -3;
    }
    ret = this->LoadWeeklyInfo(pDB);
    if (ret<0)
    {
        return -5;
    }
    ret = this->LoadMonthlyInfo(pDB);
    if (ret < 0)
    {
        return -7;
    }

    m_timer.SetInterval(1000);
    m_timer.Startup();

    return 0;
}

void CBPDataList::IncBill(const CMyString &uin, int32_t bpId)
{
    CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
    std::map<CMyString, int32_t>::iterator it(m_mapBillData.find(strKey));
    if (it == m_mapBillData.end())
    {
        m_mapBillData[strKey] = 1;
    }
    else
    {
        it->second = it->second + 1;
    }
    m_dailyStat.Add(uin, bpId, 1);
    m_weeklyStat.Add(uin, bpId, 1);
    m_monthlyStat.Add(uin, bpId, 1);
}

void CBPDataList::DecBill(const CMyString &uin, int32_t bpId)
{
    CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
    std::map<CMyString, int32_t>::iterator it(m_mapBillData.find(strKey));
    if (it == m_mapBillData.end())
    {
        m_mapBillData[strKey] = 0;
    }
    else
    {
        int32_t val = it->second;
        it->second = (val>=1) ? (val-1) : 0;
    }
    m_dailyStat.Add(uin, bpId, -1);
    m_weeklyStat.Add(uin, bpId, -1);
    m_monthlyStat.Add(uin, bpId, -1);
}

void CBPDataList::Maintance(void)
{
    // 每隔一秒统计用户充值数据
    if (m_timer.ToNextTime())
    {
        m_dailyStat.Maintance();
        m_weeklyStat.Maintance();
        m_monthlyStat.Maintance();
    }
}

int32_t CBPDataList::LoadDailyInfo(ISimDB *pDB)
{
    time_t t1 = time(NULL);
    char szToday[64] = "";
    TimeToStr(t1, szToday, sizeof(szToday), "yyyy-mm-dd");

    //pDB->Cmd("SELECT `uin`,`bpId`,COUNT(*) ");
    //pDB->Cmd("FROM `t_payorder` ");
    //pDB->Cmd("WHERE `orderStatus` IN (5,8,9,10) AND `createdAt`>='%s' AND `createdAt`<='%s 23:59:59.999999' ", szToday, szToday);
    //pDB->Cmd("GROUP BY `uin`,`bpId`; ");
    pDB->Cmd("call pUserBillpointStat_v3('%s', 1)", szToday);
    while(pDB->More())
    {
        CMyString uin = pDB->GetString(0);
        int32_t bpId = pDB->GetInt(1);
        int32_t cnt = pDB->GetInt(2);
        m_dailyStat.Add(uin, bpId, cnt);
    }

    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_payorder daily stat failure, error %s", szError);
        return -1;
    }

    return 0;
}

int32_t CBPDataList::LoadWeeklyInfo(ISimDB *pDB)
{
    time_t t1 = time(NULL);
    char szToday[64] = "";
    TimeToStr(t1, szToday, sizeof(szToday), "yyyy-mm-dd");

    pDB->Cmd("call pUserBillpointStat_v3('%s', 2)", szToday);
    while(pDB->More())
    {
        CMyString uin = pDB->GetString(0);
        int32_t bpId = pDB->GetInt(1);
        int32_t cnt = pDB->GetInt(2);
        m_weeklyStat.Add(uin, bpId, cnt);
    }

    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_payorder daily stat failure, error %s", szError);
        return -1;
    }

    return 0;
}

int32_t CBPDataList::LoadMonthlyInfo(ISimDB *pDB)
{
/*    
    time_t t1 = time(NULL);
    char szMonth1[64] = "";
    TimeToStr(t1, szMonth1, sizeof(szMonth1), "yyyy-mm-01");
    char szMonth2[64] = "";
    TimeToStr(t1, szMonth2, sizeof(szMonth2), "yyyy-mm-dd");

    pDB->Cmd("SELECT `uin`,`bpId`,COUNT(*) ");
    pDB->Cmd("FROM `t_payorder` ");
    pDB->Cmd("WHERE `orderStatus` IN (5,8,9,10) AND `createdAt`>='%s' AND `createdAt`<='%s 23:59:59.999999' ", szMonth1, szMonth2);
    pDB->Cmd("GROUP BY `uin`,`bpId`; ");
*/

    time_t t1 = time(NULL);
    char szToday[64] = "";
    TimeToStr(t1, szToday, sizeof(szToday), "yyyy-mm-dd");

    pDB->Cmd("call pUserBillpointStat_v3('%s', 3)", szToday);
    while(pDB->More())
    {
        CMyString uin = pDB->GetString(0);
        int32_t bpId = pDB->GetInt(1);
        int32_t cnt = pDB->GetInt(2);
        m_monthlyStat.Add(uin, bpId, cnt);
    }

    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_payorder monthly stat failure, error %s", szError);
        return -1;
    }

    return 0;

}

int32_t CBPDataList::getBillStat(const CMyString &uin, int32_t bpId, enum CPeriodicStat::cycle_style cycleStyle)
{
    int32_t result(-1);
    switch(cycleStyle)
    {
    case CPeriodicStat::cycleDay:
        result = m_dailyStat.getCount(uin, bpId);
        break;
    case CPeriodicStat::cycleWeek:
        result = m_weeklyStat.getCount(uin, bpId);
        break;
    case CPeriodicStat::cycleMonth:
        result = m_monthlyStat.getCount(uin, bpId);
        break;
    default:
        break;
    }
    return result;
}

int32_t CBPDataList::getBillCount(const CMyString &uin, int32_t bpId)
{
    CMyString strKey("uin:%s;bpId:%d", uin.c_str(), bpId);
    std::map<CMyString, int32_t>::iterator it(m_mapBillData.find(strKey));
    if (it == m_mapBillData.end())
    {
        return 0;
    }

    return it->second;
}

void CBPDataList::ToJson(const CMyString &uin, int32_t bpId, Json::Value &req)
{
    Json::Value count_stat;
    count_stat["total"] = this->getBillCount(uin, bpId);
    count_stat["daily"] = this->getBillStat(uin, bpId, CPeriodicStat::cycleDay);
    //count_stat["weekly"] = this->getBillStat(uin, bpId, CPeriodicStat::cycleWeek);
    count_stat["monthly"] = this->getBillStat(uin, bpId, CPeriodicStat::cycleMonth);
    req["used_stat"] = count_stat;
}


