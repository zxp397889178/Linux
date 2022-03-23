/*
 * =====================================================================================
 *
 *       Filename:  ChannelList.cpp
 *
 *    Description:  对标 t_channel，前端交互的主要入口
 *
 *        Version:  1.0
 *        Created:  2021年03月03日 23时34分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "ChannelList.h"
#include "GameList.h"
#include "BillPointList.h"
#include "VipList.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "Dict_ClientType.h"
#include "ChannelGroup.h"

CPayChannel::CPayChannel()
{

}

CPayChannel::CPayChannel(int32_t configId, int32_t gameId, const CMyString &payStyle,
    int64_t moneyBegin, int64_t moneyEnd,
    int32_t manualFlag, int32_t flag, const CMyString &iconUrl, 
#if 0
    int32_t sortOrder, const CMyString &nameId, int32_t payStyleId)
#else
    int32_t sortOrder, const CMyString &nameId, const CMyString& channelRange)
#endif
{
#if 0
    this->Init(configId, gameId, payStyle, moneyBegin, moneyEnd, manualFlag,
            flag, iconUrl, sortOrder, nameId, payStyleId);
#else
    this->Init(configId, gameId, payStyle, moneyBegin, moneyEnd, manualFlag,
            flag, iconUrl, sortOrder, nameId, channelRange);
#endif

}

CPayChannel::~CPayChannel()
{

}

bool CPayChannel::Init(int32_t configId, int32_t gameId, const CMyString &payStyle,
    int64_t moneyBegin, int64_t moneyEnd,
    int32_t manualFlag, int32_t flag, const CMyString &iconUrl, 
#if 0
    int32_t sortOrder, const CMyString &nameId, int32_t payStyleId)
#else
    int32_t sortOrder, const CMyString &nameId, const CMyString& channelRange)
#endif
{
    m_configId = configId;
    m_payStyle = payStyle;
    m_nameLangId = nameId;
    m_iconUrl = iconUrl;
    m_moneyBegin = moneyBegin;
    m_moneyEnd = moneyEnd;
    m_manualFlag = manualFlag;
    m_flag = flag;
    m_sortOrder = sortOrder;
#if 0
    m_payStyleId = payStyleId;
#endif

    m_pGame = GAME_LIST.Find(gameId);
    m_channelRange.Load(channelRange);

    // 生成Json 数据项
    m_data["configId"] = configId;
    m_data["sortOrder"] = sortOrder;
    m_data["nameId"] = nameId;
    m_data["payStyle"] = payStyle;
#if 0
    m_data["payStyleId"] = payStyleId;
#endif
    // m_data["billPointList"] = moneyPoint;
    m_data["moneyBegin"] = (Json::Int64)moneyBegin;
    m_data["moneyEnd"] = (Json::Int64)moneyEnd;
    m_data["manualMoney"] = manualFlag;
    m_data["flag"] = flag;
    m_data["iconUrl"] = iconUrl;
    m_data["channelRange"] = channelRange;

    return true;
}

#if 0
bool CPayChannel::IsValid(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType)
{
    // 检查游服是否匹配
    if (NULL==m_pGame || m_pGame->getCode()!= gameCode)
    {
        return false;
    }
    
    // 检查sdk_channel 是否支持本clientType
    if (!SDKCHANNEL_LIST.IsValid(m_configId, clientType))
    {
        LogWarn("configId %d has not sdkchannel support clientType %s", m_configId, clientType.c_str() );
        return false;
    }
    
    if (m_manualFlag == 1)
    {
        // 人工输入金额支持
        return true;
    }

    // 检查 billpoint ,本玩家是否还有计费点
    int32_t bpCount = BILLPOINT_LIST.GetCount(m_configId, uin);
    if (bpCount <= 0)
    {
        LogWarn("configId %d has not billpoint for user %s", m_configId, uin.c_str());
        return false;
    }

    return true;
}
#endif

bool CPayChannel::IsValid(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, int64_t version, const CMyString &channelCode, int32_t vipLevel)
{
    // 检查游服是否匹配
    if (NULL==m_pGame || m_pGame->getCode()!= gameCode)
    {
        return false;
    }
    
    // 检查sdk_channel 是否支持本clientType
    if (!SDKCHANNEL_LIST.IsValid(m_configId, clientType))
    {
        LogWarn("configId %d has not sdkchannel support clientType %s", m_configId, clientType.c_str() );
        return false;
    }
    
    if (m_manualFlag == 1)
    {
        // 人工输入金额支持
        return true;
    }

    // 检查 billpoint ,本玩家是否还有计费点
    bool val = BILLPOINT_LIST.IsValid(m_configId, uin, version, channelCode, vipLevel);
    if (!val)
    {
        LogWarn("configId %d has not billpoint for user %s, version:%ld, channel:%s, vipLevel:%d", m_configId, uin.c_str(), version, channelCode.c_str(), vipLevel);
        return false;
    }

    return true;
}

bool CPayChannel::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, 
            int64_t version, const CMyString &channelCode, int32_t vipLevel, const CMyString &category, 
            int32_t configId, Json::Value &channel_array)
{
    if (NULL == m_pGame || m_pGame->getCode() != gameCode)
    {
        return false;
    }

    // 检查sdk_channel 是否支持本clientType
    if (!SDKCHANNEL_LIST.IsValid(m_configId, clientType))
    {
        LogWarn("configId %d has not sdkchannel support clientType %s", m_configId, clientType.c_str() );
        return false;
    }

    // 检查channel
    LogMsg("configId: %d  channelCode: %s, channelRange: %s", m_configId, channelCode.c_str(), m_channelRange.toString().c_str());
    if (!m_channelRange.Valid(channelCode))
    {
        LogWarn("configId %d has not support channel range, channelCode: %s, channelRange: %s", m_configId, channelCode.c_str(), m_channelRange.toString().c_str());
        return false;
    }
    
    if (configId != 0 && configId != m_configId)
    {
        LogWarn("configId %d is not equal request configId %d", m_configId, configId);
        return false;
    }
    
    // 获取计费点信息
    Json::Value bp_array;
    bp_array.resize(0);
    int32_t cnt = BILLPOINT_LIST.toJson(m_configId, uin, version, channelCode, vipLevel, category, bp_array);
    if (cnt <= 0)
    {
        LogWarn("configId %d not has billpoint support", m_configId);
        return false;
    }
    m_data["billPointList"] = bp_array;

    // 填充到支付渠道中
    channel_array.append(m_data);
    return true;
}

bool CPayChannel::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, 
            int64_t version, const CMyString &channelCode, int32_t vipLevel, const CMyString &category, 
            int32_t configId, int32_t groupId, Json::Value &channel_array)
{
    if (NULL == m_pGame || m_pGame->getCode() != gameCode)
    {
        return false;
    }

    // 检查sdk_channel 是否支持本clientType
    if (!SDKCHANNEL_LIST.IsValid(m_configId, clientType))
    {
        LogWarn("configId %d has not sdkchannel support clientType %s", m_configId, clientType.c_str() );
        return false;
    }

    // 检查channel
    LogMsg("configId: %d  channelCode: %s, channelRange: %s, vipLevel:%d", m_configId, channelCode.c_str(), m_channelRange.toString().c_str(), vipLevel);
    if (!m_channelRange.Valid(channelCode))
    {
        LogWarn("configId %d has not support channel range, channelCode: %s, channelRange: %s", m_configId, channelCode.c_str(), m_channelRange.toString().c_str());
        return false;
    }
    
    // if (configId != 0 && configId != m_configId)
    // {
    //     LogWarn("configId %d is not equal request configId %d", m_configId, configId);
    //     return false;
    // }

    // 获取计费点信息
    Json::Value bp_array;
    bp_array.resize(0);
    int32_t cnt = BILLPOINT_LIST.toJson(m_configId, uin, version, channelCode, vipLevel, category, bp_array);
    if (cnt <= 0)
    {
        LogWarn("configId %d not has billpoint support", m_configId);
        return false;
    }
    m_data["billPointList"] = bp_array;

    // 填充到支付渠道中
    channel_array.append(m_data);

    return true;
}


//////////////////////////////////////////////////////////////////////////////
//CPayChannelList
CPayChannelList::CPayChannelList()
{

}

CPayChannelList::~CPayChannelList()
{

}

CPayChannelList& CPayChannelList::InstanceGet()
{
    static CPayChannelList s_obj;
    return s_obj;
}

int32_t CPayChannelList::Load(ISimDB *pDB)
{
    m_vecPayChannel.clear();

#if 0
    pDB->Cmd("SELECT distinct `configId`,t1.`gameId`,`payStyleCode`,`moneyBegin`,`moneyEnd`,`manualFlag`,`flag`,`icon`,`sortOrder`,`nameId`,ifnull(t2.`style`,0) ");
    pDB->Cmd("FROM `t_channel` t1 left join `dict_paystyle` t2 on t1.payStylecode=t2.code and t1.`gameId`=t2.`gameId` ");
    pDB->Cmd("WHERE `status`=1  order by t1.`gameId`,t1.`payStyleCode`;")
#else
    pDB->Cmd("SELECT `configId`,`gameId`,`payStyleCode`,"\
            "`moneyBegin`,`moneyEnd`,`manualFlag`,`flag`,`icon`,"\
            "`sortOrder`,`nameId`,`channelRange` ");
    pDB->Cmd("FROM `t_channel` WHERE `status`=1 ");
#endif
    while(pDB->More())
    {
        CPayChannel channel( pDB->GetInt(0),    // configId
                pDB->GetInt(1),         // gameId
                pDB->GetString(2),      // payStyle
                pDB->GetInt64(3),       // moneyBegin
                pDB->GetInt64(4),       // moneyEnd
                pDB->GetInt(5),         // manualFlag
                pDB->GetInt(6),         // flag
                pDB->GetString(7),      // icon
                pDB->GetInt(8),         // sortOrder
#if 0
                pDB->GetString(9),      // nameId
                pDB->GetInt(10)         // payStyleId
#else
                pDB->GetString(9),      // nameId
                pDB->GetString(10)     // channelRange
#endif
                );
        m_vecPayChannel.push_back(channel);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_channel data failure, error %s", szError);
        return -1;
    }

    return 0;
}

CPayChannel* CPayChannelList::Find(int32_t configId)
{
    CPayChannel *pChannel(NULL);
    VEC_PAYCHANNEL::iterator it(m_vecPayChannel.begin());
    for(; it != m_vecPayChannel.end(); ++it)
    {
        CPayChannel &channel = *it;
        if (channel.getConfigId() == configId)
        {
            pChannel = &channel;
            break;
        }
    }
    return pChannel;
}

#if 0
void CPayChannelList::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, Json::Value &resp)
{
    int32_t gameId(0);
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL != pGame)
    {
        gameId = pGame->getId();
    }

    // 获得channel 数据
    Json::Value channel_array;
    channel_array.resize(0);
    VEC_PAYCHANNEL::iterator it(m_vecPayChannel.begin());
    for(; it != m_vecPayChannel.end(); ++it)
    {
        CPayChannel &channel = *it;
        if (channel.IsValid(gameCode, uin, clientType))
        {
            channel.toJson(uin, channel_array);
        }
    }
    resp["channel"] = channel_array;

/*    
    // 获得 billPoint 数据
    Json::Value billpoint_array;
    billpoint_array.resize(0);
    for(std::vector<int32_t>::iterator it(vecConfigId.begin());
            it != vecConfigId.end(); ++it)
    {
        int32_t configId = *it;
        BILLPOINT_LIST.toJson(configId, uin, billpoint_array);
    }
    resp["billPoint"] = billpoint_array;
*/    

    // 获得 vip 数据
    Json::Value vip_array;
    vip_array.resize(0);
    VIP_LIST.toJson(gameId, vip_array);
    resp["vip"] = vip_array;
}
#endif

