/*
 * =====================================================================================
 *
 *       Filename:  SteamPay.h
 *
 *    Description:  Steam平台充值
 *
 *        Version:  1.0
 *        Created:  2022年01月15日 11时17分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once
#include "../PayBase.h"
#include "IWebPlugin.h"

class CSteamPay : public CPayBase
{
public:
    CSteamPay();
    virtual ~CSteamPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doVerify);
    ENTRY_FUNC_DEFINE(doDepositCB);

protected:
    void addDepositVerifyNeededParam(std::vector<CMyString> &vecParam);
    void storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data);
    int32_t verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);

    int32_t verifySteamOrder(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);

private:
    bool GetUserInfo(const CMyString& strKey, int64_t& nSteamId, const CMyString& strIp, CMyString& strCurrency);
    bool FinalizeTxn(const CMyString& strKey, uint32_t appid, uint64_t orderid, bool &depositOK, CMyString &platformOrderNo);
    bool QueryTxn(const CMyString& strKey, uint32_t appid, uint64_t orderid, int64_t &realFee, CMyString& strStatus);

    int64_t TransOrder(const CMyString& orderNo);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

private:
    CMyString m_strGetUserInfoUrl;
    CMyString m_strFinalizeTxnUrl;
    CMyString m_strQueryTxnUrl;
    redis_cache::CIncrCache     m_cacheSteamOrder;
};
