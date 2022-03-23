/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  一牛H5支付
 *
 *        Version:  1.0
 *        Created:  2021年12月24日 16时30分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "OneCow.h"


COneCowPay *g_pOneCowPay;
PLUGIN_SO_FINI(g_pOneCowPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pOneCowPay = new COneCowPay();
        if (NULL  == g_pOneCowPay)
        {
            return -1000;
        }

        ret = g_pOneCowPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pOneCowPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "onecow/pay",        "doDeposit");
    REG_CGI(vecEntry, "onecow/sign",       "doGetSign");
    REG_CGI(vecEntry, "onecow/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pOneCowPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pOneCowPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pOneCowPay, doGetSign)
ENTRY_FUNC_IMPLEMENT(g_pOneCowPay, doDepositCB)