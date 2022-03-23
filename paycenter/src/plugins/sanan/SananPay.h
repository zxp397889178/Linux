/*
 * =====================================================================================
 *
 *       Filename:  SananPay.h
 *
 *    Description:  三安支付
 *
 *        Version:  1.0
 *        Created:  2020年07月03日 14时19分35秒
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

class CPluginSananPay : public CPayWrapper
{
public:
    CPluginSananPay();
    virtual ~CPluginSananPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};

