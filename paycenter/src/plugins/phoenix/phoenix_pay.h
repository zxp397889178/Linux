/*
 * =====================================================================================
 *
 *       Filename:  phoenix_pay.h
 *
 *    Description:  凤凰支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年05月29日 12时37分49秒
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

class CPluginPhoenixPay : public CPayWrapper
{
public:
    CPluginPhoenixPay();
    virtual ~CPluginPhoenixPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t OnVerifyOrder(const CMyString strOrderNo);
    int32_t dealVerifyJsonResp(CMyString &strResp);

#if 0
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
#else
    int32_t dealHttpResp(http::CHttpRespone &resp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
#endif
};


