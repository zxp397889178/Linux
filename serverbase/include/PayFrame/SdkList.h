/*
 * =====================================================================================
 *
 *       Filename:  SdkList.h
 *
 *    Description:  支付SDK，以及该SDK下所有的渠道
 *
 *        Version:  1.0
 *        Created:  2020年07月26日 11时51分38秒
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
#include "MyDB.h"
#include "GameList.h"
#include "IpList.h"

////////////////////////////////////////////////////////////////////////
// CSdk => t_sdk
class CSdk
{
    friend class CSdkList;
    struct RechargeData
    {
        CMyString       cgi;            // 支付调用的CGI     (支付中心提供)
        CMyString       payUrl;         // 支付调用的URL     (SDK提供)
        CMyString       checkUrl;       // 支付结果查询的URL (SDK提供)
        CMyString       callbackUrl;    // 支付成功回调的URL (支付中心提供)
        Json::Value     extraData;      // 扩展数据          (支付中心提供)
        CIpList         validIps;       // 支付回调有效的IP  (SDK提供)
    };
    struct WithdarwData
    {
        CMyString       cgi;            // 提现调用CGI      (支付中心提供)
        CMyString       withdrawUrl;    // 提现调用URL      (SDK提供)
        CMyString       checkUrl;       // 提现结果查询URL  (SDK提供)
        CMyString       callbackUrl;    // 提现回调的URL    (SDK提供)
        CIpList         validIps;       // 提现回调有效的IP (SDK提供)
    };
    struct SdkError
    {
        CMyString   platformNote;  // 平台错误说明
        CMyString   respNote;      // 返回的错误提示
        int32_t     dealStyle;      // 处理方式:0-不处理；1-停用计费点；2-停用充值通道；3-停用SDK
    };
    typedef std::vector<SdkError>   VEC_SDKERROR;
public:
    CSdk(int32_t id, const CMyString& code, const CMyString& name,
        const CMyString& gameCode, const CMyString& account, const CMyString& apiKey,
        const CMyString& payCgi, const CMyString &payUrl, const CMyString &payCheckUrl,
        const CMyString &payCallbackUrl, const CMyString &payExtraData, const CMyString &payIps,
        const CMyString &wdCgi, const CMyString &wdUrl, const CMyString &wdCheckUrl,
        const CMyString &wdCallbackUrl, const CMyString &wdIps, 
        const int32_t& enableDeposit, const int32_t& enableWithdraw);
    ~CSdk();

    // 基础属性
    int32_t getId(void) { return m_sdkId; }
    CMyString& getCode(void) { return m_code; }
    CMyString& getName(void) { return m_name; }
    CMyString& getGameCode(void) { return m_gameCode; }
    CMyString& getAccount(void) { return m_account; }
    CMyString& getApiKey(void) { return m_apiKey; }
    bool getEnableDeposit(void) { return m_enableDeposit == 1;}
    bool getEnableWithdraw(void) { return m_enableWithdraw == 1;}

    // 充值相关属性
    CMyString& getPayCgi(void) { return m_recharge.cgi; }
    CMyString& getPayUrl(void) { return m_recharge.payUrl; }
    CMyString& getPayCheckUrl(void) { return m_recharge.checkUrl; }
    CMyString& getPayCallbackUrl(void) { return m_recharge.callbackUrl; }
    Json::Value& getPayExtraData(void) { return m_recharge.extraData; }
    void getPayExtraData(CJsonWrapper &jr) { jr.Load(m_recharge.extraData); }
    bool IsRechargeIpValid(const CMyString &ip);

    // 提现相关属性
    CMyString& getWithdrawCgi(void) { return m_withdraw.cgi; }
    CMyString& getWithdrawUrl(void) { return m_withdraw.withdrawUrl; }
    CMyString& getWithdrawCheckUrl(void) { return m_withdraw.checkUrl; }
    CMyString& getWithdrawCallbackUrl(void) { return m_withdraw.callbackUrl; }
    bool IsWithdrawIpValid(const CMyString &ip);

public:
    // 错误信息相关参数
    bool GetErrorNote(const CMyString &platformNote, CMyString &respNote, int32_t &dealStyle);

    // 禁用此SDK
    bool Disable(ISimDB *pDB);
private:
    int32_t LoadSdkError(ISimDB *pDB);

private:
    int32_t         m_sdkId;
    CMyString       m_code;
    CMyString       m_name;
    CMyString       m_gameCode;
    CMyString       m_account;
    CMyString       m_apiKey;       // 支付API加密KEY
    int32_t         m_enableDeposit;    //是否可充值
    int32_t         m_enableWithdraw;   //是否可提现

    RechargeData    m_recharge;     // 充值相关数据
    WithdarwData    m_withdraw;     // 提现相关数据

    VEC_SDKERROR    m_vecSdkError;
};

////////////////////////////////////////////////////////////////////////
// 数据库中所有SDK的集合
class CSdkList
{
    typedef std::vector<CSdk>   VEC_GAMESDK;
public:
    static CSdkList& InstanceGet();
    ~CSdkList();

    int32_t Load(ISimDB *pDB);
    CSdk* Find(const CMyString &gameCode, int32_t sdkId);
    CSdk* FindByCode(const CMyString &gameCode, const CMyString &sdkCode);

private:
    CSdkList();

private:
    VEC_GAMESDK         m_vecSdk;
};

#define SDK_LIST     CSdkList::InstanceGet()

