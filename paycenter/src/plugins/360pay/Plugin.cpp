/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  360支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月13日 17时58分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "360Pay.h"

C360Pay *g_p360Pay;
PLUGIN_SO_FINI(g_p360Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_p360Pay = new C360Pay();
        if (NULL  == g_p360Pay)
        {
            return -1000;
        }

        ret = g_p360Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_p360Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "360pay/pay",        "doDeposit");
    REG_CGI(vecEntry, "360pay/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_p360Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_p360Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_p360Pay, doDepositCB)
