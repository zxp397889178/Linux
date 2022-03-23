/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  小米支付
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
#include "XiaomiPay.h"

CXiaomiPay *g_pXiaomiPay;
PLUGIN_SO_FINI(g_pXiaomiPay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pXiaomiPay = new CXiaomiPay();
        if (NULL == g_pXiaomiPay)
        {
            return -1000;
        }
        return g_pXiaomiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pXiaomiPay->Update();
    }
    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "xiaomi/pay",              "doDeposit");
    REG_CGI(vecEntry, "xiaomi/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pXiaomiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pXiaomiPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pXiaomiPay, doDepositCB)
