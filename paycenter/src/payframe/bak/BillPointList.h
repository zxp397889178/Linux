/*
 * =====================================================================================
 *
 *       Filename:  BillPointList.h
 *
 *    Description:  计费点列表管理
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时27分00秒
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
#include <vector>

class CBillPoint
{
public:
    CBillPoint(int32_t bpId, int32_t configId, const CMyString &bpCode,
            int64_t billpoint, int64_t coin, int64_t extCoin,
            int32_t initCount, const CMyString &bpIconUrl, int32_t bpNameId);
    ~CBillPoint();

    int32_t getId(void)        { return m_bpId; }
    int32_t getInitCount(void) { return m_initCount; }
    int32_t getConfigId(void)  { return m_configId; }

    void toJson(const CMyString &uin, Json::Value &billpoint_array);

private:
    int32_t         m_bpId;
    int32_t         m_configId;
    int32_t         m_initCount;
    Json::Value     m_data;
};

class CBillPointList
{
    typedef std::vector<CBillPoint>         VEC_BILLPOINT;
public:
    ~CBillPointList();
    static CBillPointList& InstanceGet();

    int32_t Load(ISimDB *pDB);
    CBillPoint* Find(int32_t bpId);

    void toJson(int32_t configId, const CMyString &uin, Json::Value &billpoint_array);


private:
    CBillPointList();

private:
    VEC_BILLPOINT       m_vecBillPoint;

};

#define BILLPOINT_LIST      CBillPointList::InstanceGet()


