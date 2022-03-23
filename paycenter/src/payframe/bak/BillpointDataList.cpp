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

CBPDataList::CBPDataList()
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

    pDB->Cmd("SELECT `uin`,`bpId`,`dealCount` FROM `t_billpoint_stat` ORDER BY `uin`");
    while(pDB->More())
    {
        CMyString uin = pDB->GetString(0);
        Item item;
        item.bpId = pDB->GetInt(1);
        item.count = pDB->GetInt(2);
        m_mapBillData.insert(std::make_pair(uin, item));
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_billpoint_stat data failure, error %s", szError);
        return -1;
    }
    return 0;
}

CBPDataList::Item* CBPDataList::GetBillItem(const CMyString &uin, int32_t bpId)
{
    Item *pBillItem(NULL);
    MAP_BILLDATA::iterator it1(m_mapBillData.lower_bound(uin));
    MAP_BILLDATA::iterator it2(m_mapBillData.upper_bound(uin));
    for(MAP_BILLDATA::iterator it(it1); it != it2; ++it)
    {
        Item &item = it->second;
        if (item.bpId == bpId)
        {
            pBillItem = &item;
            break;
        }
    }
    return pBillItem;
}


bool CBPDataList::GetBillData(const CMyString &uin, int32_t bpId, int32_t &count)
{
    Item *pBillItem = this->GetBillItem(uin, bpId);
    if (pBillItem != NULL)
    {
        count = pBillItem->count;
        return true;
    }
    return false;
}

void CBPDataList::IncBill(const CMyString &uin, int32_t bpId)
{
    Item *pBillItem = this->GetBillItem(uin, bpId);
    if (pBillItem != NULL)
    {
        pBillItem->count = pBillItem->count + 1;
    }
    else
    {
        Item item;
        item.bpId = bpId;
        item.count = 1;
        m_mapBillData.insert(std::make_pair(uin, item));
    }
}

