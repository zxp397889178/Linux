/*
 * =====================================================================================
 *
 *       Filename:  GameList.cpp
 *
 *    Description:  游戏列表
 *
 *        Version:  1.0
 *        Created:  2020年07月23日 23时17分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "GameList.h"

CGameList::CGameList()
{
}

CGameList::~CGameList()
{
    m_vecGame.clear();
}

int32_t CGameList::Load(ISimDB *pDB)
{
    m_vecGame.clear();

    pDB->Cmd("SELECT `gameId`,`code`,`name`,`account`,`apiKey`,`apiNotifyUrl`,`apiTranferUrl`, `callbackUrl`, `alertData`, `exchangeRate` ");
    pDB->Cmd("FROM `t_game` ");
    pDB->Cmd("WHERE `status`=1 ");
    while(pDB->More())
    {
        CGame gameItem(pDB->GetInt(0),  // gameId
                pDB->GetString(1),      // 游服代码
                pDB->GetString(2),      // 游服名称
                pDB->GetString(3),      // 通讯Account
                pDB->GetString(4),      // 通讯KEY
                pDB->GetString(5),      // 充值成功后通知URL
                pDB->GetString(6),      // 提现成功后的通知URL
                pDB->GetString(7),      // 充值结束后回调的URL，H5用
                pDB->GetString(8),      // 警告数据，以JSON格式保存
                pDB->GetInt(9)          // 1RMB兑换多少COIN
            );
        m_vecGame.push_back(gameItem);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query t_game data failure, error %s", szError);
        return -1;
    }

    return 0; 
}

CGame* CGameList::Find(int32_t gameId)
{
    std::vector<CGame>::iterator it(m_vecGame.begin());
    for(; it != m_vecGame.end(); ++it)
    {
        CGame &item = *it;
        if (item.getId() == gameId)
        {
            return &item;
        }
    }
    return NULL;
}

CGame* CGameList::FindByCode(const CMyString& gameCode)
{
    std::vector<CGame>::iterator it(m_vecGame.begin());
    for(; it != m_vecGame.end(); ++it)
    {
        CGame &item = *it;
        if (item.getCode() == gameCode)
        {
            return &item;
        }
    }
    return NULL;
}

CGameList& CGameList::InstanceGet()
{
    static CGameList s_obj;
    return s_obj;
}

