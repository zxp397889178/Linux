/*
 * =====================================================================================
 *
 *       Filename:  YoulePay.h
 *
 *    Description:  有乐支付接口
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 00时04分50秒
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

class CPluginYoulePay : public CPayWrapper
{
public:
    CPluginYoulePay();
    virtual ~CPluginYoulePay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    //int32_t OnVerifyOrder(const CMyString strOrderNo);
    //int32_t dealVerifyJsonResp(CMyString &strResp);

    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};


