/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  华为支付SDK
 *
 *        Version:  1.0
 *        Created:  2021年04月28日 19时39分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "HuaweiPayEx.h"

CPluginHuaweiPayEx *g_pHuaweiPay;
PLUGIN_SO_FINI(g_pHuaweiPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pHuaweiPay = new CPluginHuaweiPayEx();
        if (NULL  == g_pHuaweiPay)
        {
            return -1000;
        }

        ret = g_pHuaweiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pHuaweiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "huawei/pay",        "doDeposit");
    REG_CGI(vecEntry, "huawei/verify",     "doVerify");
    REG_CGI(vecEntry, "huawei/verify_v2",   "doVerifyV2");
}

extern "C" void run(void)
{
    g_pHuaweiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pHuaweiPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pHuaweiPay, doVerify)
ENTRY_FUNC_IMPLEMENT(g_pHuaweiPay, doVerifyV2)

