/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 18时28分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Common.h"
#include "ShunYouPay.h"

CShunYouPay *g_pShunYouPay;
PLUGIN_SO_FINI(g_pShunYouPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pShunYouPay = new CShunYouPay();
        if (NULL  == g_pShunYouPay)
        {
            return -1000;
        }

        ret = g_pShunYouPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pShunYouPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "shunyou/pay",        "doDeposit");
    REG_CGI(vecEntry, "shunyou/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pShunYouPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pShunYouPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pShunYouPay, doDepositCB)
