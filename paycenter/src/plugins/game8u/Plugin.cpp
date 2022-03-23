/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  8U游戏平台支付-用于天趣H5支付
 *
 *        Version:  1.0
 *        Created:  2021年11月25日 14时42分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */




#include "Common.h"
#include "Game8uPay.h"

CGame8UPay *g_pGame8uPay = NULL;
PLUGIN_SO_FINI(g_pGame8uPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pGame8uPay = new CGame8UPay();
        if (NULL  == g_pGame8uPay)
        {
            return -1000;
        }

        ret = g_pGame8uPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pGame8uPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "game8upay/pay",        "doDeposit");
    REG_CGI(vecEntry, "game8upay/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pGame8uPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pGame8uPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pGame8uPay, doDepositCB)

