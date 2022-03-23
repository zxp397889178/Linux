/*
 * =====================================================================================
 *
 *       Filename:  MoneyRange.cpp
 *
 *    Description:  对金额进行检查的类
 *
 *        Version:  1.0
 *        Created:  02/07/2020 01:27:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MoneyRange.h"

CMoneyRange::CMoneyRange()
{

}

CMoneyRange::~CMoneyRange()
{

}

bool CMoneyRange::Init(const CMyString &str)
{
    m_vecFixCoin.clear();
    m_vecRangeCoin.clear();

    LogDebug("INITPAY>money string [%s]", str.c_str());
#if 0
    int32_t ret(0);
    std::vector< std::string > arMoneyRange;
    try
    {
        ret = safe_strsplit(str.c_str(), arMoneyRange, ",");
        if (ret <=0)
        {
            LogFatal("[%s:%s:%d]invalid money_string [%s]", __FILE__, __func__, __LINE__, str.c_str());
            return false;
        }

        for(std::vector< std::string >::iterator it(arMoneyRange.begin());
                it != arMoneyRange.end(); ++it)
        {
            std::string &item = *it;
            CMyString strNote("INITPAY>money value [%s]", item.c_str());
            size_t pos = item.find("-");
            if (pos == std::string::npos)
            {
                int64_t v = StrToInt64(item.c_str());
                strNote.Append(",fixed value [%ld]", v);
                m_vecFixCoin.push_back(v);
            }
            else
            {
                CMyString str1 = item.substr(0, pos);
                CMyString str2 = item.substr(pos+1);
                str1.trim();
                str2.trim();
                int64_t v1 = StrToInt64(str1.c_str());
                int64_t v2 = StrToInt64(str2.c_str());
                strNote.Append(", money range [%ld - %ld]", v1, v2);
                if (v1 == v2)
                {
                    m_vecFixCoin.push_back(v1);
                }
                else
                {
                    struct RangeItem iv;
                    iv.minCoin = (v1<v2 ? v1 : v2);
                    iv.maxCoin = (v1>v2 ? v1 : v2);
                    m_vecRangeCoin.push_back(iv);
                }
            }
            LogDebug(strNote.c_str());
        }
    }
    catch(CException &e)
    {
        LogFatal("[%s:%s:%d]decode money_string [%s] failure, %s", __FILE__, __func__, __LINE__, str.c_str(), e.what());
        return false;
    }
    catch(...)
    {
        LogFatal("[%s:%s:%d]unknown error when decode money_string [%s]", __FILE__, __func__, __LINE__, str.c_str());
        return false;
    }
#else
    CStrSpliter css;
    if (!css.Split(str, ','))
    {
        LogErr("[%s:%s:%d]decode money_string [%s] failure", __FILE__, __func__, __LINE__, str.c_str());
        return false;
    }
    CMyString strNote;
    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString strItem = css[i];
        strNote.Format("INITPAY>money string [%s]", strItem.c_str());

        CStrSpliter sub_css;
        if (sub_css.Split(strItem, '-'))
        {
            if (sub_css.size() > 1)
            {
                int64_t v1 = StrToInt64(sub_css[0].c_str());
                int64_t v2 = StrToInt64(sub_css[1].c_str());
                if (v1 == v2)
                {
                    m_vecFixCoin.push_back(v1);
                    strNote.Append(", fixed coin [%ld]", v1);
                }
                else
                {
                    struct RangeItem iv;
                    iv.minCoin = (v1<v2 ? v1 : v2);
                    iv.maxCoin = (v1>v2 ? v1 : v2);
                    m_vecRangeCoin.push_back(iv);
                    strNote.Append(", range coin [%ld] - [%ld]", iv.minCoin, iv.maxCoin);
                }
            }
            else
            {
                int64_t v = StrToInt64(sub_css[0].c_str());
                m_vecFixCoin.push_back(v);
                strNote.Append(", fixed coin [%ld]", v);
            }
        }
        else
        {
            int64_t v = StrToInt64(strItem.c_str());
            m_vecFixCoin.push_back(v);
            strNote.Append(", fixed coin [%ld]", v);
        }
        LogDebug(strNote.c_str());
    }
#endif

    if (m_vecFixCoin.empty() && m_vecRangeCoin.empty())
    {
        LogFatal("[%s:%s:%d]invalid money_string [%s]", __FILE__, __func__, __LINE__, str.c_str());
        return false;
    }

    return true;
}


bool CMoneyRange::Valid(int64_t coin)
{
    // 先检查固定值
    LogDebug("DBGPAY>user pass money:%ld", coin);
    for(std::vector<int64_t>::iterator it(m_vecFixCoin.begin());
            it != m_vecFixCoin.end(); ++it)
    {
        int64_t &v1 = *it;
        LogDebug("DBGPAY>fixed money:%ld", v1);
        if (v1 == coin)
        {
            LogDebug("DBGPAY>match fixed money.");
            return true;
        }
    }

    for(std::vector<RangeItem>::iterator iter(m_vecRangeCoin.begin());
            iter != m_vecRangeCoin.end(); ++iter)
    {
        RangeItem &r1 = *iter;
        LogDebug("DBGPAY>money range[%ld - %ld]", r1.minCoin, r1.maxCoin);
        if (coin >= r1.minCoin && coin<=r1.maxCoin)
        {
            LogDebug("DBGPAY>match range money.");
            return true;
        }
    }

    return false;
}


