/*
 * =====================================================================================
 *
 *       Filename:  PayChannel.h
 *
 *    Description:  支付渠道信息
 *
 *        Version:  1.0
 *        Created:  2019年11月14日 16时15分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __PAYCENTER_PAY_CHANNEL_H__
#define __PAYCENTER_PAY_CHANNEL_H__
#pragma once

#include "BaseCode.h"
#include <vector>
#include "MoneyRange.h"
#include "TimeRange.h"

class CPayChannel
{
public:
    CPayChannel();
    ~CPayChannel();

    /* nId:         唯一号
     * configId     外部调用支付ID
     * name         支付渠道的中文说明
     * gameCode     游戏代码
     * sdkCode      第三方支付代码
     * channelCode  支付渠道代码，如支付宝、微信等
     * methodCode   支付方式代码，如H5、原生等
     * cgi          调用的cgi
     * timeRange    该ID可用的时间范围, 如 06:00:00-12:00:00;15:00:00-22:00:00
     * moneyRange   该ID支付的上下限，以分为基数，如 10000-200000 表示 100.0 至 2000.0
     * weight       支付权重，权重越大越优先
     * interval     两笔支付的间隔，以秒计算
     * clientFlag   支持的前端类型，0表示全部支持
     * */
    bool Init(int32_t nId, int32_t configId, const CMyString &name, 
            const CMyString &gameCode, const CMyString &sdkCode, 
            const CMyString &channelCode, const CMyString &methodCode,
            const CMyString &cgi, 
            const CMyString &timeRange, const CMyString &moneyRange, 
            int32_t weight, int32_t interval, int32_t clientFlag);


    // 判断该支付渠道是否可以调用
    // 返回值： 0   符合调用条件
    //          -1  支付方式不匹配
    //          -3  不是本游戏的支付方式
    //          -5  金额不匹配
    //          -7  前端类型不支持，如某些渠道只支持android,但前端是iOS
    //          -9  超出可用的时间范围
    //          -11 调用太频繁
    int32_t IsValid(int32_t configId, const CMyString &gameCode, int64_t coin, int32_t clientFlag);

    // 确认调用此支付渠道
    void IncCall(void);

    int32_t getId(void)         { return m_nId; }
    int32_t getConfigId(void)   { return m_configId; }
    int32_t getWeight(void)     { return m_weight; }
    int32_t getInterval(void)   { return m_interval; }
    int32_t getClientFlag(void) { return m_clientFlag; }
    const char* getName(void)       { return m_name.c_str(); }
    const char* getGameCode(void)   { return m_gameCode.c_str(); }
    const char* getSdkCode(void)    { return m_sdkCode.c_str(); }
    const char* getChannelCode(void){ return m_channelCode.c_str(); }
    const char* getMethodCode(void) { return m_methodCode.c_str(); }
    const char* getCgi(void)        { return m_strCgi.c_str(); }

protected:
    // 是否在支付时间范围内
    bool IsTimeValid(time_t val);
    // 支付时间间隔是否有效
    bool IsIntervalValid(time_t val);
    // 金币是否有效(coin 以分传入)
    bool IsMoneyValid(int64_t coin);

private:
    int32_t         m_nId;            // 渠道ID
    int32_t         m_configId;       // 支付ID,与前端的configId一致
    CMyString       m_gameCode;       // 游戏CODE
    CMyString       m_sdkCode;        // SDK CODE
    CMyString       m_channelCode;    // 支付渠道CODE
    CMyString       m_methodCode;     // 支付方式CODE
    CMyString       m_name;           // 渠道名称
    CTimeRange      m_timeRange;      // 时间范围
    CMoneyRange     m_moneyRange;     // 金额范围
    int32_t         m_weight;         // 权重
    int32_t         m_interval;       // 间隔
    CMyString       m_strCgi;         // CGI 函数
    int32_t         m_clientFlag;     // 允许的客户端范围
    
    uint32_t        m_callTimes;      // 调用次数
    time_t          m_tLastCall;      // 最近调用时间
};

#endif // __PAYCENTER_PAY_CHANNEL_H__

