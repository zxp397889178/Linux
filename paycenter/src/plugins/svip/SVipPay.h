/*
 * =====================================================================================
 *
 *       Filename:  SVipPay.h
 *
 *    Description:  SVIP 支付接口
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 15时01分19秒
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

class CPluginSVipPay : public CPayWrapper
{
public:
    CPluginSVipPay();
    virtual ~CPluginSVipPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);
    ENTRY_FUNC_DEFINE(doGetHtml);

private:
    //int32_t OnVerifyOrder(const CMyString strOrderNo);
    //int32_t dealVerifyJsonResp(CMyString &strResp);

    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);

    int32_t dealHtmlResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, CMyString &strUrl);
    void dealRelativePath(CMyString &strData, const CMyString &strUrl);



private:
    CMyString   m_strTransfer;
};



