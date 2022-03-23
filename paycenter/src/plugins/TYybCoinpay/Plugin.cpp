/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  应用宝游戏币模式
 *
 *        Version:  1.0
 *        Created:  2021年07月19日 16时27分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Common.h"
#include "TYybCoinpay.h"

CPluginTCPay    *g_pTCPay;
PLUGIN_SO_FINI(g_pTCPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTCPay = new CPluginTCPay();
        if (NULL == g_pTCPay)
        {
            return -1000;
        }
        return g_pTCPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pTCPay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tcpay/pay",              "doDeposit");
    REG_CGI(vecEntry, "tcpay/consume",          "doConsumeCoin");
}

extern "C" void run(void)
{
    g_pTCPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTCPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pTCPay, doConsumeCoin)
