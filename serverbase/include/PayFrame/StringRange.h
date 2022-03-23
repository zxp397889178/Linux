/*
 * =====================================================================================
 *
 *       Filename:  StringRange.h
 *
 *    Description:  字符串的范围，支持正则表达式
 *
 *        Version:  1.0
 *        Created:  2021年04月22日 23时05分45秒
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

class CStringRange
{
public:
    CStringRange();
    ~CStringRange();

    int32_t Load(const CMyString &str, const char cSplit=',');
    bool Match(const CMyString &str);
    CMyString toString(const char cSplit=',');

    bool empty(void);
    bool Valid(const CMyString &val);

private:
    std::set<CMyString>  m_StringList;
};



