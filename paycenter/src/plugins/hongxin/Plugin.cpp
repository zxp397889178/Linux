/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  鸿鑫支付SDK，用于383项目，狮子会
 *
 *        Version:  1.0
 *        Created:  2020年03月27日 18时20分42秒
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
#include "HongxinPay.h"

CPluginHongxinPay   *g_pHongxinPay;
PLUGIN_SO_FINI(g_pHongxinPay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pHongxinPay = new CPluginHongxinPay();
        if (NULL  == g_pHongxinPay)
        {
            return -1000;
        }

        ret = g_pHongxinPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pHongxinPay->Update();
    }
    else
    {
        LogFatal("[%s:%s:%d]unknown init style %d", __FILE__, __func__, __LINE__, style);
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "hongxin/pay", "doPay");
    REG_CGI(vecEntry, "hongxin/callback", "doCallback");
}

extern "C" void run(void)
{
    g_pHongxinPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pHongxinPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pHongxinPay, doCallback)




