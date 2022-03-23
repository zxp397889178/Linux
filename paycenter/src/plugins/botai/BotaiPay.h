/*
 * =====================================================================================
 *
 *       Filename:  BotaiPay.h
 *
 *    Description:  博泰支付接口实义
 *
 *        Version:  1.0
 *        Created:  07/04/2020 11:00:54 PM
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

class CPluginBotaiPay : public CPayWrapper
{
public:
    CPluginBotaiPay();
    virtual ~CPluginBotaiPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);

    int32_t VerifyOrder(const CMyString strOrderNo, const CMyString strPlatformId);

private:
    CMyString   m_strVerifyUrl;
};

