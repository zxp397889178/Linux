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

class CBPDataList
{
    struct Item
    {
        int32_t         bpId;           // 计费点ID
        int32_t         count;          // 使用次数
    };
    typedef std::multimap<CMyString, Item>     MAP_BILLDATA;
public:
    ~CBPDataList();
    static CBPDataList& InstanceGet();

    int32_t Load(ISimDB *pDB);

    bool GetBillData(const CMyString &uin, int32_t bpId, int32_t &count);
    void IncBill(const CMyString &uin, int32_t bpId);

private:
    CBPDataList();
    struct Item* GetBillItem(const CMyString &uin, int32_t bpId);

private:
    MAP_BILLDATA        m_mapBillData;
};

#define BPDATA_LIST         CBPDataList::InstanceGet()

