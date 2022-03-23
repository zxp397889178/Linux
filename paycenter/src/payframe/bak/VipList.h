/*
 * =====================================================================================
 *
 *       Filename:  VipList.h
 *
 *    Description:  人工充值客服管理
 *
 *        Version:  1.0
 *        Created:  2021年03月05日 01时16分56秒
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
#include <map>
#include <vector>

class CVip
{
public:
    CVip(int32_t vipId, int32_t gameId, int32_t nameId, int32_t descId,
            const CMyString &iconUrl, int32_t vipFlag, int32_t enumFlag, 
            int32_t payFlag, const CMyString &url);
    ~CVip();

    int32_t getId(void)     { return m_vipId; }
    int32_t getGameId(void) { return m_gameId; }
    void toJson(Json::Value &vip_array);

private:
    int32_t         m_vipId;
    int32_t         m_gameId;
    Json::Value     m_data;
};

class CVipList
{
    typedef std::vector<CVip>   VEC_VIP;
public:
    CVipList();
    ~CVipList();
    static CVipList& InstanceGet();

    int32_t Load(ISimDB *pDB);
    CVip* Find(int32_t vipId);

    int32_t toJson(int32_t gameId, Json::Value &vip_array);

private:
    VEC_VIP     m_vecVip;
};

#define VIP_LIST    CVipList::InstanceGet()


