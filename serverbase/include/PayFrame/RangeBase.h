/*
 * =====================================================================================
 *
 *       Filename:  RangeBase.h
 *
 *    Description:  数值范围的取值
 *
 *        Version:  1.0
 *        Created:  2021年04月22日 21时53分50秒
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
#include <set>

class CRangeBase
{
public:
    CRangeBase();
    virtual ~CRangeBase();

    // 解析str,返回值:0 表示空字符串，>0表示解析出的组数,<0出错
    int32_t Load(const CMyString &str, const char cGroupSplit=',', const char cSplit='-');

public:
    // 校验val是否在设置的范围
    bool InRange(int64_t val);
    int64_t getStartVal(void);
    int64_t getEndVal(void);
    // 格式化输出值范围
    CMyString toString(const char cGroupSplit=',', const char cSplit='-');

    bool empty(void);
    bool Valid(int64_t val);

private:
    struct RangeItem
    {
        int64_t     startVal;
        int64_t     endVal;
    };
    std::set<int64_t>       m_setFixed;
    std::vector<RangeItem>  m_vecRange;
};

