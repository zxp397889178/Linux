/*
 * =====================================================================================
 *
 *       Filename:  WithdrawLimitData.cpp
 *
 *    Description:  充值提现配置数据
 *
 *        Version:  1.0
 *        Created:  2021年07月29日 11时23分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "WithdrawLimitData.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "UserCreditList.h"

extern CGame* GetGameByCode(const CMyString &gameCode, Json::Value &resp);

CWithdrawLimit::CWithdrawLimit(int32_t id, const CMyString &gameCode, int64_t deposit, int64_t withdraw)
{
    m_id = id;
    m_deposit = deposit;
    m_withdraw = withdraw;
    m_gameCode = gameCode;
    
     // 生成Json 数据项
    m_data["gamecode"] = gameCode;
    m_data["id"] = id;
    m_data["deposit"] = (Json::Int64)deposit;
    m_data["withdraw"] =(Json::Int64)withdraw;
}

CWithdrawLimit::~CWithdrawLimit()
{

}

bool CWithdrawLimit::toJson(Json::Value &array)
{
    // 填充到支付渠道中
    array.append(m_data);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
CWithdrawLimitData::CWithdrawLimitData()
{

}

CWithdrawLimitData::~CWithdrawLimitData()
{

}

CWithdrawLimitData& CWithdrawLimitData::InstanceGet()
{
    static CWithdrawLimitData s_obj;
    return s_obj;
}

int32_t CWithdrawLimitData::Load(ISimDB *pDB)
{
    m_vecWithdrawLimit.clear();

    pDB->Cmd("SELECT `id`,`gameCode`,`deposit`,`withdraw` FROM `dict_withdrawlimit` ");
    while(pDB->More())
    {
        CWithdrawLimit withdrawlimit( pDB->GetInt(0),    // id
                pDB->GetString(1),          // gameCode
                pDB->GetInt64(2),           // deposit
                pDB->GetInt64(3)            // withdraw
                );
        m_vecWithdrawLimit.push_back(withdrawlimit);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query dict_withdrawlimit data failure, error %s", szError);
        return -1;
    }

    return 0;
}

int64_t CWithdrawLimitData::FindLimitByDeposit(const CMyString& gameCode, const int64_t &deposit)
{
    std::map<int64_t, CWithdrawLimit> mapLimit;
    VEC_WITHDRAWLIMIT::iterator it(m_vecWithdrawLimit.begin());
    for(; it != m_vecWithdrawLimit.end(); ++it)
    {
        CWithdrawLimit &withdrawlimit = *it;
        if (withdrawlimit.getGameCode() == gameCode)
        {
            mapLimit.insert(make_pair(withdrawlimit.getDeposit(), withdrawlimit));
        }
    }

    std::map<int64_t, CWithdrawLimit>::reverse_iterator iter(mapLimit.rbegin());
    for (; iter != mapLimit.rend(); ++iter)
    {
        if (deposit >= iter->second.getDeposit())
        {
            return iter->second.getWithdraw();
        }
    }

    return -1;
}

void CWithdrawLimitData::toJson(const CMyString &gameCode, Json::Value &resp)
{
    // 获得withdrawlimit 数据
    std::map<int64_t, CWithdrawLimit> mapLimit;
    VEC_WITHDRAWLIMIT::iterator it(m_vecWithdrawLimit.begin());
    for(; it != m_vecWithdrawLimit.end(); ++it)
    {
        CWithdrawLimit &withdrawlimit = *it;
        if (withdrawlimit.getGameCode() == gameCode)
        {
            mapLimit.insert(make_pair(withdrawlimit.getDeposit(), withdrawlimit));
        }
    }

    std::map<int64_t, CWithdrawLimit>::iterator iter(mapLimit.begin());
    for (; iter != mapLimit.end(); ++iter)
    {
        iter->second.toJson(resp);
    }
}

void CWithdrawLimitData::toJsonEx(ISimDB *pDB, const CMyString &gameCode, const CMyString &strUin, Json::Value &resp)
{
    // 获得withdrawlimit 数据
    std::map<int64_t, CWithdrawLimit> mapLimit;
    VEC_WITHDRAWLIMIT::iterator it(m_vecWithdrawLimit.begin());
    for(; it != m_vecWithdrawLimit.end(); ++it)
    {
        CWithdrawLimit &withdrawlimit = *it;
        if (withdrawlimit.getGameCode() == gameCode)
        {
            mapLimit.insert(make_pair(withdrawlimit.getDeposit(), withdrawlimit));
        }
    }

    Json::Value limit;
    limit.resize(0);
    std::map<int64_t, CWithdrawLimit>::iterator iter(mapLimit.begin());
    for (; iter != mapLimit.end(); ++iter)
    {
        iter->second.toJson(limit);
    }
    resp["totalDeposit"] = (Json::Int64)(USERCREDIT_LIST.GetTotalDeposit(pDB, gameCode, strUin));
    resp["totalWithdraw"] = (Json::Int64)(USERCREDIT_LIST.GetTotalWithdraw(pDB, gameCode, strUin));
    resp["limitWithdraw"] = (Json::Int64)(USERCREDIT_LIST.GetLimitWithdraw(pDB, gameCode, strUin));
    resp["remainWithdraw"] =(Json::Int64)(USERCREDIT_LIST.GetRemainWithdraw(pDB, gameCode, strUin));
    
    resp["limit"] = limit;

}
