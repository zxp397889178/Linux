/*
 * =====================================================================================
 *
 *       Filename:  Pay77.h
 *
 *    Description:  pay77 支付类定义
 *
 *        Version:  1.0
 *        Created:  2021年01月08日 15时13分32秒
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

#include <map>

class CPluginVnPay : public CPayWrapper
{
public:
    CPluginVnPay();
    virtual ~CPluginVnPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

    int32_t load_key_file(CMyString& strFile, CMyString &val);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);
 //   ENTRY_FUNC_DEFINE(doWithdraw);
 //   ENTRY_FUNC_DEFINE(doWithdrawCallback);

private:
/*    
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CSdkChannel *pChannel, int32_t payMoney);
*/
    int32_t dealResp(http::CHttpRespone &http_resp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo, 
            CSdkChannel *pChannel, int32_t payMoney);

private:
    CMyString   m_pubKey;
    CMyString   m_priKey;

    void AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap, const char* key, const char* value);
    void AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap, const char*, int64_t val);
    void AddPara(http::CHttpRespMaker& resp, std::map<CMyString, CMyString>& sortMap, const char*, CMyString& val);

    CMyString GetSignSrc(std::map<CMyString, CMyString>& sortMap);
};




