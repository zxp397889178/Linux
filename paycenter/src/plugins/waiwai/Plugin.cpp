/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  歪歪支付SDK
 *                 (http://api/yypay.zs51zc.cn)
 *        Version:  1.0
 *        Created:  2020年05月28日 22时21分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "waiwai_pay.h"

CPluginYaiyaiPay *g_pYaiyaiPay;
PLUGIN_SO_FINI(g_pYaiyaiPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pYaiyaiPay = new CPluginYaiyaiPay();
        if (NULL  == g_pYaiyaiPay)
        {
            return -1000;
        }

        ret = g_pYaiyaiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pYaiyaiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "waiwai/pay",        "doPay");
    REG_CGI(vecEntry, "waiwai/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pYaiyaiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pYaiyaiPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pYaiyaiPay, doCallback)



