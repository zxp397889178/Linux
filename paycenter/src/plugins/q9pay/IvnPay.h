/*
 * =====================================================================================
 *
 *       Filename:  IvnPay.h
 *
 *    Description:  Q9 IVNPay 
 *
 *        Version:  1.0
 *        Created:  2021年06月17日 23时03分25秒
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

class CPluginIvnPay : public CPayBase
{
public:
    CPluginIvnPay();
    virtual ~CPluginIvnPay();

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
    PAYBASE_DEPOSITQRY_FUNCTION_DECLARE
    PAYBASE_CREDITQRY_FUNCTION_DECLARE
};



