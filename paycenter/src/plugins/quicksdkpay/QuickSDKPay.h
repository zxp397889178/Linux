/*
 * =====================================================================================
 *
 *       Filename:  QuickSDKPay.h
 *
 *    Description:  Quicksdk 支付- 安卓用
 *
 *        Version:  1.0
 *        Created:  2021年08月31日 11时36分12秒
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

class CQuickSDKPay : public CPayBase
{
public:
    CQuickSDKPay();
    virtual ~CQuickSDKPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doDepositCB_hy);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

private:
    CMyString m_strCallbackKey;
    map<CMyString, CMyString> m_mapCallbackKey;
};
