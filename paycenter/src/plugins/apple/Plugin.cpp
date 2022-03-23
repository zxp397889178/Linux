/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  苹果支付接口
 *
 *        Version:  1.0
 *        Created:  2021年04月09日 14时33分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "ApplePay.h"

CPluginApplePay *g_pApplePay;
PLUGIN_SO_FINI(g_pApplePay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pApplePay = new CPluginApplePay();
        if (NULL  == g_pApplePay)
        {
            return -1000;
        }

        ret = g_pApplePay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pApplePay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "apple/pay",              "doDeposit");
    REG_CGI(vecEntry, "apple/verify",           "doVerify");
}

extern "C" void run(void)
{
    g_pApplePay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pApplePay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pApplePay, doVerify)


