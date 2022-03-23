/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  信付通支付SDK （http://newmer.xfuoo.com)
 *
 *        Version:  1.0
 *        Created:  2020年07月17日 21时44分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "XinfutongPay.h"

CPluginXinfutongPay *g_pXinfutongPay;
PLUGIN_SO_FINI(g_pXinfutongPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pXinfutongPay = new CPluginXinfutongPay();
        if (NULL  == g_pXinfutongPay)
        {
            return -1000;
        }

        ret = g_pXinfutongPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pXinfutongPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "xinfutong/pay",       "doPay");
    REG_CGI(vecEntry, "xinfutong/callback",  "doCallback");
    //REG_CGI(vecEntry, "xinfutong/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pXinfutongPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pXinfutongPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pXinfutongPay, doCallback)
//ENTRY_FUNC_IMPLEMENT(g_pXinfutongPay, doGetHtml)

