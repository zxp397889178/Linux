/*
 * =====================================================================================
 *
 *       Filename:  Le7Pay.h
 *
 *    Description:  乐7联运sdk支付
 *
 *        Version:  1.0
 *        Created:  2021年08月17日 10时40分40秒
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

class CLe7Pay : public CPayBase
{
public:
    CLe7Pay();
    virtual ~CLe7Pay();

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
