/*
 * =====================================================================================
 *
 *       Filename:  zhpay.h
 *
 *    Description:  综合支付接口
 *
 *        Version:  1.0
 *        Created:  2019年11月14日 22时50分23秒
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

class CPluginZhpay : public CPayWrapper
{
public:
    CPluginZhpay();
    virtual ~CPluginZhpay();

    int32_t Init(void);
    int32_t Update(void);

    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doZhPay)
    ENTRY_FUNC_DEFINE(doZhCallback)

};


