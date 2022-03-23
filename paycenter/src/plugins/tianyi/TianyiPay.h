/*
 * =====================================================================================
 *
 *       Filename:  TianyiPay.h
 *
 *    Description:  天逸支付API定义
 *
 *        Version:  1.0
 *        Created:  2020年07月18日 19时51分49秒
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

class CPluginTianyiPay : public CPayWrapper
{
public:
    CPluginTianyiPay();
    virtual ~CPluginTianyiPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doScanPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CSdkChannel *pChannel, int32_t payMoney);
};







