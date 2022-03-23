/*
 * =====================================================================================
 *
 *       Filename:  HuaweiPay.h
 *
 *    Description:  华为支付文档定义
 *
 *        Version:  1.0
 *        Created:  2021年04月28日 19时41分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once
#include "../PayBase.h"
#include "IWebPlugin.h"

class CPluginHuaweiPayEx : public CPayBase
{
     enum HUAWEIDepositStatus
    {
        deposit_CreateOrder       = 0,  // 1 - 创建订单
        deposit_MoneyArrival      = 1,  // 3 - 金额到账
        deposit_MoneyFail         = 2,  // 3 - 金额未到账
        deposit_UserCancel        = 3,  // 3 - 支付取消
        deposit_Refund            = 4,  // 5 - 退款（TODO)
        deposit_DeliverSucceed    = 5,  // 4 - 发货完成
        deposit_RequestSucceed    = 6,  // 2 - 请求成功
        deposit_RequestFail       = 7,  // 2 - 请求失败
        deposit_DeliverHostFail   = 8,  // 4 - 发货服务器失败 (REDO)
        deposit_DeliverFail       = 9,  // 4 - 发货失败（TODO）
        deposit_DeliverManual     = 10, // 5 - 发货失败，人工补单（TODO)
        deposit_UserPayTimeout    = 11, // 3 - 支付超时
    };

    enum HUAWEIGameDepositStatus
    {
        deliver_Success             = 4,    // 充值成功
        deliver_Failure             = 7,    // 充值失败，需要重发
        deliver_OrderFinished       = 11,   // 订单已完成，更改状态
        deliver_OrderNotExist       = 12,   // 订单不存在，更改状态
        deliver_SystemFault         = 13,   // 系统错误，需要重发
        deliver_FeeMotMatch         = 14,   // 订单金额不匹配，更改状态
    };

public:
    CPluginHuaweiPayEx();
    virtual ~CPluginHuaweiPayEx();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doVerify);
    ENTRY_FUNC_DEFINE(doVerifyV2);

private:
    int32_t buildAppAT(const CMyString &gameCode, int32_t channelId);
    CMyString getAppAT(const CMyString &gameCode, int32_t channelId, bool bForce = false);

private:
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

    void doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
            const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData);
    bool doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, 
            CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote);

    void addDepositVerifyNeededParam(std::vector<CMyString> &vecParam);
    void storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data);
    int32_t verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);
    
    int32_t doCerifyAndDeliverGoods(http::CHttpRequest &req, const CMyString& strOrderNo, CMyString& billNo, Json::Value& data);

private:
    bool validHuaWeiDepositSign(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);
    bool fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data);

    void OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp);
    void doPayFailure(char* szResp, uint32_t& cbResp);
    bool finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin);
    void getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo);
    void StrReplace(CMyString &str, const char *from, const char* to);
    void judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo);
    bool updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode);

private:
    typedef std::map<CMyString, CMyString>      MAP_GAME_APPAT;
    MAP_GAME_APPAT              m_mapGameAppAT;
    redis_cache::CIncrCache     m_cacheHuaweiOrder;
    int32_t                     m_errCode;
    CMyString                   m_errNote;
};











