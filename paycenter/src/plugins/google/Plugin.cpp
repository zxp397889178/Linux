/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  谷歌支付接口
 *
 *        Version:  1.0
 *        Created:  2020年10月21日 15时13分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#define PY_SSIZE_T_CLEAN
#include "Common.h"
#include "GooglePay.h"
#include <Python.h>

CPluginGooglePay *g_pGooglePay;
PLUGIN_SO_FINI(g_pGooglePay);

extern "C" int32_t doInit(int32_t style)
{
    if (!Py_IsInitialized())
    {
        Py_SetPythonHome(L"/opt/python3");
        Py_Initialize();
    }

    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pGooglePay = new CPluginGooglePay();
        if (NULL  == g_pGooglePay)
        {
            return -1000;
        }

        ret = g_pGooglePay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pGooglePay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "google/pay",       "doPay");
    REG_CGI(vecEntry, "google/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pGooglePay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pGooglePay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pGooglePay, doCallback)





