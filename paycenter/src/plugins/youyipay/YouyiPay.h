/*
 * =====================================================================================
 *
 *       Filename:  YouyiPay.h
 *
 *    Description:  优亿支付
 *
 *        Version:  1.0
 *        Created:  2021年09月26日 11时24分32秒
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

class CYouyiPay : public CPayBase
{
public:
    CYouyiPay();
    virtual ~CYouyiPay();

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
