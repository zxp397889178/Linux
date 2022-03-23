/*
 * =====================================================================================
 *
 *       Filename:  PayWrapper.h
 *
 *    Description:  支付插件封装类
 *
 *        Version:  1.0
 *        Created:  2020年02月01日 15时17分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include "MyDB.h"
#include <vector>
#include <queue>
#include "GameList.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"
#include "BillPointList.h"

enum PayStatus
{
    payCreateOrder = 0,     // 1: 创建订单，未发送到第三方平台
    payCallbackOK,          // 3: 收到回调：支付成功
    payCallbackFail,        // 3: 收到回调：支付失败            (OVER)
    payCancel,              // 3: 支付取消，暂未用此码          (OVER)
    payRefound,             // 3: 退款，暂未用此码              (OVER)
    payFinished,            // 4: 订单完成（发放金币）          (OVER)
    payReqSucceed,          // 2: 向第三方请求成功
    payReqFail,             // 2: 向第三方请求失败              (OVER)
    payNotifyFail,          // 4: 向游服发送通知失败，需要人工处理      (REDO)
    payBalanceFail,         // 4: 游服处理订单失败，人工介入处理        (REDO)
    payCallbackPending,     //    等待第三方确认，需要重新执行请求
};

class CPayWrapper
{
public:
    explicit CPayWrapper(const CMyString &payFlag);
    virtual ~CPayWrapper();

protected:
    enum postRespError
    {
        postNothing = 0,        // 不做任何处理
        postDisableBillPoint,   // 停用本条计费点
        postDisableChannel,     // 禁用本通道
        postDisableSDK,         // 禁用本SDK
    };
protected:
    /* sect_  : 支付模块的 section
     *  <xml>
     *      <sect_/db main="xxx" redis="yyy" />
     *      <sect_/payid data="1,2,3,4..." />
     *      <sect_/notify url="http:/xxxx" />
     *      <sect_/callback url="http://127.0.0.1:..." />
     *  </xml>
     */
    int32_t Init(const CMyString &sect_);

    void Maintance(void);

// request    
protected:
    /* 记录订单，并查询订单的关键数据
     * 参数： 
     *      req         http 请求
     *      reader      本次支付SDK的关键数据
     *      orderId     返回 tPayOrder中的`id`字段
     *      strOrderNo  生成平台的订单号
     *      szResp      若失败，返回异常信息
     *      cbResp      szResp的空音大小
     */
    int32_t _before_send_request(http::CHttpRequest &req, 
            CSdkChannel* &pChannel, int32_t &nOrderId, 
            CMyString &strOrderNo, char* szResp, uint32_t &cbResp);

    /* 向第三方平台发送请求后，对订单记录的操作
     * 参数：
     *     nOrderId     tPayOrder表的`id`字段
     *     code         tPayOrder表的状态字段
     *     strReq       请求数据
     *     req_time     请求时间
     *     strRespHeader 返回的header
     *     strResp      返回的数据
     *     strPlatformOrderNo   平台的订单编号，有可能没有
     */
    bool _after_send_request(int32_t nOrderId, int32_t code, const CMyString &strReq, 
            const char *req_time, const CMyString &strRespHeader, const CMyString &strResp,
            const CMyString &strPlatformOrderNo);

    /* 把平台生成订单号对应的ID存储到redis表中
     * 参数：
     *      strOrderNo  平台生成的订单号
     *      nOrderId    tPayOrder表的`id`字段
     */
    void on_save_orderId(const CMyString &strOrderNo, int64_t nOrderId);

    /* 把订单ID对应的商户ID,商户KEY记录起来
     * 参数：
     *      strOrderNo  平台生成的订单号
     *      req         http请求
     */
    void on_save_order_channelId(const CMyString &strOrderNo, http::CHttpRequest &req);

    /* 读取订单ID对应支付渠道信息
     * 参数：
     *      strOrderNo  平台生成的订单号
     */
    CSdkChannel* on_get_order_sdkChannel(const CMyString strOrderNo);

    /* 向第三方平台发送支付请求时，失败返回的信息
     * 参数：
     *      err         错误号
     *      strNote     错误描述
     *      szResp      返回给请求端的结果
     *      cbResp      szResp的大小
     */
    void OnPayFailure(int32_t err, const CMyString &strNote, char *szResp, uint32_t &cbResp);
    void OnPayResp(Json::Value &resp, char *szResp, uint32_t &cbResp);

    /* 向第三方平台请求支付成功后，返回给游戏服务端的信息
     * 参数：
     *      code        错误号，0
     *      strNote     返回游服端的提示
     *      strTradeNo  平台生成的订单号
     *      strUrl      第三方平台返回的跳转URL,或数据
     *      szResp      返回给请求端的结果
     *      cbResp      szResp的大小
     *      strSdk      支付SDK的代码
     *      strChannel  支付渠道，如微信、支付宝等
     *      strMethod   支付方法，如原生、H5等
     */
    void OnPayResp(int32_t code, const CMyString &strNote, const CMyString &strTradeNo, 
            const CMyString &strUrl, char *szResp, uint32_t &cbResp,
            const CMyString &strSdk, const CMyString &strChannel, const CMyString &strMethod);

