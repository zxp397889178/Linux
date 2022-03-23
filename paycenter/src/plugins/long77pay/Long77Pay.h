/*
 * =====================================================================================
 *
 *       Filename:  Long77Pay.h
 *
 *    Description:  龙77pay 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月04日 17时49分09秒
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
#include "MyNet.h"

class CLong77Pay : public CPayBase
{
public:
    CLong77Pay();
    virtual ~CLong77Pay();

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




