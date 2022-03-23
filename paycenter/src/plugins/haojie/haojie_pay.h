/*
 * =====================================================================================
 *
 *       Filename:  haojie_pay.h
 *
 *    Description:  豪杰支付API接口
 *
 *        Version:  1.0
 *        Created:  03/15/2020 01:14:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "../PayWrapper.h"
#include "IWebPlugin.h"

class CPluginHaojiePay : public CPayWrapper
{
public:
    CPluginHaojiePay();
    virtual ~CPluginHaojiePay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};

