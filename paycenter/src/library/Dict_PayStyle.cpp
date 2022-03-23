/*
 * =====================================================================================
 *
 *       Filename:  Dict_PayStyle.cpp
 *
 *    Description:  支付类型管理
 *
 *        Version:  1.0
 *        Created:  2021年07月01日 11时30分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Dict_PayStyle.h"

CDictPayStyle::CDictPayStyle()
{
}

CDictPayStyle::~CDictPayStyle()
{
}

CDictPayStyle& CDictPayStyle::InstanceGet()
{
    static CDictPayStyle s_obj;
    return s_obj;
}

int32_t CDictPayStyle::Load(ISimDB *pDB)
{
    m_vecPayStyle.clear();
    pDB->Cmd("select `gameId`,`gameCode`,`code`,`style` from `dict_paystyle`; ");
    while(pDB->More())
    {
        Item item;
        item.gameId = pDB->GetInt(0);
        item.gameCode = pDB->GetString(1);
        item.code = pDB->GetString(2);
        item.payStyle = pDB->GetInt(3);

        m_vecPayStyle.push_back(item);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query dict_paystyle data failure, error %s", szError);
        return -1;
    }

    return 0;
}

bool CDictPayStyle::getPayStyle(int32_t gameId, const CMyString &code, int32_t &payStyle)
{
    bool bFind(false);
    VEC_PAYSTYLE::iterator it(m_vecPayStyle.begin());
    for(; it != m_vecPayStyle.end(); ++it)
    {
        Item &item = *it;
        if (item.gameId == gameId && item.code==code)
        {
            payStyle = item.payStyle;
            bFind = true;
            break;
        }
    }
    return bFind;
}

bool CDictPayStyle::getPayStyleByCode(const CMyString &gameCode, const CMyString &code, int32_t &payStyle)
{
    bool bFind(false);
    VEC_PAYSTYLE::iterator it(m_vecPayStyle.begin());
    for(; it != m_vecPayStyle.end(); ++it)
    {
        Item &item = *it;
        if (item.gameCode == gameCode && item.code==code)
        {
            payStyle = item.payStyle;
            bFind = true;
            break;
        }
    }
    return bFind;
}


