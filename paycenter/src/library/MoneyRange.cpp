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
    : CRangeBase()
{
}

CMoneyRange::~CMoneyRange()
{
}

bool CMoneyRange::Init(const CMyString &str)
{
    int32_t ret = CRangeBase::Load(str, ',', '-');
    if (ret <0)
    {
        LogErr("invalid money range string [%s]", str.c_str());
        return false;
    }
    if (ret==0)
    {
        LogWarn("money range string empty.");
        return true;
    }

    CMyString val = CRangeBase::toString(',', '-');
    LogDebug("origin text[%s], split result[%s]", str.c_str(), val.c_str());
    return true;
}


bool CMoneyRange::Valid(int64_t coin)
{
    return CRangeBase::InRange(coin);
}

int64_t CMoneyRange::getMoneyMin(void)
{
    return CRangeBase::getStartVal();
}

int64_t CMoneyRange::getMoneyMax(void)
{
    return CRangeBase::getEndVal();
}


