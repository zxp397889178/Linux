/*
 * =====================================================================================
 *
 *       Filename:  Dict_PayStyle.h
 *
 *    Description:  支付类型字典
 *
 *        Version:  1.0
 *        Created:  2021年07月01日 11时21分47秒
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

enum EnumPaStyle
{
    psDummy = 0,    // 出错
    psNative,       // 原生
    psH5,           // H5
    psGM,           // 人工支付
};

class CDictPayStyle
{
    struct Item
    {
        CMyString       code;
        int32_t         payStyle;   // 对应EnumPayStyle
        int32_t         gameId;
        CMyString       gameCode;
    };
    typedef std::vector<Item>       VEC_PAYSTYLE;
public:
    ~CDictPayStyle();
    static CDictPayStyle& InstanceGet();

    int32_t Load(ISimDB *pDB);
    bool getPayStyle(int32_t gameId, const CMyString &code, int32_t &payStyle);
    bool getPayStyleByCode(const CMyString &gameCode, const CMyString &code, int32_t &payStyle);

private:
    CDictPayStyle();

private:
    VEC_PAYSTYLE        m_vecPayStyle;
};

#define DICT_PAYSTYLE       CDictPayStyle::InstanceGet()

