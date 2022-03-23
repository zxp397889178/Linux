/*
 * =====================================================================================
 *
 *       Filename:  Dict_ClientType.cpp
 *
 *    Description:  前端类型字典
 *
 *        Version:  1.0
 *        Created:  2021年03月05日 15时18分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Dict_ClientType.h"

CDictClientType::CDictClientType()
{

}

CDictClientType::~CDictClientType()
{

}

CDictClientType& CDictClientType::InstanceGet()
{
    static CDictClientType s_obj;
    return s_obj;
}

int32_t CDictClientType::Load(ISimDB *pDB)
{
    m_vecClientType.clear();

    pDB->Cmd("SELECT`code`,`type` FROM `dict_clienttype`;");
    while(pDB->More())
    {
        Item item;
        item.code = pDB->GetString(0);
        item.id = pDB->GetInt(1);
        m_vecClientType.push_back(item);
    }
    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("query dict_clienttype data failure, error %s", szError);
        return -1;
    }

    return 0;
}

const CMyString CDictClientType::getCode(int32_t clientTypeId)
{
    VEC_CLIENT_TYPE::iterator it(m_vecClientType.begin());
    for(; it != m_vecClientType.end(); ++it)
    {
        Item &item = *it;
        if (item.id == clientTypeId)
        {
            return item.code;
        }
    }
    return "";
}

int32_t CDictClientType::getTypeId(const CMyString &clientTypeCode)
{
    VEC_CLIENT_TYPE::iterator it(m_vecClientType.begin());
    for(; it != m_vecClientType.end(); ++it)
    {
        Item &item = *it;
        if (item.code == clientTypeCode)
        {
            return item.id;
        }
    }
    return 0;  // 未找到一律认为是通用
}

