/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  V8充值通道
 *
 *        Version:  1.0
 *        Created:  2020年08月05日 16时16分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "V8Pay.h"

CPluginV8Pay *g_pV8Pay;
PLUGIN_SO_FINI(g_pV8Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pV8Pay = new CPluginV8Pay();
        if (NULL  == g_pV8Pay)
        {
            return -1000;
        }

        ret = g_pV8Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pV8Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "v8/pay",       "doPay");
    REG_CGI(vecEntry, "v8/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pV8Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pV8Pay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pV8Pay, doCallback)





