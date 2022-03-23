/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  灵猴游戏SDK支付
 *
 *        Version:  1.0
 *        Created:  2020年03月27日 09时44分51秒
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
#include "MonkeyPay.h"

CPluginMonkeyPay   *g_pMonkeyPay;
PLUGIN_SO_FINI(g_pMonkeyPay)

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pMonkeyPay = new CPluginMonkeyPay();
        if (NULL  == g_pMonkeyPay)
        {
            return -1000;
        }

        ret = g_pMonkeyPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pMonkeyPay->Update();
    }
    else
    {
        LogFatal("[%s:%s:%d]unknown init style %d", __FILE__, __func__, __LINE__, style);
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "monkey/pay", "doPay");
    REG_CGI(vecEntry, "monkey/callback", "doCallback");
}

extern "C" void run(void)
{
    g_pMonkeyPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pMonkeyPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pMonkeyPay, doCallback)


