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
#include "RangeBase.h"

class CMoneyRange : public CRangeBase
{
public:
    CMoneyRange();
    ~CMoneyRange();

    bool Init(const CMyString &str);

public:
    // 以分为单位进行校验
    bool Valid(int64_t coin);

    // 获得最小，最大金额
    int64_t getMoneyMin(void);
    int64_t getMoneyMax(void);
};
