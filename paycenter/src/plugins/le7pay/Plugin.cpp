/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  乐7联运sdk支付
 *
 *        Version:  1.0
 *        Created:  2021年08月17日 10时40分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */




#include "Common.h"
#include "Le7Pay.h"

CLe7Pay *g_pLe7Pay;
PLUGIN_SO_FINI(g_pLe7Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pLe7Pay = new CLe7Pay();
        if (NULL  == g_pLe7Pay)
        {
            return -1000;
        }

        ret = g_pLe7Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pLe7Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "le7pay/pay",        "doDeposit");
    REG_CGI(vecEntry, "le7pay/sign",       "doGetSign");
    REG_CGI(vecEntry, "le7pay/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pLe7Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pLe7Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pLe7Pay, doGetSign)
ENTRY_FUNC_IMPLEMENT(g_pLe7Pay, doDepositCB)

