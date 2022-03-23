/*
 * =====================================================================================
 *
 *       Filename:  XHPay.h
 *
 *    Description:  xhpay declare
 *
 *        Version:  1.0
 *        Created:  2020年08月08日 17时32分39秒
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

class CPluginXHPay : public CPayWrapper
{
public:
    CPluginXHPay();
    virtual ~CPluginXHPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CGame *pGame);
};




