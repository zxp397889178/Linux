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

class CPluginIvnPay : public CPayWrapper
{
public:
    enum CreditState
    {
        //creditNotFound = 0, // 无此订单号
        creditPending,      // 待处理
        creditPaied,        // 已支付
        creditFailed,       // 代付失败
        creditPaying,       // 付款中
        creditReject,       // 已驳回
        creditCorrupted,    // 订单异常
    };

public:
    CPluginIvnPay();
    virtual ~CPluginIvnPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doPay);
    ENTRY_FUNC_DEFINE(doCallback);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCallback);

private:
    int32_t QueryPayOrder(const CMyString &orderNo, Json::Value &resp);

private:
/*    
    int32_t dealJsonResp(CMyString &strResp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo,
            CSdkChannel *pChannel, int32_t payMoney);

    int32_t dealResp(http::CHttpRespone &http_resp, CMyString &strTransferUrl, 
            CMyString &strError, CMyString &platformOrderNo, 
            CSdkChannel *pChannel, int32_t payMoney);
*/
    int32_t ValidPayCallbackSign(http::CHttpRespMaker &req, const CMyString &strMd5Valid, const CMyString &strOrderNo, char *szResp, uint32_t &cbResp);
    int32_t dealCreditRequest(CSdk *pSdk, CGame *pGame, int64_t fee, const CMyString &bankCard, const CMyString &bankCode, const CMyString &bankName, const CMyString &uin, const CMyString &orderNo, char *szResp, uint32_t &cbResp);
    bool ValidIp(const CMyString &strIp, const CMyString &sect_enable, const CMyString &sect_ips);
    void getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field);
    int32_t valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame);
    int32_t CheckCreditOrder(CGame *pGame, const CMyString &strUrl, const CMyString &pid, const CMyString &uin, const CMyString &orderNo, const CMyString &key, CMyString &strError, CMyString &strResp, int32_t &state);
    int32_t dealCreditCheckResp(http::CHttpRespone &resp, const CMyString &strResp, CMyString &strError, int32_t &result);
    
    int32_t LogCreditStatus(const CMyString &orderNo, int32_t status, const CMyString &remark, int64_t creditTime, const CMyString &creditOp, char *szResp, uint32_t &cbResp);
    int32_t dealCreditSyncResp(CMyString &strResp, CMyString &strError, CMyString &platformOrderNo);
    int32_t dealCreditOrder(const CMyString &creditData);
    int32_t NotifyGamesvr(const CMyString &gameCode, const CMyString &uin, const CMyString &orderNo, int64_t fee, int32_t step, int32_t result, CGame *pGame);
    int32_t LogLoanStatus(const CMyString &orderNo, int32_t status, char *szResp, uint32_t &cbResp);
private:
    CMyTimer    m_timer;
};




