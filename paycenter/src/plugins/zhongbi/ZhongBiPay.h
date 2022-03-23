/*
 * =====================================================================================
 *
 *       Filename:  ZhongBiPay.h
 *
 *    Description:  众币支付接口
 *
 *        Version:  1.0
 *        Created:  03/08/2020 07:01:18 PM
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

class CPluginZhongBiPay : public CPayWrapper
{
public:
    CPluginZhongBiPay();
    virtual ~CPluginZhongBiPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(http::CHttpRespone &resp, const CMyString &strResp, 
            CMyString &strError, CMyString &platformOrderNo);

};


