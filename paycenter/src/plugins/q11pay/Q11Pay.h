/*
 * =====================================================================================
 *
 *       Filename:  Q11Pay.h
 *
 *    Description:  Q11 主支付定义
 *
 *        Version:  1.0
 *        Created:  2021年07月02日 16时43分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#pragma once

#include "../PayBase.h"
#include "IWebPlugin.h"

class CPluginQ11Pay : public CPayBase
{
public:
    CPluginQ11Pay();
    virtual ~CPluginQ11Pay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCB);
    
protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
};