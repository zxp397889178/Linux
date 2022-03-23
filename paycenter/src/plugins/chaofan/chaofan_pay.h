/*
 * =====================================================================================
 *
 *       Filename:  chaofan_pay.h
 *
 *    Description:  超凡支付平台API
 *
 *        Version:  1.0
 *        Created:  03/13/2020 07:21:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "../PayWrapper.h"
#include "IWebPlugin.h"
#include "pay_error.h"

class CPluginChaofanPay : public CPayWrapper
{
public:
    CPluginChaofanPay();
    virtual ~CPluginChaofanPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);
    ENTRY_FUNC_DEFINE(doGetHtml);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError);
    CMyString getClientIp(http::CHttpRequest &req, IClient *pClient);
};