// callback
protected:
    /** 记录数据库信息、发送充值回调给游服并记录成功状态
     *  参数：
     *      strOrderNo          tPayOrder表中，由平台方生成的订单号，字符串
     *      strPlatformId       第三方平台生成的订单号/流水号
     *      strCallbackResp     第三方平台返回的完整的字符串，POST
     *      code                tPayOrder中的状态
     *      realFee             第三方平台真实扣款金额,以分计
     *  返回值：
     *      0       无需后续处理，成功或失败
     *      <0      通知失败了，需要后续人工处理
     */
    int32_t _deal_notify(const CMyString &strOrderNo, const CMyString &strPlatformId, 
            const CMyString &strCallbackResp, int32_t code, int64_t realFee, const CMyString &regionCode="");

    // 校验回调IP是否合法
    // 返回值： 
    //  0 - IP校验合法
    // -1 - 订单号查找不到
    // -2 - 游戏编码出错（空）
    // -3 - 游戏信息查找失败
    // -4 - SDK信息查找失败
    // -5 - IP 不合法
    // pGame => strOrderNo 对应的游戏指针
    int32_t validCallbackIp(const CMyString &strOrderNo, const CMyString &strIp, CGame* &pGame);

protected:
    CSdk* getSdk(const CMyString &gameCode);
    CSdkChannel* getSdkChannel(int32_t channelId);
    CGame* getGame(const CMyString &gameCode);
    CGame* getGame(int32_t gameId);

    // 向 pGame 发送警告信息
    // 返回值：1成功， 0未配置警告，-1出错
    int32_t postAlert(CGame *pGame, const CMyString &strText);

    // 返回 gameCode + errorDesc 对应的错误描述，以及后续的处理办法
    int32_t dealRespError(CSdkChannel *pChannel, int32_t payMoney, const CMyString &errorDesc, CMyString &errorResp);

    /* 收到第三方平台回调后，从redis 或 mysql 中获得 orderId，返回给游戏方的JSON串
     * 参数：
     *      strOrderNo          平台方生成的订单号，第三方平台返回值
     *      orderId             tPayOrder表中的`id` 字段，唯一标识
     *      strRespData         由平台方生成的需要返回给游戏方的JSON串,存在redis中(超时后从mysqk表中查询)
     * 返回值：    
     *      <0      失败
     *       0      成功
     */
    int32_t getStoredOrderInfo(const CMyString &strOrderNo, int32_t &orderId, Json::Value &resp, bool freeCache);
    void freeOrderExpired(const CMyString &orderNo);

private:
    int32_t createNewOrder(http::CHttpRequest &req, int32_t channelId, int32_t &nOrderId, CMyString &strOrderNo, char *szResp, uint32_t &cbResp );

    int32_t on_read_orderId(const CMyString &strOrderNo, int32_t &nOrderId, bool freeCache);
    int32_t on_read_orderData(const CMyString &strOrderNo, Json::Value &resp, bool freeCache);

    // 查询已超时的订单，返回值：
    // -1 系统错误，请查日志
    // 0  该订单已完成，不需要处理
    // 1  需要对该订单做后续处理
    int32_t queryTimeoutOrder(const CMyString &strOrderNo, int32_t &orderId, Json::Value &resp);


    /* 把第三方平台的回调信息存储到数据库中
     * 参数：
     *      orderId             tPayOrder表中的`id`字段，关键字
     *      code                第三方平台回调返回的状态，成功或失败
     *      strPlatformId       第三方平台回调返回的流水号
     *      strResp             第三方平台回调返回的完整的字符串，POST
     *      realFee             第三方平台真实扣款金额
     *      regionCode          支付的地区代码，与币种对应
     */
    int32_t storeCallbackData(int32_t nOrderId, int32_t code, const CMyString &strPlatformId, const CMyString &strResp, int32_t realFee, const CMyString &regionCode);

    /* 记录向游戏服务器通知的状态，存储到数据库中去
     * 参数：
     *      orderId             tPayOrder表中的`id`字段，关键字
     *      code                通知游服发金币的状态：未通知到/成功/失败
     */
    int32_t logNotifyStatus(int32_t nOrderId, const CMyString &uin, int32_t bpId, int32_t code);

    int32_t check_mysql_failure(ISimDB *pDB, const CMyString &strNote, const CMyString &strFunc, int32_t nLine);

    void applyChange(void);
    
    bool disableBillPoint(CSdkChannel *pChannel, int32_t payMoney);
    bool disableSdk(CSdkChannel *pChannel);
    bool disableSdkChannel(CSdkChannel *pChannel);

    // 从redis中查到需要向gamesvr 重新发送的数据
    void notify2gamesvr(void);
    void redoNotifyGamesvr(const CMyString &notify_data);

    // 检查订单是否超时
    void checkOrderExpired(void);

protected:
    http::CHttpClient   m_http;
    CMyString           m_sdkCode;              // SDK 的code
    ISimDB*             m_pDB;                  // 充值的主访问接口
    CMyRedis            m_redis;
    const CMyString     m_payFlag;              // 支付状态说明，用于日志提示之用

private:
    CMyString           m_applyUrl;             // 响应更新的URL

    CMyTimer            m_timerRedo;            // 每隔5s重新检查一次

    // 订单支付超时设置
    int32_t             m_nOrderTimeout;        // 订单超时时间，以秒计
    typedef std::map<CMyString, int64_t>    ORDER_EXPIRED_MAP;
    ORDER_EXPIRED_MAP   m_mapOrderExpired;
};


