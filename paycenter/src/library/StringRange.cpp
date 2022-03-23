/*
 * =====================================================================================
 *
 *       Filename:  StringRange.cpp
 *
 *    Description:  字符串范围比较，支持正常表达式
 *
 *        Version:  1.0
 *        Created:  2021年04月22日 23时08分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "StringRange.h"
#include <pcrecpp.h>

CStringRange::CStringRange()
{

}

CStringRange::~CStringRange()
{

}

int32_t CStringRange::Load(const CMyString &str, const char cSplit)
{
    m_StringList.clear();

    if (str.empty())
    {
        return 0;
    }

    CStrSpliter css;
    if (!css.Split(str, cSplit))
    {
        return -1;
    }

    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString s1 = css[i];
        m_StringList.insert(s1);
    }
    return 1;
}

bool CStringRange::Match(const CMyString &str)
{
    bool result(false);
    for(std::set<CMyString>::iterator it(m_StringList.begin());
            it != m_StringList.end(); ++it)
    {
        CMyString fmt = *it;
        pcrecpp::RE oPattern(fmt);
        if (oPattern.FullMatch(str))
        {
            result = true;
            break;
        }
    }
    return result;
}

CMyString CStringRange::toString(const char cSplit)
{
    CMyString result;

    for(std::set<CMyString>::iterator it(m_StringList.begin());
            it != m_StringList.end(); ++it)
    {
        if (result.empty())
        {
            result = *it;
        }
        else
        {
            result.Append("%c", cSplit);
            result += *it;
        }
    }

    return result;
}

bool CStringRange::empty(void)
{
    return m_StringList.empty();
}

bool CStringRange::Valid(const CMyString &val)
{
    if (this->empty())
    {
        return true;
    }

    if (this->Match(val))
    {
        return true;
    }

    return false;
}

