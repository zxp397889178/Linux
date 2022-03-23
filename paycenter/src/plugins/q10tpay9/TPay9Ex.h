/*
 * =====================================================================================
 *
 *       Filename:  TPay9.h
 *
 *    Description:  tpay9 支付接口实现
 *
 *        Version:  1.0
 *        Created:  2021年06月05日 16时27分11秒
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

class CPluginTPay9Ex : public CPayBase
{
public:
    CPluginTPay9Ex();
    virtual ~CPluginTPay9Ex();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

private:
    bool GetSecret(CMyString& strOrderNo, CMyString& strSecret);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCB);
    
protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
};





