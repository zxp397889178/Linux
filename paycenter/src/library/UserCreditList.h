/*
 * =====================================================================================
 *
 *       Filename:  UserCreditList.h
 *
 *    Description:  用户充值提现额度及可提现额度
 *
 *        Version:  1.0
 *        Created:  2021年08月02日 15时55分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"
#include "MyNet.h"
#include <map>

class CUserCreditList
{
    struct UserCreditData
    {
        CMyString       strUin;             //用户id
        CMyString       strGameCode;        //
        int64_t         nTotalDeposit;      //总充值额
        int64_t         nTotalWithdraw;     //总提现额
        int64_t         nLimitWithdraw;     //提现限额
        int64_t         nRemainWithdraw;    //还可以提现的额度
    };

    typedef std::map<CMyString, UserCreditData>        MAP_USERCREDITLIST;
public:
    ~CUserCreditList();
    static CUserCreditList& InstanceGet();
    int32_t Load(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, UserCreditData& userData);

    bool AddDeposit(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, const int64_t nDeposit);
    bool AddWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin, const int64_t nWithdraw);
    int64_t GetTotalDeposit(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin);
    int64_t GetTotalWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin);
    int64_t GetLimitWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin);
    int64_t GetRemainWithdraw(ISimDB *pDB, const CMyString& gameCode, const CMyString& strUin);

private: 
    CUserCreditList();

private:
    MAP_USERCREDITLIST  m_mapUserCreditList;
};

#define USERCREDIT_LIST     CUserCreditList::InstanceGet()
