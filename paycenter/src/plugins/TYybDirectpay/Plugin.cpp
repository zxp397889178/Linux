/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  腾讯应用宝直购模式
 *
 *        Version:  1.0
 *        Created:  2021年07月15日 17时55分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "TYybDirectpay.h"

CPluginTDPay    *g_pTDPay;
PLUGIN_SO_FINI(g_pTDPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTDPay = new CPluginTDPay();
        if (NULL == g_pTDPay)
        {
            return -1000;
        }
        return g_pTDPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pTDPay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tdpay/pay",              "doDeposit");
    REG_CGI(vecEntry, "tdpay/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pTDPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTDPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pTDPay, doDepositCB)
