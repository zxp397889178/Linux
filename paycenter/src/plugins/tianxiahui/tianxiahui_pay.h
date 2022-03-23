/*
 * =====================================================================================
 *
 *       Filename:  tianxiahui_pay.h
 *
 *    Description:  天下汇支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年05月29日 13时10分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once
#include "../PayWrapper.h"
#include "IWebPlugin.h"

class CPluginTianxiahuiPay : public CPayWrapper
{
public:
    CPluginTianxiahuiPay();
    virtual ~CPluginTianxiahuiPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t OnVerifyOrder(const CMyString strOrderNo);
    int32_t dealVerifyJsonResp(CMyString &strResp);

    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};



