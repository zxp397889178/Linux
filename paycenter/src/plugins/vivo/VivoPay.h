/*
 * =====================================================================================
 *
 *       Filename:  VivoPay.h
 *
 *    Description:  VIVO支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 17时49分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#pragma once
#include "../PayBase.h"
#include "IWebPlugin.h"

class CVivoPay : public CPayBase
{
public:
    CVivoPay();
    virtual ~CVivoPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doGetSign)
    ENTRY_FUNC_DEFINE(doDepositCB);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
    
private:
    bool fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data);
    void OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp);
};

