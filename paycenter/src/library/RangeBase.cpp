/*
 * =====================================================================================
 *
 *       Filename:  RangeBase.cpp
 *
 *    Description:  数值范围的分组取值，以int64_t为限
 *
 *        Version:  1.0
 *        Created:  2021年04月22日 22时32分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "RangeBase.h"

CRangeBase::CRangeBase()
{

}

CRangeBase::~CRangeBase()
{

}


int32_t CRangeBase::Load(const CMyString &str, const char cGroupSplit, const char cSplit)
{
    m_setFixed.clear();
    m_vecRange.clear();

    if (str.empty())
    {
        return 0;
    }

    CStrSpliter css;
    if (!css.Split(str, cGroupSplit))
    {
        return -1;
    }

    int32_t cnt(0);
    for(int32_t grp(0); grp<css.size(); ++grp)
    {
        CMyString strItem = css[grp];
        if (strItem.empty())
        {
            continue;
        }

        ++cnt;
        CStrSpliter css_sub;
        if (!css_sub.Split(strItem, cSplit))
        {
            int64_t v1 = StrToInt64(strItem.c_str());
            m_setFixed.insert(v1);
            continue;
        }
        if (css_sub.size()==1)
        {
            int64_t v1 = StrToInt64(css_sub[0].c_str());
            m_setFixed.insert(v1);
            continue;
        }
        int64_t v1 = StrToInt64(css_sub[0].c_str());
        int64_t v2 = StrToInt64(css_sub[1].c_str());
        if (v1==v2)
        {
            m_setFixed.insert(v1);
            continue;
        }
        struct RangeItem ri;
        ri.startVal = (v1<v2) ? v1 : v2;
        ri.endVal = (v1>v2) ? v1 : v2;
        m_vecRange.push_back(ri);
    }

    return cnt;
}


bool CRangeBase::InRange(int64_t val)
{
    // 先检查固定值
    if (m_setFixed.find(val) != m_setFixed.end())
    {
        return true;
    }

    // 再检查范围值
    bool bFind(false);
    for(std::vector<RangeItem>::iterator it(m_vecRange.begin());
            it != m_vecRange.end(); ++it)
    {
        RangeItem &ri = *it;
        if(val>=ri.startVal && val<=ri.endVal)
        {
            bFind = true;
            break;
        }
    }

    return bFind;
}

int64_t CRangeBase::getStartVal(void)
{
    int64_t val = INT64_MAX;
    if (!m_setFixed.empty())
    {
        std::set<int64_t>::iterator it = m_setFixed.begin();
        int64_t v1 = *it;
        if (val > v1)
        {
            val = v1;
        }
    }

    for(std::vector<RangeItem>::iterator it(m_vecRange.begin());
            it != m_vecRange.end(); ++it)
    {
        RangeItem &ri = *it;
        if (val > ri.startVal)
        {
            val = ri.startVal;
        }
    }

    if (val == INT64_MAX)
    {
        val = 0;
    }

    return val;
}

int64_t CRangeBase::getEndVal(void)
{
    int64_t val = 0;
    if (!m_setFixed.empty())
    {
        std::set<int64_t>::reverse_iterator it = m_setFixed.rbegin();
        int64_t v1 = *(it.base());
        if (val < v1)
        {
            val = v1;
        }
    }

    for(std::vector<RangeItem>::iterator it(m_vecRange.begin());
            it != m_vecRange.end(); ++it)
    {
        RangeItem &ri = *it;
        if (val < ri.endVal)
        {
            val = ri.endVal;
        }
    }

    return val;
}

CMyString CRangeBase::toString(const char cGroupSplit, const char cSplit)
{
    CMyString result;
    for(std::set<int64_t>::iterator it(m_setFixed.begin()); 
            it != m_setFixed.end(); ++it)
    {
        int64_t v1 = *it;
        if (result.empty())
        {
            result.Append("%ld", v1);
        }
        else
        {
            result.Append("%c%ld", cGroupSplit, v1);
        }
    }

    for(std::vector<RangeItem>::iterator it(m_vecRange.begin());
            it != m_vecRange.end(); ++it)
    {
        RangeItem &ri = *it;
        if (result.empty())
        {
            result.Append("%ld%c%ld", ri.startVal, cSplit, ri.endVal);
        }
        else
        {
            result.Append("%c%ld%c%ld", cGroupSplit, ri.startVal, cSplit, ri.endVal);
        }
    }
    return result;
}

bool CRangeBase::empty(void)
{
    return m_setFixed.empty() && m_vecRange.empty();
}

bool CRangeBase::Valid(int64_t val)
{
    if (this->empty())
    {
        return true;
    }

    if (this->InRange(val))
    {
        return true;
    }

    return false;
}

