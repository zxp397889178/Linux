/*
 * =====================================================================================
 *
 *       Filename:  ApplePay.h
 *
 *    Description:  苹果支付接口
 *
 *        Version:  1.0
 *        Created:  2021年04月09日 14时35分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "../PayBase.h"
#include "IWebPlugin.h"

// #define APPLE_CERT_BY_REDIS
#undef APPLE_CERT_BY_REDIS

class CPluginApplePay : public CPayBase
{
public:
    CPluginApplePay();
    virtual ~CPluginApplePay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doVerify);

private:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

    void addDepositVerifyNeededParam(std::vector<CMyString> &vecParam);
    void storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data);
    int32_t verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);

private:
    int32_t dealResp(CMyString &strResp, CMyString &strError, CMyString &appleOrderNo);
    bool doAppleCert(const CMyString &strJsonStr, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);

private:
    CMyString   m_sandboxUrl;       // 沙盒充值URL
    CMyString   m_appleUrl;         // 苹果充值URL
    bool        m_sandboxFirst;     // 是否优化采用sandbox, true-是, false-否
    bool        m_autoSwitchUrl;    // 是否自动切换URL， 21007=>sandbox, 21008=>real

    redis_cache::CIncrCache  m_cacheAppleOrder;
};


