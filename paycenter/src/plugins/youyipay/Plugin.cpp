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
#include "YouyiPay.h"

CYouyiPay *g_pYouyiPay;
PLUGIN_SO_FINI(g_pYouyiPay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pYouyiPay = new CYouyiPay();
        if (NULL == g_pYouyiPay)
        {
            return -1000;
        }
        return g_pYouyiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pYouyiPay->Update();
    }
    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "youyipay/pay",              "doDeposit");
    REG_CGI(vecEntry, "youyipay/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pYouyiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pYouyiPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pYouyiPay, doDepositCB)
