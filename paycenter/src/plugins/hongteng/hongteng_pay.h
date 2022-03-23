/*
 * =====================================================================================
 *
 *       Filename:  hongteng_pay.h
 *
 *    Description:  鸿腾支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年05月28日 21时25分26秒
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

class CPluginHongTengPay : public CPayWrapper
{
public:
    CPluginHongTengPay();
    virtual ~CPluginHongTengPay();

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



