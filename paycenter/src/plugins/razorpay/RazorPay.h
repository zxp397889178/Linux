/*
 * =====================================================================================
 *
 *       Filename:  RazorPay.h
 *
 *    Description:  RazorPay(https://razorpay.com/docs/api/orders/#create-an-order)
 *
 *        Version:  1.0
 *        Created:  2020年09月01日 11时45分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "../PayWrapper.h"
#include "IWebPlugin.h"

class CPluginRazorPay : public CPayWrapper
{
public:
    CPluginRazorPay();
    virtual ~CPluginRazorPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doKYC);
    ENTRY_FUNC_DEFINE(doBindCard);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo, 
            CSdkChannel *pChannel, int32_t payMoney);
    int32_t dealKycResp(CMyString &strResp, bool bUseProxy, Json::Value &resp);
    int32_t dealFundResp(CMyString &strResp, bool bUseProxy, Json::Value &resp);
    int32_t dealWithdrawResp(CMyString &strResp, bool bUseProxy, Json::Value &resp);

    int32_t decode_order(CJsonWrapper &jRoot, CMyString &strOrderNo, 
            CMyString &strPlatformId, CMyString &strStatus, int32_t &nFee, int32_t &nRealFee);
    int32_t decode_payout(CJsonWrapper &jRoot, CMyString &strGameCode, CMyString &strUin, http::CHttpRespMaker &resp);
};
