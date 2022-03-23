/*
 * =====================================================================================
 *
 *       Filename:  XiaomiPay.h
 *
 *    Description:  小米支付
 *
 *        Version:  1.0
 *        Created:  2021年09月26日 19时42分22秒
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

class CXiaomiPay : public CPayBase
{
public:
    CXiaomiPay();
    virtual ~CXiaomiPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);

private:
    CMyString decodeParam(const CMyString& strUrl);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
};
