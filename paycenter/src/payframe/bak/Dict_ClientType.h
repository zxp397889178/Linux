/*
 * =====================================================================================
 *
 *       Filename:  Dict_ClientType.h
 *
 *    Description:  前端类型字典
 *
 *        Version:  1.0
 *        Created:  2021年03月05日 15时07分15秒
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

class CDictClientType
{
    struct Item
    {
        CMyString   code;
        int32_t     id;
    };
    typedef std::vector<Item>   VEC_CLIENT_TYPE;
public:
    ~CDictClientType();
    static CDictClientType& InstanceGet();

    int32_t Load(ISimDB *pDB);

    // 返回 空字符串表示无此类型ID
    const CMyString getCode(int32_t clientTypeId);
    // 返回 0 表示无此类型
    int32_t getTypeId(const CMyString& clientTypeCode);

private:
    CDictClientType();

private:
    VEC_CLIENT_TYPE     m_vecClientType;
};

#define DICT_CLIENTTYPE         CDictClientType::InstanceGet()


