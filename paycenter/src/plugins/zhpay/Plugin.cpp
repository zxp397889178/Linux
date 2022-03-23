/*
 * =====================================================================================
 *
 *       Filename:  plugin.cpp
 *
 *    Description:  综合支付接口
 *
 *        Version:  1.0
 *        Created:  2019年11月14日 22时47分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "zhpay.h"
#include "IWebPlugin.h"
#include "IWebClientMgr.h"
#include "IClient.h"

CPluginZhpay *g_pZhpayObj;
// PLUGIN_SO_INIT(CPluginZhpay)
PLUGIN_SO_FINI(g_pZhpayObj)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pZhpayObj = new CPluginZhpay();
        if (NULL == g_pZhpayObj)
        {
            return -1000;
        }
        ret = g_pZhpayObj->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pZhpayObj->Update();
    }
    
    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "zhpay/pay", "doZhPay");
    REG_CGI(vecEntry, "zhpay/callback", "doZhCallback");
}

extern "C" void run(void)
{
    g_pZhpayObj->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pZhpayObj, doZhPay)
ENTRY_FUNC_IMPLEMENT(g_pZhpayObj, doZhCallback)