void CPayChannelList::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, 
        int64_t version, const CMyString &channelCode, int32_t vipLevel, const CMyString &category, 
        int32_t configId, Json::Value &resp)
{
    int32_t gameId(0);
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL != pGame)
    {
        gameId = pGame->getId();
    }

    // 获得channel 数据
    Json::Value channel_array;
    channel_array.resize(0);
    VEC_PAYCHANNEL::iterator it(m_vecPayChannel.begin());
    for(; it != m_vecPayChannel.end(); ++it)
    {
        CPayChannel &channel = *it;
        channel.toJson(gameCode, uin, clientType, version, channelCode, vipLevel, category, configId, channel_array);
#if 0        
        if (channel.IsValid(gameCode, uin, clientType, version, channelCode, vipLevel))
        {
            channel.toJson(uin, channel_array);
        }
#endif
    }
    resp["channel"] = channel_array;

    // 获得 vip 数据
    Json::Value vip_array;
    vip_array.resize(0);
    VIP_LIST.toJson(gameId, vip_array);
    resp["vip"] = vip_array;

}

void CPayChannelList::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, 
        int64_t version, const CMyString &channelCode, int32_t vipLevel, const CMyString &category, 
        int32_t configId, int32_t groupId, Json::Value &resp)
{
    int32_t gameId(0);
    CGame *pGame = GAME_LIST.FindByCode(gameCode);
    if (NULL != pGame)
    {
        gameId = pGame->getId();
    }

    // 获得channel 数据
    Json::Value channel_array;
    channel_array.resize(0);
    CChannelGroup* pChannelGroup = CHANNELGROUP_LIST.FindById(groupId);
    if (pChannelGroup)
    {
        GroupVipRange* pVipRange = pChannelGroup->FindValidVipRange(vipLevel);
        if (pVipRange)
        {
            LogMsg("[%s:%s:%d] vipLevel:%d has valid vipRange in groupId:%d, vipRange=%s", __FILE__, __func__, __LINE__, vipLevel, groupId, pVipRange->strVipRange.c_str());
            vector<uint32_t>& vecConfigId = pVipRange->vecConfigId;
            for (size_t i = 0; i < vecConfigId.size(); ++i)
            {
                int32_t nConfigId = vecConfigId[i];
                LogMsg("[%s:%s:%d] vipLevel:%d has valid vipRange in groupId:%d, vipRange=%s, configId=%d", __FILE__, __func__, __LINE__, vipLevel, groupId, pVipRange->strVipRange.c_str(), nConfigId);
                VEC_PAYCHANNEL::iterator it(m_vecPayChannel.begin());
                for(; it != m_vecPayChannel.end(); ++it)
                {
                    CPayChannel &channel = *it;
                    if (channel.getConfigId() == nConfigId)
                    {
                        channel.toJson(gameCode, uin, clientType, version, channelCode, vipLevel, category, nConfigId, groupId, channel_array);
                    }
                }
            }
        }
        else
        {
            LogWarn("[%s:%s:%d] vipLevel:%d not has valid vipRange in groupId:%d", __FILE__, __func__, __LINE__, vipLevel, groupId);
        }
    }

    resp["channel"] = channel_array;

    // 获得 vip 数据
    Json::Value vip_array;
    vip_array.resize(0);
    VIP_LIST.toJson(gameId, vip_array);
    resp["vip"] = vip_array;

}

