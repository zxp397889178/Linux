/*
 * =====================================================================================
 *
 *       Filename:  SdkChannelList.cpp
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
#include "SdkChannelList.h"
#include "GameList.h"
#include "SdkList.h"
#include "ChannelList.h"

CSdkChannel::CSdkChannel(int32_t channelId, int32_t configId, int32_t sdkId, int32_t gameId,
            const CMyString &channelDesc, const CMyString &bankCode, const CMyString &keyData,
            const CMyString &timeRange, const CMyString &moneyRange,
            int32_t weight, int32_t interval, int32_t clientFlag)
    : m_channelId(channelId), m_configId(configId), m_sdkId(sdkId), m_gameId(gameId)
    , m_name(channelDesc), m_bankCode(bankCode) 
    , m_weight(weight), m_interval(interval), m_clientFlag(clientFlag)
    , m_pGame(NULL), m_pSdk(NULL)
{
    // 解析keyData
    if (!keyData.empty())
    {
        Json::Reader reader;
        reader.parse(keyData.c_str(), m_keyData, false);
    }

    // 解析时间段函数
    bool ret = m_timeRange.Init(timeRange);
    if (!ret)
    {
        LogWarn("timeRange [%s] invalid, channelId:%d ", timeRange.c_str(), channelId);
    }

    // 解析金额范围时间段
    ret = m_moneyRange.Init(moneyRange);
    if (!ret)
    {
        LogWarn("moneyRange [%s] invalid, channelId:%d", moneyRange.c_str(), channelId);
    }

    // 引用CGame 及 CSdk对象
    m_pGame = GAME_LIST.Find(gameId);
    if (m_pGame != NULL)
    {
        m_pSdk = SDK_LIST.Find(m_pGame->getCode(), sdkId);
    }
    m_pPayChannel = CHANNEL_LIST.Find(configId);
}

CSdkChannel::~CSdkChannel()
{
}

bool CSdkChannel::IsTimeValid(time_t val)
{
    return m_timeRange.Valid(val);
}

bool CSdkChannel::IsMoneyValid(int64_t coin)
{
    return m_moneyRange.Valid(coin);
}

void CSdkChannel::IncCall(void)
{
    ++m_callTimes;
    m_tLastCall = time(NULL);
}

bool CSdkChannel::IsIntervalValid(time_t val)
{
    if (0 == m_interval)
    {
        return true;
    }
    return (val > m_tLastCall + m_interval);
}

int32_t CSdkChannel::IsValid(int32_t configId, int64_t coin, int32_t clientFlag)
{
    if (m_configId != configId)
    {
        // 支付配置方式不匹配
        return -1;
    }
/*
    if (m_pGame==NULL || gameCode!=m_pGame->getCode())
    {
        // 游戏编码不匹配
        return -3;
    }
*/    

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

bool CSdkChannel::Disable(ISimDB *pDB)
{
    pDB->Cmd("UPDATE `t_sdkchannel` SET `status`=0,`updateTime`=NOW() WHERE `channelId`=%d", m_channelId );
    int32_t ret = pDB->Exec();

    // TODO: 处理上一级 SDK， 以及 t_channel 中的许可情况

    return (ret>=0);
}

const CMyString CSdkChannel::getSdkCode(void)
{
    if (NULL == m_pSdk)
    {
        return "";
    }
    return m_pSdk->getCode();
}

const CMyString CSdkChannel::getMethodCode(void)
{
    if (NULL == m_pPayChannel)
    {
        return "";
    }
    return m_pPayChannel->getPayStyle();
}

const CMyString CSdkChannel::getGameCode(void)
{
    if (NULL == m_pGame)
    {
        return "";
    }
    return m_pGame->getCode();
}

///////////////////////////////////////////////////////////////////////////////
CSdkChannelList::CSdkChannelList()
{
}

CSdkChannelList& CSdkChannelList::InstanceGet()
{
    static CSdkChannelList s_obj;
    return s_obj;
}

CSdkChannelList::~CSdkChannelList()
{
}

int32_t CSdkChannelList::Load(ISimDB *pDB)
{
    m_vecSdkChannel.clear();

//    pDB->Cmd("SELECT `configId`,`gameId`,`payStyleCode`,`moneyPoint`,`moneyBegin`,`moneyEnd`,`manualFlag`,`flag`,`icon`,`sortOrder`,`nameId` ");
//    pDB->Cmd("FROM `t_channel` WHERE `status`=1;");
    pDB->Cmd("SELECT `channelId`,`configId`,`sdkId`,`gameId`,`name`,`bankCode`,`keyData`,`activeTime`,`moneyRange`,`weight`,`interval`,`clientFlag` ");
    pDB->Cmd("FROM `t_sdkchannel` WHERE `status`=1 ");
    while(pDB->More())
    {
        CSdkChannel sdkChannel( pDB->GetInt(0),     // channelId
                pDB->GetInt(1),         // configId
                pDB->GetInt(2),         // sdkId
                pDB->GetInt(3),         // gameId
                pDB->GetString(4),      // name
                pDB->GetString(5),      // bankCode
                pDB->GetString(6),      // keyData
                pDB->GetString(7),      // activeTime
                pDB->GetString(8),      // moneyRange
                pDB->GetInt(9),         // weight
                pDB->GetInt(10),        // interval
                pDB->GetInt(11)         // clientFlag
                ); 
        m_vecSdkChannel.push_back(sdkChannel);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query sdkChannel list failure, error %s", szError);
        return -1;
    }
    return 0;
}

CSdkChannel* CSdkChannelList::Find(int32_t channelId)
{
    CSdkChannel *pSdkChannel(NULL);
    VEC_SDKCHANNEL::iterator it(m_vecSdkChannel.begin());
    for(; it != m_vecSdkChannel.end(); ++it)
    {
        CSdkChannel &sdkChannel = *it;
        if (sdkChannel.getId() == channelId)
        {
            pSdkChannel = &sdkChannel;
            break;
        }
    }
    return pSdkChannel;
}

int32_t CSdkChannelList::GetChannel(int32_t configId, std::vector<CSdkChannel*> &vecChannel)
{
    VEC_SDKCHANNEL::iterator it(m_vecSdkChannel.begin());
    for(; it != m_vecSdkChannel.end(); ++it)
    {
        CSdkChannel &sdkChannel = *it;
        if (sdkChannel.getConfigId() == configId)
        {
            vecChannel.push_back(&sdkChannel);
        }
    }
    int32_t cnt = static_cast<int32_t>(vecChannel.size());
    return cnt;
}

int32_t CSdkChannelList::GetChannel(int32_t configId, int64_t payMoney, int32_t clientType, std::vector<CSdkChannel*> &vecChannel, int32_t &totalWeight)
{
    VEC_SDKCHANNEL::iterator it(m_vecSdkChannel.begin());
    for(; it != m_vecSdkChannel.end(); ++it)
    {
        CSdkChannel &sdkChannel = *it;
        if (sdkChannel.IsValid(configId, payMoney, clientType))
        {
            vecChannel.push_back(&sdkChannel);
            totalWeight += sdkChannel.getWeight();
        }
    }
    int32_t cnt = static_cast<int32_t>(vecChannel.size());
    return cnt;
}

