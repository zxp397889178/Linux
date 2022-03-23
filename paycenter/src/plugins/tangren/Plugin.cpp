/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  唐人街棋牌支付SDK，跟老司机合作的
 *
 *        Version:  1.0
 *        Created:  2020年04月23日 18时01分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "TangrenPay.h"

CPluginTangrenPay *g_pTangrenPay;
PLUGIN_SO_FINI(g_pTangrenPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTangrenPay = new CPluginTangrenPay();
        if (NULL  == g_pTangrenPay)
        {
            return -1000;
        }

        ret = g_pTangrenPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pTangrenPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tangren/pay",        "doPay");
    REG_CGI(vecEntry, "tangren/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pTangrenPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTangrenPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pTangrenPay, doCallback)


