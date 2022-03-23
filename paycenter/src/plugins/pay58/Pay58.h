/*
 * =====================================================================================
 *
 *       Filename:  Pay58.h
 *
 *    Description:  58paypay定义,用于TeenPatti的充值
 *
 *        Version:  1.0
 *        Created:  2020年08月06日 16时07分27秒
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

class CPluginPay58 : public CPayWrapper
{
public:
    CPluginPay58();
    virtual ~CPluginPay58();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    void postAlter(CGame *pGame, const CMyString &strText);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CGame *pGame);
};




