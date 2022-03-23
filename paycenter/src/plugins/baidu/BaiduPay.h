/*
 * =====================================================================================
 *
 *       Filename:  BaiduPay.h
 *
 *    Description:  百度 多酷联运SDK 单机 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 15时37分03秒
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

class CBaiduPay : public CPayBase
{
public:
    CBaiduPay();
    virtual ~CBaiduPay();

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

