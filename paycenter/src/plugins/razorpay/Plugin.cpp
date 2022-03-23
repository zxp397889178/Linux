/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  Razorpay (https://razorpay.com/docs)
 *                  主要用于与 uplive 合作印度 Rummy 游戏的支付之用
 *        Version:  1.0
 *        Created:  2020年09月01日 11时41分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "RazorPay.h"

CPluginRazorPay *g_pRazorPay;
PLUGIN_SO_FINI(g_pRazorPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pRazorPay = new CPluginRazorPay();
        if (NULL  == g_pRazorPay)
        {
            return -1000;
        }

        ret = g_pRazorPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pRazorPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "razor/pay",       "doPay");
    REG_CGI(vecEntry, "razor/withdraw",  "doWithdraw" );
    REG_CGI(vecEntry, "razor/kyc",       "doKYC" );
    REG_CGI(vecEntry, "razor/bindcard",  "doBindCard" );
    REG_CGI(vecEntry, "razor/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pRazorPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pRazorPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pRazorPay, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pRazorPay, doKYC)
ENTRY_FUNC_IMPLEMENT(g_pRazorPay, doBindCard)
ENTRY_FUNC_IMPLEMENT(g_pRazorPay, doWithdraw)


