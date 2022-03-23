/*
 * =====================================================================================
 *
 *       Filename:  alipay.h
 *
 *    Description:  支付宝支付响应
 *
 *        Version:  1.0
 *        Created:  2019年09月21日 14时05分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBAPP_PLUGIN_ALIPAY_H__
#define __WEBAPP_PLUGIN_ALIPAY_H__

#include "../PayBase.h"
#include "IWebPlugin.h"
#include <map>

class CPluginAlipay : public CPayBase
{
public:
    CPluginAlipay();
    ~CPluginAlipay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit)
    ENTRY_FUNC_DEFINE(doDepositCB)

private:
    int32_t rsaSign(const uint8_t *input, size_t cbInput, const CMyString &strPriKey, const CMyString strType, CMyString &strCipher);
    bool rsaVerify(const uint8_t *input, size_t cbInput, const CMyString &strSign, const CMyString &strPubKey, const CMyString strType);

private:
    int32_t load_key_file(CXmlFile &xml, const CMyString &strApp, const CMyString &strKey, CMyString &val);
    int32_t load_key_file(const CMyString &strFile, CMyString &val);
    int32_t sign(const CMyString &url_param, const CMyString &strType, const CMyString& strAppId, CMyString &sign);
    bool    signVerify(const CMyString &url_param, const CMyString &strType, const CMyString& strAppId, CMyString &sign);

    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

private:
    CMyString   m_strCancelPayUrl;
    map<CMyString, CMyString> m_mapPriKey;
    map<CMyString, CMyString> m_mapPubKey;
};


#endif // __WEBAPP_PLUGIN_ALIPAY_H__
