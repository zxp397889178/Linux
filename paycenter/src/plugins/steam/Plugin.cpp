/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  Steam平台充值
 *
 *        Version:  1.0
 *        Created:  2022年01月15日 11时17分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "SteamPay.h"

CSteamPay *g_pSteamPay;
PLUGIN_SO_FINI(g_pSteamPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pSteamPay = new CSteamPay();
        if (NULL  == g_pSteamPay)
        {
            return -1000;
        }

        ret = g_pSteamPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pSteamPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "steam/pay",        "doDeposit");
    REG_CGI(vecEntry, "steam/verify",     "doVerify");
    REG_CGI(vecEntry, "steam/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pSteamPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pSteamPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pSteamPay, doVerify)
ENTRY_FUNC_IMPLEMENT(g_pSteamPay, doDepositCB)
