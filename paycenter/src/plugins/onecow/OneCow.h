/*
 * =====================================================================================
 *
 *       Filename:  OneCow.h
 *
 *    Description:  一牛H5支付
 *
 *        Version:  1.0
 *        Created:  2021年12月24日 16时30分34秒
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

class COneCowPay : public CPayBase
{
public:
    COneCowPay();
    virtual ~COneCowPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doGetSign);
    ENTRY_FUNC_DEFINE(doDepositCB);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
};