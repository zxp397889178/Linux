/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  百度 多酷联运SDK 单机 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 15时36分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "BaiduPay.h"


CBaiduPay *g_pBaiduPay;
PLUGIN_SO_FINI(g_pBaiduPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pBaiduPay = new CBaiduPay();
        if (NULL  == g_pBaiduPay)
        {
            return -1000;
        }

        ret = g_pBaiduPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pBaiduPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "baidu/pay",        "doDeposit");
    REG_CGI(vecEntry, "baidu/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pBaiduPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pBaiduPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pBaiduPay, doDepositCB)

