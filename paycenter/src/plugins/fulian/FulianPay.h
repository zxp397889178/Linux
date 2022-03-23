/*
 * =====================================================================================
 *
 *       Filename:  FulianPay.h
 *
 *    Description:  富连支付
 *
 *        Version:  1.0
 *        Created:  07/04/2020 10:08:42 PM
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

class CPluginFulianPay : public CPayWrapper
{
public:
    CPluginFulianPay();
    virtual ~CPluginFulianPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CSdkChannel *pChannel, int32_t payMoney);
};







