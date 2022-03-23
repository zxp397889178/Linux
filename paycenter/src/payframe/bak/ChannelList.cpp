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

CPayChannel::CPayChannel(int32_t configId, int32_t gameId, const CMyString &payStyle,
    const CMyString &moneyPoint, int64_t moneyBegin, int64_t moneyEnd,
    int32_t manualFlag, int32_t flag, const CMyString &iconUrl, 
    int32_t sortOrder, int32_t nameId)
    : m_configId(configId)
    , m_payStyle(payStyle)
    , m_nameLangId(nameId)
    , m_iconUrl(iconUrl)
    , m_moneyBegin(moneyBegin)
    , m_moneyEnd(moneyEnd)
    , m_manualFlag(manualFlag)
    , m_flag(flag)
    , m_sortOrder(sortOrder)
{
    m_billPoint.Init(moneyPoint);
    m_pGame = GAME_LIST.Find(gameId);

    // 生成Json 数据项
    m_data["configId"] = configId;
    m_data["sortOrder"] = sortOrder;
    m_data["nameId"] = nameId;
    m_data["payStyle"] = payStyle;
    m_data["billPoint"] = moneyPoint;
    m_data["moneyBegin"] = (Json::Int64)moneyBegin;
    m_data["moneyEnd"] = (Json::Int64)moneyEnd;
    m_data["manualMoney"] = manualFlag;
    m_data["flag"] = flag;
    m_data["iconUrl"] = iconUrl;
}

CPayChannel::~CPayChannel()
{

}

bool CPayChannel::IsValid(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType)
{
    // 检查游服是否匹配
    if (NULL==m_pGame || m_pGame->getCode()!= gameCode)
    {
        return false;
    }
    
    // TODO: 检查sdk_channel 是否支持本clientType

    
    // TODO: 检查 billpoint ,本玩家是否还有计费点
    

    return true;
}

void CPayChannel::toJson(Json::Value &channel_array)
{
    channel_array.append(m_data);
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

    pDB->Cmd("SELECT `configId`,`gameId`,`payStyleCode`,`moneyPoint`,"\
            "`moneyBegin`,`moneyEnd`,`manualFlag`,`flag`,`icon`,"\
            "`sortOrder`,`nameId` ");
    pDB->Cmd("FROM `t_channel` WHERE `status`=1 ");
    while(pDB->More())
    {
        CPayChannel channel( pDB->GetInt(0),    // configId
                pDB->GetInt(1),         // gameId
                pDB->GetString(2),      // payStyle
                pDB->GetString(3),      // moneyPoints
                pDB->GetInt64(4),       // moneyBegin
                pDB->GetInt64(5),       // moneyEnd
                pDB->GetInt(6),         // manualFlag
                pDB->GetInt(7),         // flag
                pDB->GetString(8),      // icon
                pDB->GetInt(9),         // sortOrder
                pDB->GetInt(10)         // nameId
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


void CPayChannelList::toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, Json::Value &resp)
{
    std::vector<int32_t>    vecConfigId;
    // 获得channel 数据
    Json::Value channel_array;
    channel_array.resize(0);
    VEC_PAYCHANNEL::iterator it(m_vecPayChannel.begin());
    for(; it != m_vecPayChannel.end(); ++it)
    {
        CPayChannel &channel = *it;
        if (channel.IsValid(gameCode, uin, clientType))
        {
            channel.toJson(channel_array);
            vecConfigId.push_back(channel.getConfigId());
        }
    }
    resp["channel"] = channel_array;

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

    Json::Value vip_array;
    vip_array.resize(0);
    // TODO: 获得 vip 数据
    resp["vip"] = vip_array;

}


