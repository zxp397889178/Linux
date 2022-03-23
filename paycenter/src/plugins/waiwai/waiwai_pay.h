/*
 * =====================================================================================
 *
 *       Filename:  waiwai_pay.h
 *
 *    Description:  歪歪支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年05月28日 22时24分43秒
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

class CPluginYaiyaiPay : public CPayWrapper
{
public:
    CPluginYaiyaiPay();
    virtual ~CPluginYaiyaiPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);

private:
    int32_t OnVerifyOrder(const CMyString strOrderNo);
    int32_t dealVerifyJsonResp(CMyString &strResp);

    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo);
};




