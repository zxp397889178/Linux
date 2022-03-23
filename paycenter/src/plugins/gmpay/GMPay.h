/*
 * =====================================================================================
 *
 *       Filename:  GMPay.h
 *
 *    Description:  客服放款
 *
 *        Version:  1.0
 *        Created:  2021年07月06日 17时40分08秒
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

class CPluginGMPay : public CPayBase
{
    enum GMPayDepositStatus
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

    enum GMPayGameDepositStatus
    {
        deliver_Success             = 4,    // 充值成功
        deliver_Failure             = 7,    // 充值失败，需要重发
        deliver_OrderFinished       = 11,   // 订单已完成，更改状态
        deliver_OrderNotExist       = 12,   // 订单不存在，更改状态
        deliver_SystemFault         = 13,   // 系统错误，需要重发
        deliver_FeeMotMatch         = 14,   // 订单金额不匹配，更改状态
    };

public:
    CPluginGMPay();
    virtual ~CPluginGMPay();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    
protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE

private:
    bool createDepositOrder(http::CHttpRequest& req, CSdk* pSdk, CGame* pGame, int32_t& orderId, CMyString& orderNo);
    bool createDepositRequest(http::CHttpRequest &req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, 
            int32_t orderId, const CMyString &orderNo, CMyString& platformUrl, CMyString& platformOrderNo, int32_t &cost);
    bool updateDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platforOrderNo);
    void doPayResp(CSdkChannel *pChannel, const CMyString& strUrl, const CMyString &orderNo, char* szResp, uint32_t& cbResp);
    void OnPayResp(Json::Value &resp, char *szResp, uint32_t& cbResp);

    bool fetchDepositOrderData(http::CHttpRequest& req, const CMyString& orderNo, Json::Value& req_data);
    bool validDepositCallbackIp(CSdk *pSdk, const CMyString& strIp);
    bool updateDepositNotify(const CMyString &gameCode, int32_t orderId, int32_t result, const CMyString &platformOrderNo, int64_t realFee, const CMyString &regionCode);
    bool postDepositResultToGamesvr(CGame *pGame, CSdk* pSdk, Json::Value &req_data);
    bool deliverGoods(const CMyString &gameCode, const CMyString &strUrl, const CMyString &strData, enum GMPayDepositStatus &result, bool &bNeedReDeliver);
    void doPayFailure(const CMyString &gameCode, const CMyString &strUin, const CMyString &orderNo, int64_t fee, char* szResp, uint32_t& cbResp);
    bool finishDepositOrder(const CMyString &gameCode, int32_t orderId, int32_t result, int32_t bpId, const CMyString &strUin);

private:
    int32_t                     m_errCode;
    CMyString                   m_errNote;

};