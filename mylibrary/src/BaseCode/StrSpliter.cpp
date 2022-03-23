/*
 * =====================================================================================
 *
 *       Filename:  StrSpliter.cpp
 *
 *    Description:  对字符串做分割处理
 *
 *        Version:  1.0
 *        Created:  2020年02月11日 20时52分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "StrSpliter.h"
#include "StringFunc.h"
#include <stdio.h>


CStrSpliter::CStrSpliter() 
{

}

CStrSpliter::~CStrSpliter()
{

}

bool CStrSpliter::Split(const CMyString &str, const char delimiter)
{
    CMyString str2(str.c_str());
    str2.trim();
    m_vecItem.clear();
    int32_t ret = this->decode(str2, delimiter);
    return (ret>=0);
}

int32_t CStrSpliter::size(void) 
{
    return static_cast<int32_t>(m_vecItem.size());
}

bool CStrSpliter::empty(void)
{
    return m_vecItem.empty();
}

void CStrSpliter::clear(void)
{
    m_vecItem.clear();
}

CMyString& CStrSpliter::operator[] (int i)
{
    return m_vecItem[i];
}

CMyString& CStrSpliter::at(int i)
{
    return m_vecItem.at(i);
}

int32_t CStrSpliter::asInt32(int32_t i)
{
    CMyString &str = m_vecItem[i];
    if (str.empty())
    {
        return 0;
    }
    return StrToInt32(str.c_str());
}

uint32_t CStrSpliter::asUInt32(int32_t i)
{
    CMyString &str = m_vecItem[i];
    if (str.empty())
    {
        return 0;
    }
    return StrToUint32(str.c_str());
}

int64_t CStrSpliter::asInt64(int32_t i)
{
    CMyString &str = m_vecItem[i];
    if (str.empty())
    {
        return 0;
    }
    return StrToInt64(str.c_str());
}

double CStrSpliter::asDouble(int32_t i)
{
    CMyString &str = m_vecItem[i];
    if (str.empty())
    {
        return 0;
    }
    return StrToDouble(str.c_str());
}

const char* CStrSpliter::asString(int32_t i)
{
    CMyString &str = m_vecItem[i];
    return str.c_str();
}

void CStrSpliter::dump(void)
{
    std::vector<CMyString>::iterator it(m_vecItem.begin());
    for(; it != m_vecItem.end(); ++it)
    {
        CMyString &item = *it;
        printf("[%s]\n", item.c_str());
    }
}

int32_t CStrSpliter::decode(const CMyString &str, const char delimiter)
{
    int32_t cnt(0);
    CMyString temp;
    size_t pos1(0);
    size_t pos2(0);
    bool bFinished(false);

    do
    {
        pos2 = str.find_first_of(delimiter, pos1);
        if (pos2 == std::string::npos)
        {
            temp = str.substr(pos1);
            m_vecItem.push_back(temp);
            bFinished =true;
        }
        else
        {
            temp = str.substr(pos1, pos2-pos1);
            if (pos2+1==str.length())
            {
                m_vecItem.push_back(temp);
                temp.clear();
                m_vecItem.push_back(temp);
                ++cnt;
                bFinished = true;
            }
            else
            {
                m_vecItem.push_back(temp);
                pos1 = pos2+1;
                // 对以 ' ' 进行分隔的特殊处理
                if (delimiter == ' ')
                {
                    while(true)
                    {
                        if (str[pos1]==delimiter && pos1<str.length())
                        {
                            ++pos1;
                            continue;
                        }
                        break;
                    }
                }
            }
        }
        ++cnt;
    }while(!bFinished);
    return cnt;
}
