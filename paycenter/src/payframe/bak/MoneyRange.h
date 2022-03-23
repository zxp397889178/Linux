/*
 * =====================================================================================
 *
 *       Filename:  MoneyRange.h
 *
 *    Description:  检查金额范围
 *
 *        Version:  1.0
 *        Created:  02/07/2020 01:24:14 PM
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
#include <vector>

class CMoneyRange
{
public:
    CMoneyRange();
    ~CMoneyRange();

    bool Init(const CMyString &str);

public:
    // 以分为单位进行校验
    bool Valid(int64_t coin);

private:
    std::vector<int64_t>    m_vecFixCoin;
    struct RangeItem
    {
        int64_t     minCoin;
        int64_t     maxCoin;
    };
    std::vector<RangeItem>  m_vecRangeCoin;
};
