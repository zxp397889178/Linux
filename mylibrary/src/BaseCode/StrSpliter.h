/*
 * =====================================================================================
 *
 *       Filename:  StrSpliter.h
 *
 *    Description:  对字符串做split分隔处理
 *
 *        Version:  1.0
 *        Created:  2020年02月11日 20时52分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include <string>
#include <vector>
#include "MyString.h"

class CStrSpliter
{
public:
    CStrSpliter();
    ~CStrSpliter();

    bool Split(const CMyString &str, const char delimiter=';');
    int32_t size(void);
    bool empty(void);
    CMyString& operator[] (int i);
    CMyString& at(int i);

    int32_t asInt32(int32_t i);
    uint32_t asUInt32(int32_t i);
    int64_t asInt64(int32_t i);
    double asDouble(int32_t i);
    const char* asString(int32_t i);

    void dump(void);
    void clear(void);

protected:
    int32_t decode(const CMyString &str, const char delimiter);

private:
    std::vector<CMyString>  m_vecItem;
};


