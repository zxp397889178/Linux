/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  众币支付接口
 *
 *        Version:  1.0
 *        Created:  03/08/2020 06:50:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "IWebPlugin.h"
#include "IWebClientMgr.h"
#include "IClient.h"
#include "ZhongBiPay.h"

CPluginZhongBiPay   *g_pZhongbiPay;
PLUGIN_SO_FINI(g_pZhongbiPay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pZhongbiPay = new CPluginZhongBiPay();
        if (NULL  == g_pZhongbiPay)
        {
            return -1000;
        }

        ret = g_pZhongbiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pZhongbiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "zhongbi/pay", "doPay");
    REG_CGI(vecEntry, "zhongbi/callback", "doCallback");
}

extern "C" void run(void)
{
    g_pZhongbiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pZhongbiPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pZhongbiPay, doCallback)

