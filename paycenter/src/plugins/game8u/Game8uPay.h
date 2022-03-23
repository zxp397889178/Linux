/*
 * =====================================================================================
 *
 *       Filename:  Game8uPay.h
 *
 *    Description:  8U游戏平台支付-用于天趣H5支付
 *
 *        Version:  1.0
 *        Created:  2021年11月25日 14时42分23秒
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

class CGame8UPay : public CPayBase
{
public:
    CGame8UPay();
    virtual ~CGame8UPay();

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
