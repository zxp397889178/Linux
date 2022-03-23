/*
 * =====================================================================================
 *
 *       Filename:  Withdraw.h
 *
 *    Description:  提现接口定义
 *
 *        Version:  1.0
 *        Created:  2021年01月09日 12时03分09秒
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
#include "MyNet.h"
#include "MyDB.h"
#include <vector>
#include <queue>
#include "Common.h"
#include "GameList.h"
#include "SdkList.h"
#include "SdkChannelList.h"
#include "ChannelList.h"
#include "BillPointList.h"
#include "IClient.h"
#include "IWebClientMgr.h"
#include "IWebPlugin.h"
#include "MyUtility.h"

class CPluginWithdraw
{
public:
    CPluginWithdraw();
    virtual ~CPluginWithdraw();

    int32_t Init(void);
    int32_t Update(void);
    void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doWithdrawRequest);
    ENTRY_FUNC_DEFINE(doWithdrawAudit);
    ENTRY_FUNC_DEFINE(doWithdrawCredit);

private:
    int32_t valid_req(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, std::vector<CMyString> &vecParam, bool bSign, const CMyString &signField, bool bSignFullParam, const CMyString &gameCodeField, CGame *&pGame);
    CGame* getGame(const CMyString &gameCode);

    void build_error_msg(int32_t code, const CMyString &errMsg, const CMyString &gameCode, char *szResp, uint32_t &cbResp);

    int32_t postAlert(CGame *pGame, const CMyString &strText);

    int32_t notifyGamesvr(const CMyString &gameCode, const CMyString &strUin, const CMyString &orderNo, const CMyString &gameOrderNo, int64_t fee, int32_t step, int32_t result, CGame *pGame, CMyString& strError);
    void postNextCredit(const CMyString &gameCode, const CMyString &sdkCode, const CMyString &orderNo, const CMyString &remark, const CMyString &creditOp, int64_t rcreditTime, char *szResp, uint32_t &cbResp);

    bool ValidIp(const CMyString &strIp, const CMyString &sect);
    void getSignInfo(const CMyString &sect, bool &bEnable, CMyString &field);
 
    void setWithdrawLog(const CMyString& orderNo, const int& result, CMyString oper, CMyString remark, CMyString message, int64_t createtime = 0);
    int32_t checkCanWithdraw(const CMyString& gameCode, const CMyString& uin, const int64_t& fee, const int64_t& tax, char *szResp, uint32_t &cbResp);

protected:
    bool InitRedisCache(redis_cache::CFmtCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect);
    bool InitRedisCache(redis_cache::CIncrCache &cache, CMyRedis &redis, CXmlFile &xml, const CMyString &sect);

private:
    http::CHttpClient   m_http;
    ISimDB*             m_pDB;
    CMyRedis            m_redis;

private:
    redis_cache::CFmtCache  m_cacheCreditReq;
    // CRedisCache         m_cacheCreditReq;
    int32_t             m_creditTimeout;
};


