/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  https://238.58paypay.com
 *
 *        Version:  1.0
 *        Created:  2020年08月06日 16时05分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "Pay58.h"

CPluginPay58 *g_pPay58;
PLUGIN_SO_FINI(g_pPay58);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pPay58 = new CPluginPay58();
        if (NULL  == g_pPay58)
        {
            return -1000;
        }

        ret = g_pPay58->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pPay58->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "pay58/pay",       "doPay");
    REG_CGI(vecEntry, "pay58/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pPay58->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pPay58, doPay)
ENTRY_FUNC_IMPLEMENT(g_pPay58, doCallback)







