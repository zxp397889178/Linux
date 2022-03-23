/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  支付宝支付
 *
 *        Version:  1.0
 *        Created:  2021年07月01日 10时53分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "Common.h"
#include "alipay.h"

CPluginAlipay *g_pAlipay;
PLUGIN_SO_FINI(g_pAlipay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pAlipay = new CPluginAlipay();
        if (NULL == g_pAlipay)
        {
            return -1000;
        }
        return g_pAlipay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pAlipay->Update();
    }
    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "alipay/pay",              "doDeposit");
    REG_CGI(vecEntry, "alipay/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pAlipay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pAlipay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pAlipay, doDepositCB)
