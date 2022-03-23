/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  客服放款
 *
 *        Version:  1.0
 *        Created:  2021年07月06日 17时39分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "Common.h"
#include "GMPay.h"

CPluginGMPay    *g_pGMPay;
PLUGIN_SO_FINI(g_pGMPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pGMPay = new CPluginGMPay();
        if (NULL == g_pGMPay)
        {
            return -1000;
        }
        return g_pGMPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pGMPay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "gmpay/pay",              "doDeposit");
    REG_CGI(vecEntry, "gmpay/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pGMPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pGMPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pGMPay, doDepositCB)
