/*
 * =====================================================================================
 *
 *       Filename:  PayChannel.cpp
 *
 *    Description:  支付渠道信息
 *
 *        Version:  1.0
 *        Created:  2019年11月14日 18时24分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "PayChannel.h"

CPayChannel::CPayChannel()
    : m_nId(0)
    , m_configId(0)
    , m_weight(0)
    , m_interval(0)
    , m_clientFlag(0)
    , m_callTimes(0)
    , m_tLastCall(0)
{
}

CPayChannel::~CPayChannel()
{

}

bool CPayChannel::Init(int32_t nId, int32_t configId, const CMyString &name, 
            const CMyString &gameCode, const CMyString &sdkCode, 
            const CMyString &channelCode, const CMyString &methodCode,
            const CMyString &cgi, 
            const CMyString &timeRange, const CMyString &moneyRange, 
            int32_t weight, int32_t interval, int32_t clientFlag)
{
    if (configId <=0 || cgi.empty())
    {
        LogWarn("configId & cgi must not empty, ignore this channel.");
        return false;
    }

    m_nId = nId;
    m_configId = configId;
    m_name = name;
    m_gameCode = gameCode;
    m_sdkCode = sdkCode;
    m_channelCode = channelCode;
    m_methodCode = methodCode;
    m_strCgi = cgi;
    m_weight = weight;
    m_interval = interval;
    m_clientFlag = clientFlag;

    // 解析时间段函数
    bool ret = m_timeRange.Init(timeRange);
    if (!ret)
    {
        return false;
    }

    // 解析金额范围时间段
    ret = m_moneyRange.Init(moneyRange);
    if (!ret)
    {
        return false;
    }

    LogInit("load paychannel ok, configId: %s%d%s, channelId: %d,"\
            "name:%s%s%s,"\
            "game:%s, sdk:%s%s%s,"\
            "channel:%s%s_%s%s, "\
            "cgi:%s%s%s, "\
            "time_range:%s, money_range:%s, "\
            "weith:%d, interval:%d, clientFlag:%d",
            _color_red, configId, _color_end, nId, 
            _color_red, name.c_str(), _color_end, 
            gameCode.c_str(), _color_red, sdkCode.c_str(), _color_end, 
            _color_blue, channelCode.c_str(), methodCode.c_str(), _color_end,
            _color_blue, cgi.c_str(), _color_end,
            timeRange.c_str(), moneyRange.c_str(),
            weight, interval, clientFlag);

    return true;
}

bool CPayChannel::IsTimeValid(time_t val)
{
    return m_timeRange.Valid(val);
}

bool CPayChannel::IsMoneyValid(int64_t coin)
{
    return m_moneyRange.Valid(coin);
}

void CPayChannel::IncCall(void)
{
    ++m_callTimes;
    m_tLastCall = time(NULL);
}


bool CPayChannel::IsIntervalValid(time_t val)
{
    if (0 == m_interval)
    {
        return true;
    }
    return (val > m_tLastCall + m_interval);
}

int32_t CPayChannel::IsValid(int32_t configId, const CMyString &gameCode, int64_t coin, int32_t clientFlag)
{
    if (m_configId != configId)
    {
        // 支付配置方式不匹配
        return -1;
    }

    if (m_gameCode != gameCode)
    {
        // 游戏编码不匹配
        return -3;
    }

    if (!m_moneyRange.Valid(coin))
    {
        // 金额不匹配
        return -5;
    }

    if (m_clientFlag != 0 && m_clientFlag != clientFlag)
    {
        // 该支付方式与前端类型不匹配
        return -7;
    }

    time_t t1 = ::time(NULL);
    if (!m_timeRange.Valid(t1))
    {
        // 时间不匹配
        return -9;
    }

#if 0
    if (m_interval != 0 && (t1>m_tLastCall+m_interval) )
    {
        // 调用太频繁
        return -11;
    }
#endif

    return 0;
}

