/*
 * =====================================================================================
 *
 *       Filename:  VipList.cpp
 *
 *    Description:  人工充值客服管理
 *
 *        Version:  1.0
 *        Created:  2021年03月05日 01时17分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "VipList.h"

CVip::CVip(int32_t vipId, int32_t gameId, int32_t nameId, int32_t descId,
            const CMyString &iconUrl, int32_t vipFlag, int32_t enumFlag, 
            int32_t payFlag, const CMyString &url)
    : m_vipId(vipId)
    , m_gameId(gameId)
{
    m_data["vipId"] = vipId;
    m_data["nameId"] = nameId;
    m_data["descId"] = descId;
    m_data["iconUrl"] = iconUrl;
    m_data["flag"] = vipFlag;
    m_data["enumFlag"] = enumFlag;
    m_data["payFlag"] = payFlag;
    m_data["vipUrl"] = url;
}

CVip::~CVip()
{
}

void CVip::toJson(Json::Value &vip_array)
{
    vip_array.append(m_data);
}

////////////////////////////////////////////////////////////////////////
CVipList::CVipList()
{

}

CVipList::~CVipList()
{
}

CVipList& CVipList::InstanceGet()
{
    static CVipList s_obj;
    return s_obj;
}

int32_t CVipList::Load(ISimDB *pDB)
{
    m_vecVip.clear();

    pDB->Cmd("SELECT `id`,`gameId`,`nameId`,`descId`,`icon`,`vipFlag`,`enumFlag`,`payFlag`,`url` ");
    pDB->Cmd("FROM `t_vip` WHERE `status`=1");
    while(pDB->More())
    {
        CVip vip(pDB->GetInt(0),    // vipId
                pDB->GetInt(1),     // gameId
                pDB->GetInt(2),     // nameId
                pDB->GetInt(3),     // descId
                pDB->GetString(4),  // iconUrl
                pDB->GetInt(5),     // vipFlag
                pDB->GetInt(6),     // enumFlag
                pDB->GetInt(7),     // payFlag
                pDB->GetString(8)   // url
                );
        m_vecVip.push_back(vip);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_vip data failure, error %s", szError);
        return -1;
    }
    return 0;
}

CVip* CVipList::Find(int32_t vipId)
{
    CVip *pVip(NULL);
    VEC_VIP::iterator it(m_vecVip.begin());
    for(; it != m_vecVip.end(); ++it)
    {
        CVip &vip = *it;
        if (vip.getId() == vipId)
        {
            pVip = &vip;
            break;
        }
    }
    return pVip;
}

int32_t CVipList::toJson(int32_t gameId, Json::Value &vip_array)
{
    vip_array.resize(0);
    VEC_VIP::iterator it(m_vecVip.begin());
    for(; it != m_vecVip.end(); ++it)
    {
        CVip &vip = *it;
        if (vip.getGameId() != gameId)
        {
            continue;
        }
        vip.toJson(vip_array);
    }
    return static_cast<int32_t>(vip_array.size());
}
