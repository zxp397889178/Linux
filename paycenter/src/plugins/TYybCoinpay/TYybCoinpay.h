/*
 * =====================================================================================
 *
 *       Filename:  TYybCoinpay.h
 *
 *    Description:  应用宝游戏币模式
 *
 *        Version:  1.0
 *        Created:  2021年07月19日 16时26分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#pragma once

#include "../PayBase.h"
#include "IWebPlugin.h"

class CPluginTCPay : public CPayBase
{
    enum TCPayDepositStatus
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

    enum TCPayGameDepositStatus
    {
        deliver_Success             = 4,    // 充值成功
        deliver_Failure             = 7,    // 充值失败，需要重发
        deliver_OrderFinished       = 11,   // 订单已完成，更改状态
        deliver_OrderNotExist       = 12,   // 订单不存在，更改状态
        deliver_SystemFault         = 13,   // 系统错误，需要重发
        deliver_FeeMotMatch         = 14,   // 订单金额不匹配，更改状态
    };

public:
    CPluginTCPay();
    virtual ~CPluginTCPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doConsumeCoin);
    
protected:
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

    void doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
            const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData);
    bool doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, 
            CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote);

    void addDepositVerifyNeededParam(std::vector<CMyString> &vecParam);
    void storeDepositVerifyData(CGame *pGame, CSdk *pSdk, http::CHttpRequest &req, Json::Value &data);
    int32_t verifyDepositRequest(Json::Value &req_data, bool &depositOK, CMyString &platformOrderNo, int64_t &realFee, CMyString &regionCode);

private:
    CMyString GetSign(const CMyString& strAppKey, const CMyString& strParam, const CMyString& strUrl);
    void      dealUnFinishOrder();
    //轮询时调用，支付中心调用游服发货接口
    int32_t   doConsumeAndDeliverGoods(const CMyString& strOrderNo, const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, 
                                        const CMyString& strUserId, int32_t nPlatform, int64_t amount, CMyString& billNo, int64_t save_amt);
     
    //不调游服发货接口，发货数据直接返回给游服data，游服自己发货
    int32_t   doConsumeAndDeliverGoodsEx(const CMyString& strOrderNo, const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, 
                                        const CMyString& strUserId, int32_t nPlatform, int64_t amount, CMyString& billNo, Json::Value& data);

    int32_t getBalance(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, int32_t& amt);
    int32_t consumeCoin(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, int64_t amount, CMyString& billNo);
    int32_t cancelPayCoin(const CMyString& strOpenId, const CMyString& strOpenKey, const CMyString& strPf, const CMyString& strPfKey, const CMyString& strAppId, const CMyString& strAppKey, int32_t nPlatform, CMyString& billNo);

private:
    void OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp);
    void postAlert(const CMyString &gameCode, const CMyString &note);

    void StrReplace(CMyString &str, const char *from, const char* to);
    bool fetchDepositOrderData(const CMyString& orderNo, Json::Value& req_data);
    bool updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode);
    bool postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data);
    bool deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum TCPayDepositStatus &result, bool &bNeedReDeliver);
    void doPayFailure(char* szResp, uint32_t& cbResp);
    bool finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin);
    void getDepositWealth(CGame *pGame, int32_t bpId, int64_t fee, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo);
    void judgeBPInfo(CGame *pGame, const int32_t& bpId, const int64_t& fee, const CMyString& strUin, Json::Value& req_data, int64_t& coin, int64_t& ext_coin, CMyString& itemInfo, bool bNeedEscape);

    void dealReDeliver(void);
    void dealReDeliver(const CMyString &key);
private:
    CMyString m_strGetBanlanceHost;
    CMyString m_strGetBanlanceUrl;
    CMyString m_strConsumeCoinHost;
    CMyString m_strConsumeCoinUrl;
    CMyString m_strCancelPayHost;
    CMyString m_strCancelPayUrl;
    
    std::vector<int32_t>        m_reDeliverTimes;     // 重新向游服申请发货的时间序列
    redis_cache::CIncrCache     m_cacheDeliverGoods;  // 向gamesvr发送通知的cache
    CMyTimer                    m_timerDeliverGoods;  // 定时检查发货缓存

    redis_cache::CIncrCache     m_cacheYybConsumeOrder;
    CMyTimer                    m_timerOrder;
    redis_cache::CFmtCache      m_cacheOrder;
    int32_t                     m_errCode;
    CMyString                   m_errNote;
};
