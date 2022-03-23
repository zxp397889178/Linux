/*
 * =====================================================================================
 *
 *       Filename:  BillpointDataList.h
 *
 *    Description:  关于UIN的计费点处理
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时49分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"
#include <map>

// 周期性统计的类
class CPeriodicStat
{
public:
    enum cycle_style
    {
        cycleDay   = 1,     // 以天为周期
        cycleWeek,          // 以周为周期
        cycleMonth,         // 以月为周期
    };
public:
    CPeriodicStat(int cycleStyle);
    ~CPeriodicStat();

    void Maintance();
    void Add(const CMyString &uin, int32_t bpId, int32_t cnt);
    void clear();

    // 0表示uin未用bpId, >0表示uin采用bpId的次数
    int32_t getCount(const CMyString &uin, int32_t bpId);

private:
    // key: uin:<uin>;bpId:<bpId>
    std::map<CMyString, int32_t>    m_mapBpStat;

    enum cycle_style    m_cycle;
    time_t              m_tBegin;
};
///////////////////////////////////////////////////////////////////////////////


class CBPDataList
{
public:
    ~CBPDataList();
    static CBPDataList& InstanceGet();

    int32_t Load(ISimDB *pDB);

    // 每秒对日、月计费点进行统计
    void Maintance(void);

    // 增加用户使用计费点
    void IncBill(const CMyString &uin, int32_t bpId);
    void DecBill(const CMyString &uin, int32_t bpId);

    // 获得计费点周期性统计
    // 返回值：-1表示出错，0表示uin未用该计费点， >0 表示uin采用计费点的次数
    int32_t getBillStat(const CMyString &uin, int32_t bpId, enum CPeriodicStat::cycle_style cycleStyle);

    // 获得计费点总共的支付次数
    // 返回值： 0表示uin未用该计费点,>0表示uin采用计费点的次数
    int32_t getBillCount(const CMyString &uin, int32_t bpId);

    void ToJson(const CMyString &uin, int32_t bpId, Json::Value &req);

private:
    CBPDataList();

    int32_t LoadDailyInfo(ISimDB *pDB);
    int32_t LoadWeeklyInfo(ISimDB *pDB);
    int32_t LoadMonthlyInfo(ISimDB *pDB);

private:
    std::map<CMyString, int>    m_mapBillData;
    // MAP_BILLDATA        m_mapBillData;

    CPeriodicStat       m_dailyStat;    // 每日计费点统计
    CPeriodicStat       m_weeklyStat;   // 每周计费点统计
    CPeriodicStat       m_monthlyStat;  // 每月计费点统计
    CMyTimer            m_timer;
};

#define BPDATA_LIST         CBPDataList::InstanceGet()

