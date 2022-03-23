/*
 * =====================================================================================
 *
 *       Filename:  Q8Pay.h
 *
 *    Description:  Q9 充值，PayNow支付
 *
 *        Version:  1.0
 *        Created:  2021年06月16日 00时11分43秒
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

class CPluginQ8Pay : public CPayBase
{
public:
    CPluginQ8Pay();
    virtual ~CPluginQ8Pay();

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


