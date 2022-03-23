/*
 * =====================================================================================
 *
 *       Filename:  TYybDirectpay.h
 *
 *    Description:  腾讯应用宝直购模式
 *
 *        Version:  1.0
 *        Created:  2021年07月15日 17时56分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "../PayBase.h"
#include "IWebPlugin.h"

class CPluginTDPay : public CPayBase
{
public:
    CPluginTDPay();
    virtual ~CPluginTDPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    
protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

private:
    CMyString GetSign(CMyString& strUrl, CMyString& strAppKey, CMyString& strParam);
    bool fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data);

};
