/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  lion （mars 104）支付接口
 *
 *        Version:  1.0
 *        Created:  12/07/2019 09:23:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "LionPay.h"

CLionPay  *g_pLionPay = NULL;
PLUGIN_SO_FINI(g_pLionPay)

extern "C" int32_t doInit(int32_t style)
{
    if (style == init_pluginInit)
    {
        SAFE_DELETE(g_pLionPay);
        g_pLionPay = new CLionPay();
        if (NULL == g_pLionPay)
        {
            return -1000;
        }
    }
    return g_pLionPay->Init();
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "lionpay", "doLionPay");
    REG_CGI(vecEntry, "lionpay_cb", "doLionPayCallback");
    REG_CGI(vecEntry, "lionpay_qry", "doLionPayQuery");
    REG_CGI(vecEntry, "lionpay_echo", "doLionPayEcho");
}

extern "C" void run(void)
{
    g_pLionPay->Maintance();

}

ENTRY_FUNC_IMPLEMENT(g_pLionPay, doLionPay)
ENTRY_FUNC_IMPLEMENT(g_pLionPay, doLionPayCallback)
ENTRY_FUNC_IMPLEMENT(g_pLionPay, doLionPayQuery)
ENTRY_FUNC_IMPLEMENT(g_pLionPay, doLionPayEcho)
    
