/*
 * =====================================================================================
 *
 *       Filename:  TimeRange.h
 *
 *    Description:  可用时间段函数
 *
 *        Version:  1.0
 *        Created:  02/07/2020 01:57:21 PM
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

class CTimeRange
{
public:
    CTimeRange();
    ~CTimeRange();

    bool Init(const CMyString &str);

    bool Valid(time_t &t1);

public:
    struct TimeRange
    {
        int64_t     begin;
        int64_t     end;
    };
    std::vector<TimeRange>  m_vecTime;
};
