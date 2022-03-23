/*
 * =====================================================================================
 *
 *       Filename:  BillPointList.h
 *
 *    Description:  计费点列表管理
 *
 *        Version:  1.0
 *        Created:  2021年03月04日 23时27分00秒
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
#include "RangeBase.h"
#include "StringRange.h"

class CBillPoint
{
public:
    CBillPoint(int32_t bpId, int32_t configId, const CMyString &bpCode,
            int64_t billpoint, int64_t coin, int64_t extCoin, const CMyString &itemInfo,
            int32_t initCount, const CMyString &bpIconUrl, int32_t bpNameId,
            const CMyString &versionRange, const CMyString &channelRange, const CMyString &vipRange,
            int32_t dailyCount, int32_t monthlyCount, int32_t discount, const CMyString &category,
            int32_t pos, const CMyString &extraInfo);
    ~CBillPoint();

    int32_t getId(void)        { return m_bpId; }
    int32_t getInitCount(void) { return m_initCount; }
    int32_t getConfigId(void)  { return m_configId; }
    int64_t getBillPoint(void) { return m_billPoint; }
    int64_t getCoin(void)      { return m_coin; }
    int64_t getExtCoin(void)   { return m_extCoin; }
    const CMyString& getBpCode(void)        { return m_bpCode; }
    const CMyString& getBpItemData(void)    { return m_bpItemData; }
    const CMyString& getCategory(void)      { return m_category; }
    const CMyString& getExtraInfo(void)     { return m_extraInfo; }

    bool toJson(const CMyString &uin, Json::Value &billpoint_array);
    bool IsValid(int64_t version, const CMyString &channel, int32_t vip);
    bool CanBuy(const CMyString &uin);

private:
    int32_t         m_bpId;
    int32_t         m_configId;         // t_channel 中的ID, 1..n
    CMyString       m_bpCode;           // 计费点名称，主键,2021-3-28
    int32_t         m_initCount;        // 赠送次数
    int64_t         m_billPoint;        // 计费点金额
    int64_t         m_coin;             // 对应的金币
    int64_t         m_extCoin;          // 赠送金额
    CMyString       m_category;         // 分类

    CMyString       m_bpItemData;       // 物品列表数据，按itemId:count;... 组成
    CMyString       m_extraInfo;        // 扩展数据，以json表达
    Json::Value     m_data;

private:
    CRangeBase      m_version;
    CRangeBase      m_vip;
    CStringRange    m_channel;
    int32_t         m_dailyCount;
    int32_t         m_monthlyCount;
    int32_t         m_totalCount;
};

class CBillPointList
{
    typedef std::vector<CBillPoint>         VEC_BILLPOINT;
public:
    ~CBillPointList();
    static CBillPointList& InstanceGet();

    int32_t Load(ISimDB *pDB);

    CBillPoint* Find(int32_t bpId);

    // 查找configId 下匹配money的计费点
    int32_t Find(int32_t configId, int64_t money, std::vector<CBillPoint*> &vecBillPoint);

    CBillPoint* FindByCode(int32_t configId, const CMyString &bpCode);
    CBillPoint* FindById(int32_t bpId);

    // 判断configId 下是否有计费点
    //int32_t GetCount(int32_t configId, const CMyString &uin);

    // 判断configId 下是否有可用的计费点
    bool IsValid(int32_t configId, const CMyString &uin, int64_t version, 
            const CMyString &channelCode, int32_t vipLevel);

    /* 返回指定分类的计费眯
     * 参数：[IN]  configId         指定计费类目
     *       [IN]  uin              当前用户的uin，用于过滤限制购买次数的计费点
     *       [IN]  version          当前用户的版本号
     *       [IN]  channelCode      当前用户的渠道编码
     *       [IN]  vipLevel         当前用户的vip等级
     *       [IN]  category         指定分类，若不指定则全选
     *       [OUR] billpoint_array  导出的计费点列表
     * 返回值：添加到 billpoint_array中计费点的数量
     */
    int32_t toJson(int32_t configId, const CMyString &uin, int64_t version,
            const CMyString &channelCode, int32_t vipLevel, const CMyString &category,  
            Json::Value &billpoint_array);

private:
    CBillPointList();

private:
    VEC_BILLPOINT       m_vecBillPoint;

};

#define BILLPOINT_LIST      CBillPointList::InstanceGet()


