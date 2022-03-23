/*
 * =====================================================================================
 *
 *       Filename:  richcw.h
 *
 *    Description:  极速支付的SDK
 *
 *        Version:  1.0
 *        Created:  12/30/2019 04:50:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "../PayWrapper.h"
#include "IWebPlugin.h"

class CPluginRichcw : public CPayWrapper
{
public:
    CPluginRichcw();
    ~CPluginRichcw();

    int32_t Init(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doRichcwPay);

};


