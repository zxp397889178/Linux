/*
 * =====================================================================================
 *
 *       Filename:  WithdrawLimitData.h
 *
 *    Description:  充值提现配置数据
 *
 *        Version:  1.0
 *        Created:  2021年07月29日 11时22分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"
#include "MyNet.h"
#include <vector>

class CWithdrawLimit
{
public:
    CWithdrawLimit(int32_t id, const CMyString &gameCode, int64_t deposit, int64_t withdraw);
    ~CWithdrawLimit();

    const CMyString& getGameCode(void) { return m_gameCode; }
    int32_t getId(void) {return m_id; }
    int64_t getDeposit(void)   { return m_deposit; }
    int64_t getWithdraw(void)   { return m_withdraw; }
     
    bool toJson(Json::Value &array);

private:
    CMyString       m_gameCode;     
    int32_t         m_id;
    int64_t         m_deposit;      //充值额度
    int64_t         m_withdraw;     //提现额度
    Json::Value     m_data;
};

class CWithdrawLimitData
{
    typedef std::vector<CWithdrawLimit>        VEC_WITHDRAWLIMIT;
public:
    ~CWithdrawLimitData();
    static CWithdrawLimitData& InstanceGet();

    int32_t Load(ISimDB *pDB);
    int64_t FindLimitByDeposit(const CMyString& gameCode, const int64_t &deposit);

    void toJson(const CMyString &gameCode, Json::Value &resp);
    void toJsonEx(ISimDB *pDB, const CMyString &gameCode, const CMyString &strUin, Json::Value &resp);

private:
    CWithdrawLimitData();

private:
    VEC_WITHDRAWLIMIT      m_vecWithdrawLimit;
};

#define WITHDRAWLIMIT_DATA     CWithdrawLimitData::InstanceGet()






