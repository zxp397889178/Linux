/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  用于支付中心的测试渠道，没有真实金额发生
 *
 *        Version:  1.0
 *        Created:  2021年03月06日 14时41分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "DummyStub.h"

CPluginDummyStub *g_pDummyStub;
PLUGIN_SO_FINI(g_pDummyStub);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pDummyStub = new CPluginDummyStub();
        if (NULL  == g_pDummyStub)
        {
            return -1000;
        }

        ret = g_pDummyStub->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pDummyStub->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "dummy/pay",              "doDeposit");
    REG_CGI(vecEntry, "dummy/callback",         "doDepositCB");
}

extern "C" void run(void)
{
    g_pDummyStub->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pDummyStub, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pDummyStub, doDepositCB)


