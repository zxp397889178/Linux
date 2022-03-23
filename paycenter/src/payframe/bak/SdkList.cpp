/*
 * =====================================================================================
 *
 *       Filename:  SdkList.cpp
 *
 *    Description:  支付SDK及其下属支付渠道实现
 *
 *        Version:  1.0
 *        Created:  2020年07月26日 12时02分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "SdkList.h"
#include <algorithm>


////////////////////////////////////////////////////////////////////////
CSdk::CSdk(int32_t sdkId, const CMyString& code, const CMyString& name,
        const CMyString& gameCode, const CMyString& account, const CMyString& apiKey,
        const CMyString& payCgi, const CMyString &payUrl, const CMyString &payCheckUrl,
        const CMyString &payCallbackUrl, const CMyString &payExtraData, const CMyString &payIps,
        const CMyString &wdCgi, const CMyString &wdUrl, const CMyString &wdCheckUrl,
        const CMyString &wdCallbackUrl, const CMyString &wdIps)
    : m_sdkId(sdkId), m_code(code), m_name(name), m_gameCode(gameCode)
    , m_account(account), m_apiKey(apiKey)
{
    m_recharge.cgi = payCgi;
    m_recharge.payUrl = payUrl;
    m_recharge.checkUrl = payCheckUrl;
    m_recharge.callbackUrl = payCallbackUrl;
    if (!payExtraData.empty())
    {
        Json::Reader reader;
        reader.parse(payExtraData.c_str(), m_recharge.extraData, false);
    }
    if (!payIps.empty())
    {
        m_recharge.validIps.setIps(payIps, ',');
    }

    m_withdraw.cgi = wdCgi;
    m_withdraw.withdrawUrl = wdUrl;
    m_withdraw.checkUrl = wdCheckUrl;
    m_withdraw.callbackUrl = wdCallbackUrl;
    if (!wdIps.empty())
    {
        m_withdraw.validIps.setIps(wdIps, ',');
    }
}

CSdk::~CSdk()
{
    m_vecSdkError.clear();
}

int32_t CSdk::LoadSdkError(ISimDB *pDB)
{
    m_vecSdkError.clear();

    pDB->Cmd("SELECT `errorPlatform`,`errorResp`,`errorDeal` FROM `t_sdk_error` ");
    pDB->Cmd("WHERE `gameCode`='%s' and `sdkCode`='%s'", m_gameCode.c_str(), m_code.c_str());
    while(pDB->More())
    {
        SdkError error;
        error.platformNote = pDB->GetString(2);
        error.respNote = pDB->GetString(3);
        error.dealStyle = pDB->GetInt(4);
        m_vecSdkError.push_back(error);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("get valid sdk failure, sdk:%s", m_code.c_str());
        return -1;
    }
    return 0;
}

bool CSdk::GetErrorNote(const CMyString &platformNote, CMyString &respNote, int32_t &dealStyle)
{
    bool bFind(false);
    VEC_SDKERROR::iterator it(m_vecSdkError.begin());
    for(; it != m_vecSdkError.end(); ++it)
    {
        SdkError &error = *it;
        if (error.platformNote == platformNote)
        {
            respNote = error.respNote;
            dealStyle = error.dealStyle;
            bFind = true;
            break;
        }
    }
    return bFind;
}

bool CSdk::Disable(ISimDB *pDB)
{
    pDB->Cmd("UPDATE `t_sdk` SET `status`=0,`updateTime`=NOW() WHERE `sdkId`=%d", m_sdkId);
    int32_t ret = pDB->Exec();
    // TODO: 修改ChannelList中的配置
    return ret>=0;
}

////////////////////////////////////////////////////////////////////////
CSdkList::CSdkList()
{
}

CSdkList::~CSdkList()
{
    m_vecSdk.clear();
}

int32_t CSdkList::Load(ISimDB *pDB)
{
    m_vecSdk.clear();

    pDB->Cmd("SELECT `sdkId`,`code`,`name`,`gameCode`,`accountId`,`apiKey`, ");
    pDB->Cmd("`recharge_cgi`,`apiPayUrl`,`apiCheckUrl`,`notifyUrl`,`extraData`,`validIps`, ");
    pDB->Cmd("`withdraw_cgi`,`apiWithdrawUrl`,`apiWithdrawQueryUrl`,`apiWithdrawCallbackUrl`,`validIps_withdraw` ");
    pDB->Cmd("FROM `t_sdk` WHERE `status`=1");
    while(pDB->More())
    {
        CSdk gameSdk(pDB->GetInt(0),    // id
                pDB->GetString(1),          // code
                pDB->GetString(2),          // name
                pDB->GetString(3),          // gameCode
                pDB->GetString(4),          // accountId
                pDB->GetString(5),          // accountKey
                // 以下为支付相关的参数
                pDB->GetString(6),          // cgi
                pDB->GetString(7),          // pay_url
                pDB->GetString(8),          // pay_check_url
                pDB->GetString(9),          // pay_callback_url
                pDB->GetString(10),         // pay_extra_data
                pDB->GetString(11),         // pay_callback_ip
                // 以下为提现相关的参数
                pDB->GetString(12),         // withdraw_cgi
                pDB->GetString(13),         // withdraw_url 
                pDB->GetString(14),         // withdraw_query_url
                pDB->GetString(15),         // withdraw_callback_url
                pDB->GetString(16)          // withdraw_callback_ip
                );
        m_vecSdk.push_back(gameSdk);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_sdk data failure");
        return -1;
    }

    // 逐个sdk查询 error信息
    VEC_GAMESDK::iterator it(m_vecSdk.begin());
    for(; it != m_vecSdk.end(); ++it)
    {
        CSdk &sdk = *it;
        sdk.LoadSdkError(pDB);
    }
    return 0;
}

CSdk* CSdkList::Find(const CMyString &gameCode, int32_t sdkId)
{
    CSdk *pSdk(NULL);
    VEC_GAMESDK::iterator it(m_vecSdk.begin());
    for(; it != m_vecSdk.end(); ++it)
    {
        CSdk &sdk = *it;
        if (sdk.getGameCode()==gameCode && sdk.getId()==sdkId)
        {
            pSdk = &sdk;
            break;
        }
    }
    return pSdk;
}

CSdk* CSdkList::FindByCode(const CMyString &gameCode, const CMyString &sdkCode)
{
    CSdk *pSdk(NULL);
    VEC_GAMESDK::iterator it(m_vecSdk.begin());
    for(; it != m_vecSdk.end(); ++it)
    {
        CSdk &sdk = *it;
        if (sdk.getGameCode()==gameCode && sdk.getCode()==sdkCode)
        {
            pSdk = &sdk;
            break;
        }
    }
    return pSdk;
}

CSdkList& CSdkList::InstanceGet()
{
    static CSdkList s_obj;
    return s_obj;
}


