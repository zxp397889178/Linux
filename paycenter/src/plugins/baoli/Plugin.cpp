/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  宝丽支付 (http://www.baolipay.cn/agent_Index_index.html)
 *
 *        Version:  1.0
 *        Created:  2020年02月02日 15时09分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "baoli_pay.h"
#include "IWebPlugin.h"
#include "IWebClientMgr.h"
#include "IClient.h"

CPluginBaoliPay *g_pBaoliPay;
PLUGIN_SO_FINI(g_pBaoliPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pBaoliPay = new CPluginBaoliPay();
        if (NULL  == g_pBaoliPay)
        {
            return -1000;
        }

        ret = g_pBaoliPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pBaoliPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "baoli/pay", "doBaoliPay");
    REG_CGI(vecEntry, "baoli/callback", "doBaoliCallback");
    REG_CGI(vecEntry, "baoli/html", "doBaoliHtml");
}

extern "C" void run(void)
{
    g_pBaoliPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pBaoliPay, doBaoliPay)
ENTRY_FUNC_IMPLEMENT(g_pBaoliPay, doBaoliCallback)
ENTRY_FUNC_IMPLEMENT(g_pBaoliPay, doBaoliHtml)


