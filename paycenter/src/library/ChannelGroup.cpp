/*
 * =====================================================================================
 *
 *       Filename:  ChannelGroup.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年12月20日 17时44分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "ChannelGroup.h"
#include "ChannelList.h"
#include "SdkChannelList.h"
#include <algorithm>

extern CGame* GetGameByCode(const CMyString &gameCode, Json::Value &resp);

CChannelGroup::CChannelGroup()
{

}

CChannelGroup::CChannelGroup(int32_t nId, const CMyString &strChannelGroup,
            const CMyString &strGameCode, const CMyString &strGroupName, int32_t nStatus)
{
    m_nGroupId = nId;
    m_strChannelGroup = strChannelGroup;
    m_strGameCode = strGameCode;
    m_strGroupName = strGroupName;
    m_nStatus = nStatus;
}

CChannelGroup::~CChannelGroup()
{

}

void CChannelGroup::AddChannel(ChannelInfo& channelInfo)
{
    m_vecChannel.push_back(channelInfo);
}

void CChannelGroup::AddGroupVipRange(GroupVipRange& groupVipRange)
{
    m_vecGroupVipRange.push_back(groupVipRange);
}

GroupVipRange* CChannelGroup::FindValidVipRange(int32_t vipLevel)
{
    GroupVipRange* pVipRange = NULL;
    std::vector<GroupVipRange>::iterator iter = m_vecGroupVipRange.begin();
    for (; iter != m_vecGroupVipRange.end(); ++iter)
    {
        GroupVipRange& groupVipRange = *iter;
        CMyString strVipRange = groupVipRange.strVipRange;
        if (IsValidVipRange(strVipRange, vipLevel))
        {
            pVipRange = &groupVipRange;
            break;
        }
    }
    return pVipRange;
}

ChannelInfo* CChannelGroup::FindChannelInfo(const CMyString& strChannel)
{
    ChannelInfo* pChannelInfo = NULL;
    std::vector<ChannelInfo>::iterator iter = m_vecChannel.begin();
    for (; iter != m_vecChannel.end(); ++iter)
    {
        ChannelInfo& channelInfo = *iter;
        if (channelInfo.strChannel == strChannel)
        {
            pChannelInfo = &channelInfo;
            break;
        }
    }
    return pChannelInfo;
}

bool CChannelGroup::IsValidVipRange(CMyString strVipRange, int32_t vipLevel)
{
    std::vector<uint32_t> vecGroupVip;
    safe_strtok(strVipRange.c_str(), vecGroupVip, "-");
    if (vecGroupVip.size() == 1)
    {
        int32_t nVip = vecGroupVip[0];
        LogDebug("[%s:%s:%d] nVip=%d ", __FILE__, __func__, __LINE__, nVip);
        if (nVip == vipLevel)
        {
            return true;
        }
    }
    else if (vecGroupVip.size() == 2)
    {
        int32_t nVipStart = vecGroupVip[0];
        int32_t nVipEnd = vecGroupVip[1];
        LogDebug("[%s:%s:%d] nVipStart=%d, nVipEnd=%d ", __FILE__, __func__, __LINE__, nVipStart, nVipEnd);
        if (vipLevel >= nVipStart && vipLevel <= nVipEnd)
        {
            return true;
        }
    }

    return false;
}





CChannelGroupList::CChannelGroupList()
{

}

CChannelGroupList::~CChannelGroupList()
{

}

CChannelGroupList& CChannelGroupList::InstanceGet()
{
    static CChannelGroupList s_obj;
    return s_obj;
}

int32_t CChannelGroupList::Load(ISimDB *pDB)
{
    m_vecChannelGroup.clear();

    pDB->Cmd("SELECT `groupId`,`channel_group`,`gameCode`,`group_name`,`status` FROM `t_channelgroup`;");
    while(pDB->More())
    {
        CChannelGroup channelgroup( pDB->GetInt(0),    // groupId
                pDB->GetString(1),      // channel_group
                pDB->GetString(2),      // gameCode
                pDB->GetString(3),      // note
                pDB->GetInt(4)          // status
                );
        m_vecChannelGroup.push_back(channelgroup);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_channelgroup data failure, error %s", szError);
        return -1;
    }

    std::vector<ChannelInfo> vecChannelInfo;
    pDB->Cmd("SELECT `id`,`channel`,`channel_groupId`,`note`,`platform` FROM `t_channelwithgroup`;");
    while(pDB->More())
    {
        ChannelInfo channelInfo;
        channelInfo.nChannelId = pDB->GetInt(0);
        channelInfo.strChannel = pDB->GetString(1);
        channelInfo.nChannelGroupId = pDB->GetInt(2);
        channelInfo.strNote = pDB->GetString(3);
        channelInfo.strPlatform = pDB->GetString(4);

        vecChannelInfo.push_back(channelInfo);
    }
    LogDebug("[%s:%s:%d] vecChannelInfo.size:%lu", __FILE__, __func__, __LINE__, vecChannelInfo.size());
    err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_channelwithgroup data failure, error %s", szError);
        return -1;
    }

    std::vector<GroupVipRange> vecGroupVipRange;
    pDB->Cmd("SELECT `id`,`channel_groupId`,`channel_group`,`vipRange`,`status` FROM `t_channelwithvip`;");
    while(pDB->More())
    {
        GroupVipRange vipRangeInfo;
        vipRangeInfo.nId = pDB->GetInt(0);
        vipRangeInfo.nChannelGroupId = pDB->GetInt(1);
        vipRangeInfo.strChannelGroup = pDB->GetString(2);
        vipRangeInfo.strVipRange = pDB->GetString(3);
        vipRangeInfo.nStatus = pDB->GetInt(4);

        vecGroupVipRange.push_back(vipRangeInfo);
    }
    LogDebug("[%s:%s:%d] vecGroupVipRange.size:%lu", __FILE__, __func__, __LINE__, vecGroupVipRange.size());
    err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_channelwithgroup data failure, error %s", szError);
        return -1;
    }

    std::map<int32_t, vector<uint32_t> > mapVipConfig;
    pDB->Cmd("SELECT `vipRangeId`,`configId` FROM `t_channelvipwithconfig`;");
    while(pDB->More())
    {
        int32_t nVipRangeId = pDB->GetInt(0);
        uint32_t nConfigId = pDB->GetInt(1);

        std::map<int32_t, vector<uint32_t> >::iterator iter_find = mapVipConfig.find(nVipRangeId);
        if (iter_find != mapVipConfig.end())
        {
            vector<uint32_t>& vecConfigId = iter_find->second;
            vecConfigId.push_back(nConfigId);
        }
        else
        {
            vector<uint32_t> vecConfigId;
            vecConfigId.push_back(nConfigId);
            mapVipConfig[nVipRangeId] = vecConfigId;
        }
    }
    LogDebug("[%s:%s:%d] mapVipConfig.size:%lu", __FILE__, __func__, __LINE__, mapVipConfig.size());
    err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_channelvipwithconfig data failure, error %s", szError);
        return -1;
    }

    for (size_t i = 0; i < vecGroupVipRange.size(); ++i)
    {
        GroupVipRange& vipRangeInfo = vecGroupVipRange[i];
        std::map<int32_t, vector<uint32_t> >::iterator iter_find = mapVipConfig.find(vipRangeInfo.nId);
        if (iter_find != mapVipConfig.end())
        {
            vipRangeInfo.vecConfigId = iter_find->second;
        }
    }

    for (size_t i = 0; i < m_vecChannelGroup.size(); ++i)
    {
        CChannelGroup& channelGroup = m_vecChannelGroup[i];
        for (size_t j = 0; j < vecChannelInfo.size(); ++j)
        {
            ChannelInfo& channelInfo = vecChannelInfo[j];
            if (channelInfo.nChannelGroupId == channelGroup.getGroupId())
            {
                channelGroup.AddChannel(channelInfo);
            }
        }

        for (size_t j = 0; j < vecGroupVipRange.size(); ++j)
        {
            GroupVipRange& vipRangeInfo = vecGroupVipRange[j];
            if (vipRangeInfo.nChannelGroupId == channelGroup.getGroupId())
            {
                channelGroup.AddGroupVipRange(vipRangeInfo);
            }
        }
    }

    return 0;
}

CChannelGroup* CChannelGroupList::FindById(int32_t nId)
{
    CChannelGroup *pChannelGroup(NULL);
    for (size_t i = 0; i < m_vecChannelGroup.size(); ++i)
    {
        CChannelGroup& channelGroup = m_vecChannelGroup[i];
        if (channelGroup.getGroupId() == nId)
        {
            pChannelGroup = &channelGroup;
            break;
        }
    }
    return pChannelGroup;
}

CChannelGroup* CChannelGroupList::FindByGroupName(const CMyString &strChannelGroup)
{
    CChannelGroup *pChannelGroup(NULL);
    for (size_t i = 0; i < m_vecChannelGroup.size(); ++i)
    {
        CChannelGroup& channelGroup = m_vecChannelGroup[i];
        if (channelGroup.getChannelGroup() == strChannelGroup)
        {
            pChannelGroup = &channelGroup;
            break;
        }
    }
    return pChannelGroup;
}

CChannelGroup* CChannelGroupList::FindByChannel(const CMyString &strChannel)
{
    CChannelGroup *pChannelGroup(NULL);
    for (size_t i = 0; i < m_vecChannelGroup.size(); ++i)
    {
        CChannelGroup& channelGroup = m_vecChannelGroup[i];
        if (channelGroup.FindChannelInfo(strChannel) != NULL)
        {
            pChannelGroup = &channelGroup;
            break;
        }
    }
    return pChannelGroup;
}
