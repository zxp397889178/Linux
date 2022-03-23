/*
 * =====================================================================================
 *
 *       Filename:  LionPay.h
 *
 *    Description:  狮子会提供的支付接口
 *
 *        Version:  1.0
 *        Created:  12/07/2019 09:28:15 PM
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
#include "MyNet.h"
#include <vector>
#include "IWebPlugin.h"

class CLionPay
{
    struct Keydata
    {
        int32_t     id;
        CMyString   keydata;
    };
    typedef std::vector<Keydata>    VEC_KEYDATA;
public:
    CLionPay();
    ~CLionPay();

    int32_t Init(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doLionPay)
    ENTRY_FUNC_DEFINE(doLionPayCallback)
    ENTRY_FUNC_DEFINE(doLionPayQuery)
    ENTRY_FUNC_DEFINE(doLionPayEcho)

private:
    void OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp);

    void Rollback(uint32_t dbId, const CMyString &strNote);
    void OnSendOK(uint32_t dbId, const CMyString &strNote);

private:
    ISimDB*             m_pDB;
    CMyRedis            m_redis;

    http::CHttpClient   m_http;
    VEC_KEYDATA         m_vecKeyData;           // 综合支付 - 支付调用的关键数据
    CMyString           m_strCallbackUrl;       // 综合支付 - 支付结果回调地址
    CMyString           m_strNotifyUrl;         // 回传给游戏调用方的地址
};


