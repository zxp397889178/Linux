/*
 * =====================================================================================
 *
 *       Filename:  UserCreditList.cpp
 *
 *    Description:  用户充值提现额度及可提现额度
 *
 *        Version:  1.0
 *        Created:  2021年08月02日 15时55分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "UserCreditList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "WithdrawLimitData.h"


CUserCreditList::CUserCreditList()
{
 
}

CUserCreditList::~CUserCreditList()
{

}

CUserCreditList& CUserCreditList::InstanceGet()
{
    static CUserCreditList s_obj;
    return s_obj;
}

int32_t CUserCreditList::Load(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, UserCreditData& userData)
{
    int64_t nTotalDeposit(0);
    int64_t nTotalWithdraw(0);
    int64_t nLimitWithdraw(-1);

    //获取用户充值、提现总额
    pDB->Cmd("call pQueryUserSummary('%s', '%s')", gameCode.c_str(), strUin.c_str());
    while(pDB->More()) 
    {
        nTotalDeposit = pDB->GetInt64(0);
        nTotalWithdraw = pDB->GetInt64(1);
    }

    char szError[256] = "";
    int32_t err = pDB->GetLastError(szError, sizeof(szError));
    if (err != 0)
    {
        LogErr("[%s:%s:%d] CUserCredit load %s data failure, error %s", __FILE__, __func__, __LINE__, strUin.c_str(), szError);
        return -1;
    }

    //获取充值提现配置
    nLimitWithdraw = WITHDRAWLIMIT_DATA.FindLimitByDeposit(gameCode, nTotalDeposit);

    userData.strUin = strUin;
    userData.strGameCode = gameCode;
    userData.nTotalDeposit = nTotalDeposit;
    userData.nTotalWithdraw = nTotalWithdraw;
    userData.nLimitWithdraw = nLimitWithdraw;
    if (nLimitWithdraw > 0)
    {
        if (nLimitWithdraw > nTotalWithdraw)
            userData.nRemainWithdraw = nLimitWithdraw - nTotalWithdraw;
        else
            userData.nRemainWithdraw = 0;
    }
    else
    {
        userData.nRemainWithdraw = -1;
    }

    return 0;
}

bool CUserCreditList::AddDeposit(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, const int64_t nDeposit)
 {
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        data.nTotalDeposit += nDeposit;

        //获取充值提现配置
        data.nLimitWithdraw = WITHDRAWLIMIT_DATA.FindLimitByDeposit(gameCode, data.nTotalDeposit);
        if (data.nLimitWithdraw > 0)
        {
            if (data.nLimitWithdraw > data.nTotalWithdraw)
                data.nRemainWithdraw = data.nLimitWithdraw - data.nTotalWithdraw;
            else
                data.nRemainWithdraw = 0;
        }
        else
        {
            data.nRemainWithdraw = -1;
        }

        return true;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return false;
    }

    m_mapUserCreditList[strKey] = userData;
    return true;
 }

bool CUserCreditList::AddWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, const int64_t nWithdraw)
{
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        data.nTotalWithdraw += nWithdraw;

        //获取充值提现配置
        if (data.nLimitWithdraw > 0)
        {
            if (data.nLimitWithdraw > data.nTotalWithdraw)
                data.nRemainWithdraw = data.nLimitWithdraw - data.nTotalWithdraw;
            else
                data.nRemainWithdraw = 0;
        }
        else
        {
            data.nRemainWithdraw = -1;
        }
        return true;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return false;
    }

    m_mapUserCreditList[strKey] = userData;
    return true;
}

int64_t CUserCreditList::GetTotalDeposit(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin)
{
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        return data.nTotalDeposit;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return 0;
    }

    m_mapUserCreditList[strKey] = userData;
    return userData.nTotalDeposit;
}

int64_t CUserCreditList::GetTotalWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin)
{
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        return data.nTotalWithdraw;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return 0;
    }

    m_mapUserCreditList[strKey] = userData;
    return userData.nTotalWithdraw;
}

int64_t CUserCreditList::GetLimitWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin)
{
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        return data.nLimitWithdraw;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return 0;
    }

    m_mapUserCreditList[strKey] = userData;
    return userData.nLimitWithdraw;
}

int64_t CUserCreditList::GetRemainWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin)
{
    CMyString strKey = gameCode + strUin;
    MAP_USERCREDITLIST::iterator it = m_mapUserCreditList.find(strKey);
    if (it != m_mapUserCreditList.end())
    {
        UserCreditData& data = it->second;
        return data.nRemainWithdraw;
    }
    
    UserCreditData userData;
    int nRet = Load(pDB, gameCode, strUin, userData);
    if (nRet != 0)
    {
        return 0;
    }

    m_mapUserCreditList[strKey] = userData;
    return userData.nRemainWithdraw;
}
