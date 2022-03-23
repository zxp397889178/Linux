/*
 * =====================================================================================
 *
 *       Filename:  MonkeyPay.h
 *
 *    Description:  灵猴游戏SDK，用于捕鱼接入
 *
 *        Version:  1.0
 *        Created:  2020年03月27日 09时47分00秒
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

class CPluginMonkeyPay : public CPayWrapper
{
public:
    CPluginMonkeyPay();
    virtual ~CPluginMonkeyPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(http::CHttpRespone &resp, const CMyString &strResp, 
            CMyString &strError, CMyString &platformOrderNo);

    // int32_t queryOrder(const CMyString &strOrderNo);
};



