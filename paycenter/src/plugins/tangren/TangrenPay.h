/*
 * =====================================================================================
 *
 *       Filename:  TangrenPay.h
 *
 *    Description:  唐人支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年04月23日 18时27分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"
#include "MyDB.h"
#include "IWebPlugin.h"
#include "IClient.h"
#include "IWebClientMgr.h"
#include "../PayWrapper.h"

class CPluginTangrenPay : public CPayWrapper
{
public:
    CPluginTangrenPay();
    virtual ~CPluginTangrenPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(http::CHttpRespone &resp, const CMyString &strResp, 
            CMyString &strError, CMyString &platformOrderNo);

    //int32_t queryOrder(const CMyString &strOrderNo);
};

