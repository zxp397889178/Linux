/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  搜狗支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 17时15分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Common.h"
#include "SogouPay.h"

CSogouPay *g_pSogouPay;
PLUGIN_SO_FINI(g_pSogouPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pSogouPay = new CSogouPay();
        if (NULL  == g_pSogouPay)
        {
            return -1000;
        }

        ret = g_pSogouPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pSogouPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "sogou/pay",        "doDeposit");
    REG_CGI(vecEntry, "sogou/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pSogouPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pSogouPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pSogouPay, doDepositCB)
