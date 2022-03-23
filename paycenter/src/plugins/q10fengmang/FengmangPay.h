/*
 * =====================================================================================
 *
 *       Filename:  FengmangPay.h
 *
 *    Description:  锋芒支付定义
 *
 *        Version:  1.0
 *        Created:  2021年06月08日 13时03分12秒
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
#include "MyNet.h"

class CFengmangPay : public CPayBase
{
public:
    CFengmangPay();
    virtual ~CFengmangPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCB);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

};







