/*
 * =====================================================================================
 *
 *       Filename:  DotWalletPay.h
 *
 *    Description:  数字钱包支付
 *
 *        Version:  1.0
 *        Created:  2021年11月30日 19时16分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#pragma once
#include "../PayBase.h"
#include "IWebPlugin.h"

class CDotWalletPay : public CPayBase
{
public:
    CDotWalletPay();
    virtual ~CDotWalletPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCB);
    ENTRY_FUNC_DEFINE(doGetRate);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
    int32_t doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);
    int32_t doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);
    bool delayedDealDepositCallback(http::CHttpRequest& req, const CMyString& orderNo);

    void addCreditVerifyNeededParam(std::vector<CMyString> &vecParam);
    void storeCreditVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data, Json::Value &resp_data);
    int32_t verifyCreditRequest(Json::Value &req_data, bool &creditOK, CMyString &creditOrderNo, CMyString& errNote);

    int32_t doDepositAutoPay(IClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::map<CMyString, CMyString> &ext_headers);

private:
    void    GetRate();
    int64_t GetRMBTransCoin(int64_t nRMB, const CMyString& strCoinType);  //人民币单位为分
    int64_t GetCoinTransRMB(int64_t nCoin, const CMyString& strCoinType); //人民币单位为分

    CMyString GetAccessToken(CMyString& strClientId, CMyString& strClientSecret);
    CMyString GetUserReceiveAddress(const CMyString& strAccessToken, const CMyString& strCoinType, const CMyString& strUserId);
    bool GetUserAutoPayBalance(const CMyString& strCoinType, const CMyString& strUserId, int64_t& nBalance); //true 有授权自动支付，FALSE 没授权
    bool GetOrderIsPay(const CMyString& strPlatformOrder);

protected:
    //充值自动支付
    int32_t doRechargeAutoPay(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, const CMyString &orderNo, CMyString& platformOrderNo, int64_t& platformCode);
    void dealDepositRespBack(void);

    //充值轮询订单
    void dealUnFinishOrder();
    //轮询已充值，发货
    void deliveGoods(const CMyString& orderNo, const CMyString& platformOrderNo);

    //兑出相关函数
    int32_t doWithdrawRequest(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers);
    int32_t doWithdrawAudit(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers);
    int32_t doWithdrawCredit(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers);
    void getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field);
    int32_t valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame);
    void postNextCredit(const CMyString &gameCode, const CMyString &sdkCode, const CMyString &orderNo, http::CHttpRequest &req, char *szResp, uint32_t &cbResp);
    
    int32_t requestDotWalletAutoPay(Json::Value &req_data, bool &creditOK, CMyString &creditOrderNo, CMyString& errNote);

private:
    map<CMyString, CMyString> m_mapAddress;
    CMyString m_strAuthUrl;
    CMyString m_strClientId;
    CMyString m_strClientSecret;
    CMyString m_strUserAddressUrl;
    CMyString m_strPayUrl;
    CMyString m_strTransUrl;
    CMyString m_strGetBalanceUrl;
    CMyString m_strQueryOrderUrl;
    CMyString m_strAutoPayTransferUrl; //10180007 表示用户的自动支付余额不足 引导用户跳转到打点自动支付划转页面进行充值
    CMyString m_strAutoPayConfigUrl; //10180029 表示订单支付金额大于用户的授权金额 引导用户跳转到打点自动支付单笔限额设置页面进行限额设置
    CMyString m_strAccessToken;
    int64_t   m_nExpiresIn;
    bool      m_bDepositAutoPay;
    
    CMyString m_strDepositKeyFormat;
    CMyString m_strCreditKeyFormat;
    int32_t   m_nTimeout;

    CMyTimer  m_timerRefershRate;  // 定时检查发货缓存
    Json::Value m_jsRate;

    //充值-自动支付回调
    redis_cache::CFmtCache  m_cacheDepositRespBack;     // 充值请求时缓存KEY
    std::queue<CMyString>   m_qDepositRespBackParam;

    //充值查询订单
    redis_cache::CFmtCache  m_cacheQueryOrderNum;
    redis_cache::CIncrCache m_cacheQueryOrderData;
    CMyString               m_strCurrentOrderNo;
    CMyTimer                m_timerQueryOrder;

    //兑出
    redis_cache::CFmtCache  m_cacheCreditReq;
    redis_cache::CIncrCache m_cacheCreditOrder;
};
