/*
 * =====================================================================================
 *
 *       Filename:  DummyStub.h
 *
 *    Description:  测试支付中心的插件
 *
 *        Version:  1.0
 *        Created:  2021年03月06日 14时44分39秒
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

class CPluginDummyStub : public CPayBase
{
public:
    CPluginDummyStub();
    virtual ~CPluginDummyStub();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

};


