/*
 * =====================================================================================
 *
 *       Filename:  QWPay.h
 *
 *    Description:  360支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月13日 17时58分43秒
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

class C360Pay : public CPayBase
{
public:
    C360Pay();
    virtual ~C360Pay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
};
