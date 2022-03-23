/*
 * =====================================================================================
 *
 *       Filename:  ChannelList.h
 *
 *    Description:  对标 t_channel, 前端交互的主要入口
 *
 *        Version:  1.0
 *        Created:  2021年03月03日 23时31分39秒
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
class CGame;

class CPayChannel
{
public:
    CPayChannel(int32_t configId, int32_t gameId, const CMyString &payStyle,
            const CMyString &moneyPoint, int64_t moneyBegin, int64_t moneyEnd,
            int32_t manualFlag, int32_t flag, const CMyString &iconUrl, 
            int32_t sortOrder, int32_t nameId);
    ~CPayChannel();

    int32_t getConfigId(void)   { return m_configId; }
    const CMyString& getPayStyle(void) { return m_payStyle; }
     
    bool IsValid(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType);
    void toJson(Json::Value &channal_array);

private:
    int32_t         m_configId;     // 配置ID, 主键
    CMoneyRange     m_billPoint;    // 计费点
    CGame*          m_pGame;
    CMyString       m_payStyle;     // 支付方式
    Json::Value     m_data;

    int32_t         m_nameLangId;   // 显示名称的语言ID
    CMyString       m_iconUrl;      // 图标的URL
    int64_t         m_moneyBegin;   // 起始金额，以元显示
    int64_t         m_moneyEnd;     // 结束金额，以元显示
    int32_t         m_manualFlag;   // 是否允许手工输入金额
    int32_t         m_flag;         // 其他标识，如推荐等，参见dict_flag
    int32_t         m_sortOrder;    // 排序号,用于界面上的显示
};

class CPayChannelList
{
    typedef std::vector<CPayChannel>        VEC_PAYCHANNEL;
public:
    ~CPayChannelList();
    static CPayChannelList& InstanceGet();

    int32_t Load(ISimDB *pDB);
    CPayChannel* Find(int32_t configId);

    // 查询符合要求的channel数据，封装到resp数据中去
    void toJson(const CMyString &gameCode, const CMyString &uin, const CMyString &clientType, Json::Value &resp);


private:
    CPayChannelList();

private:
    VEC_PAYCHANNEL      m_vecPayChannel;
};

#define CHANNEL_LIST     CPayChannelList::InstanceGet()




