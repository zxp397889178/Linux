/*
 * =====================================================================================
 *
 *       Filename:  demo.h
 *
 *    Description:  demo for plugin
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 17时42分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_PLUGIN_DEMO_H__
#define __WEBFRAME_PLUGIN_DEMO_H__

#include "BaseCode.h"
#include "MyNet.h"
#include <typeinfo>
#include "../IWebPlugin.h"


class CPluginDemo
{
public:
    CPluginDemo();
    ~CPluginDemo();

public:
    void Maintance(void);

    ENTRY_FUNC_DEFINE(doEcho)
    ENTRY_FUNC_DEFINE(doServerInfo)

};

#endif // __WEBFRAME_PLUGIN_DEMO_H__
