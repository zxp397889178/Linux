/*
 * =====================================================================================
 *
 *       Filename:  BillPointList.cpp
 *
 *    Description:  计费点列表管理
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时29分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "BillPointList.h"
#include "BillpointDataList.h"

CBillPoint::CBillPoint(int32_t bpId, int32_t configId, const CMyString &bpCode,
            int64_t billpoint, int64_t coin, int64_t extCoin,
            int32_t initCount, const CMyString &bpIconUrl, int32_t bpNameId)
    : m_bpId(bpId)
    , m_configId(configId)
    , m_initCount(initCount)
{
    m_data["bpId"] = bpId;
    m_data["configId"] = configId;
    m_data["bpCode"] = bpCode;
    m_data["billPoint"] = (Json::Int64)billpoint;
    m_data["coin"] = (Json::Int64)coin;
    m_data["extraCoin"] = (Json::Int64)extCoin;
    m_data["initCount"] = initCount;
    // m_data["usedCount"] = 0;    // 需要后续重置
    m_data["bpIcon"] = bpIconUrl;
    m_data["bpDescId"] = bpNameId;
}

CBillPoint::~CBillPoint()
{
}

void CBillPoint::toJson(const CMyString &uin, Json::Value &billpoint_array)
{
    int32_t count(0);
    bool ret = BPDATA_LIST.GetBillData(uin, m_bpId, count);
    if (!ret)
    {
        count = 0;
    }

    Json::Value data = m_data;
    data["usedCount"] = count;
    billpoint_array.append(data);
}

//////////////////////////////////////////////////////////////////////
//CBillPointList
CBillPointList::CBillPointList()
{

}

CBillPointList::~CBillPointList()
{

}

CBillPointList& CBillPointList::InstanceGet()
{
    static CBillPointList s_obj;
    return s_obj;
}

int32_t CBillPointList::Load(ISimDB *pDB)
{
    m_vecBillPoint.clear();

    pDB->Cmd("SELECT `bpId`,`configId`,`bpCode`,`billPoint`,`coin`,"\
            "`extraCoin`,`extraCount`,`bpIcon`,`bpDescId` ");
    pDB->Cmd("FROM `t_billpoint` WHERE `status`=1 ");
    while(pDB->More())
    {
        CBillPoint bp(pDB->GetInt(0),   // billpointId
                pDB->GetInt(1),     // configId
                pDB->GetString(2),  // billpointCode
                pDB->GetInt64(3),   // billpoint
                pDB->GetInt64(4),   // coin
                pDB->GetInt64(5),   // extraCoin
                pDB->GetInt(6),     // extraCount
                pDB->GetString(7),  // billpointIconUrl
                pDB->GetInt(8)      // billpointDescId (langId)
                );
        m_vecBillPoint.push_back(bp);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_billpoint data failure, error %s", szError);
        return -1;
    }

    return 0;
}

CBillPoint* CBillPointList::Find(int32_t bpId)
{
    CBillPoint *pBillPoint(NULL);
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getId() == bpId)
        {
            pBillPoint = &bp;
            break;
        }
    }
    return pBillPoint;
}

void CBillPointList::toJson(int32_t configId, const CMyString &uin, Json::Value &billpoint_array)
{
    VEC_BILLPOINT::iterator it(m_vecBillPoint.begin());
    for(; it != m_vecBillPoint.end(); ++it)
    {
        CBillPoint &bp = *it;
        if (bp.getConfigId() != configId)
        {
            continue;
        }
        
        bp.toJson(uin, billpoint_array);
    }
}
