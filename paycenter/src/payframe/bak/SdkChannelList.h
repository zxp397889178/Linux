/*
 * =====================================================================================
 *
 *       Filename:  SdkChannelList.h
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
#pragma once
#include "BaseCode.h"
#include "MyDB.h"
#include <vector>
#include "MoneyRange.h"
#include "TimeRange.h"

class CGame;
class CSdk;
class CPayChannel;
class CSdkChannel
{
public:
    CSdkChannel(int32_t channelId, int32_t configId, int32_t sdkId, int32_t gameId,
            const CMyString &channelDesc, const CMyString &bankCode, const CMyString &keyData,
            const CMyString &timeRange, const CMyString &moneyRange,
            int32_t weight, int32_t interval, int32_t clientFlag);
    ~CSdkChannel();

    int32_t getId(void)         { return m_channelId; }
    int32_t getConfigId(void)   { return m_configId; }
    CGame* getGame(void)        { return m_pGame; }
    CSdk* getSdk(void)          { return m_pSdk; }
    CPayChannel* getPayChannel(void) { return m_pPayChannel; }
    CMyString& getName(void)    { return m_name; }
    int32_t getWeight(void)     { return m_weight; }
    int32_t getInterval(void)   { return m_interval; }
    int32_t getClientFlag(void) { return m_clientFlag; }
    CMyString& getBankCode(void)     { return m_bankCode; }
    Json::Value& getKeyData(void)    { return m_keyData; }
    void getKeyData(CJsonReader &jr) { jr.Init(m_keyData); }

    // 是否在支付时间范围内
    bool IsTimeValid(time_t val);
    // 支付时间间隔是否有效
    bool IsIntervalValid(time_t val);
    // 金币是否有效(coin 以分传入)
    bool IsMoneyValid(int64_t coin);

public:
    // 判断该支付渠道是否可以调用
    // 返回值： 0   符合调用条件
    //          -1  支付方式不匹配
    //          -3  不是本游戏的支付方式
    //          -5  金额不匹配
    //          -7  前端类型不支持，如某些渠道只支持android,但前端是iOS
    //          -9  超出可用的时间范围
    //          -11 调用太频繁
    int32_t IsValid(int32_t configId, int64_t coin, int32_t clientFlag);

    // 确认调用此支付渠道
    void IncCall(void);

    bool Disable(ISimDB *pDB);

    const CMyString getSdkCode(void);
    const CMyString getMethodCode(void);
    const CMyString getGameCode(void);

private:
    int32_t         m_channelId;      // 渠道ID
    int32_t         m_configId;       // 支付ID, 1..n
    int32_t         m_sdkId;          // SDKID,  1..n 与 CSdk 中的id 一致
    int32_t         m_gameId;         // 游戏ID，1..n 与 CGameItem 中的id一致

    CMyString       m_name;           // 渠道名称
    CMyString       m_bankCode;       // 银行码
    Json::Value     m_keyData;        // 关键数据
    CTimeRange      m_timeRange;      // 时间范围,该ID可用的时间范围, 如 06:00:00-12:00:00;15:00:00-22:00:00
    CMoneyRange     m_moneyRange;     // 金额范围,该ID支付的上下限，以分为基数，如 10000-200000 表示 100.0 至 2000.0
    int32_t         m_weight;         // 权重,支付权重，权重越大越优先
    int32_t         m_interval;       // 间隔,两笔支付的间隔，以秒计算
    int32_t         m_clientFlag;     // 允许的客户端范围,支持的前端类型，0表示全部支持
    
private:
    uint32_t        m_callTimes;      // 调用次数
    time_t          m_tLastCall;      // 最近调用时间
    CGame*          m_pGame;
    CSdk*           m_pSdk;
    CPayChannel*    m_pPayChannel;
};

class CSdkChannelList
{
    typedef std::vector<CSdkChannel>    VEC_SDKCHANNEL;
public:
    static CSdkChannelList& InstanceGet();
    ~CSdkChannelList();

    int32_t Load(ISimDB *pDB);

    CSdkChannel* Find(int32_t channelId);
    int32_t GetChannel(int32_t configId, std::vector<CSdkChannel*> &vecChannel);

    int32_t GetChannel(int32_t configId, int64_t payMoney, int32_t clientType,
            std::vector<CSdkChannel*> &vecChannel, int32_t &totalWeight);

private:
    CSdkChannelList();

private:
    VEC_SDKCHANNEL      m_vecSdkChannel;
};

#define SDKCHANNEL_LIST     CSdkChannelList::InstanceGet()


