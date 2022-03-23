/*
 * =====================================================================================
 *
 *       Filename:  XinfutongPay.h
 *
 *    Description:  信付通支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年07月17日 21时47分16秒
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

class CPluginXinfutongPay : public CPayWrapper
{
public:
    CPluginXinfutongPay();
    virtual ~CPluginXinfutongPay();

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





