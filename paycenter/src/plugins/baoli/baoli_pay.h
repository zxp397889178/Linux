/*
 * =====================================================================================
 *
 *       Filename:  baoli_pay.h
 *
 *    Description:  宝丽支付的API
 *
 *        Version:  1.0
 *        Created:  2020年02月02日 15时14分54秒
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

class CPluginBaoliPay : public CPayWrapper
{
public:
    CPluginBaoliPay();
    virtual ~CPluginBaoliPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doBaoliPay);
    ENTRY_FUNC_DEFINE(doBaoliCallback);
    ENTRY_FUNC_DEFINE(doBaoliHtml);

private:

    int32_t dealJsonResp(CMyString &strResp, CMyString &strError);
    int32_t dealRelocation(http::CHttpRespone &respIn, CMyString &strResp, CMyString &strTransferUrl, CMyString &strError, const CMyString &strUrl);
    int32_t dealHtmlResp(CMyString &strResp, CMyString &strTransferUrl, CMyString &strError);

    void dealRelativePath(CMyString &strData, const CMyString &strUrl);
private:
    CMyString       m_strTransfer;
};
