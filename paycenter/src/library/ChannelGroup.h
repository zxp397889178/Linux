/*
 * =====================================================================================
 *
 *       Filename:  ChannelGroup.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年12月20日 17时43分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"
#include "MyNet.h"
#include <vector>

class CGame;

struct ChannelInfo
{   
    int32_t nChannelId;
    int32_t nChannelGroupId;
    CMyString strChannel;
    CMyString strNote;
    CMyString strPlatform;

    ChannelInfo() : nChannelId(0), nChannelGroupId(0) {}
};

struct GroupVipRange
{   
    int32_t nId;
    int32_t nChannelGroupId;
    CMyString strChannelGroup;
    CMyString strVipRange;
    int32_t nStatus;
    vector<uint32_t> vecConfigId;

    GroupVipRange() : nId(0), nChannelGroupId(0), nStatus(0) {}
};

class CChannelGroup
{
public:
    CChannelGroup();
    CChannelGroup(int32_t nId, const CMyString &strChannelGroup,
            const CMyString &strGameCode, const CMyString &strGroupName, int32_t nStatus);
    ~CChannelGroup();

    int32_t getGroupId(void)   { return m_nGroupId; }
    const CMyString& getChannelGroup(void) { return m_strChannelGroup; }
    const CMyString& getGroupName(void) { return m_strGroupName; }
    const CMyString getGameCode(void) { return m_strGameCode; }
    std::vector<ChannelInfo>& getVecChannel(void) { return m_vecChannel; }
    int32_t getStatus(void) { return m_nStatus; }
     
    void AddChannel(ChannelInfo& channelInfo);
    void AddGroupVipRange(GroupVipRange& groupVipRange);
    GroupVipRange* FindValidVipRange(int32_t vipLevel);
    ChannelInfo* FindChannelInfo(const CMyString& strChannel);

private:
    bool IsValidVipRange(CMyString strVipRange, int32_t vipLevel);

private:
    int32_t         m_nGroupId;     // 配置ID, 主键
    Json::Value     m_data;

    CMyString       m_strChannelGroup;  // 渠道组名
    CMyString       m_strGameCode;      // 游戏code
    CMyString       m_strGroupName;          // 渠道组说明
    int32_t         m_nStatus;          // 渠道组状态

    std::vector<ChannelInfo> m_vecChannel; //渠道组下的渠道
    std::vector<GroupVipRange> m_vecGroupVipRange; //渠道组下的vip范围信息
};

class CChannelGroupList
{
    typedef std::vector<CChannelGroup>        VEC_CHANNELGROUP;
public:
    ~CChannelGroupList();
    static CChannelGroupList& InstanceGet();

    int32_t Load(ISimDB *pDB);
    CChannelGroup* FindById(int32_t nId);
    CChannelGroup* FindByGroupName(const CMyString &strChannelGroup);
    CChannelGroup* FindByChannel(const CMyString &strChannel);

private:
    CChannelGroupList();

private:
    VEC_CHANNELGROUP      m_vecChannelGroup;
};

#define CHANNELGROUP_LIST     CChannelGroupList::InstanceGet()