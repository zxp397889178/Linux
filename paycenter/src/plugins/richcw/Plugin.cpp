/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  极速支付调用
 *
 *        Version:  1.0
 *        Created:  12/30/2019 04:38:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "richcw.h"

static CPluginRichcw *g_pRichCW;
PLUGIN_SO_FINI(g_pRichCW)

extern "C" int32_t doInit(int32_t style)
{
    if (style == init_pluginInit)
    {
        g_pRichCW = new CPluginRichcw();
        if (NULL == g_pRichCW)
        {
            return -1;
        }
    }
    return g_pRichCW->Init();
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "richcw/pay", "doRichcwPay");
}

extern "C" void run(void)
{
    g_pRichCW->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pRichCW, doRichcwPay);
