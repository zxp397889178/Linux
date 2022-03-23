/*
* =====================================================================================
*
*       Filename:  UserItemMgr.cpp
*
*    Description:  用户信息管理
*
*        Version:  1.0
*        Created:  2018年2月2日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#include "UserInfoMgr.h"
#include "Protocol/protoSystem.h"

#define LOG_MODOULE "USERINFOMGR"

UserInfoMgr::UserInfoMgr()
{
}

UserInfoMgr::~UserInfoMgr()
{
}


int32_t UserInfoMgr::QueryUserInfo(uint32_t uin, uint32_t roomId, struct protoUserInfoResponse &resp)
{
	struct protoUserInfoRequest req;
	req.uin = uin;
	req.roomId = roomId;

	return this->QueryResult(req, resp);
}

int32_t UserInfoMgr::UserLeaveRoom(struct protoSvrUserLeaveRoomRequest &req)
{
	struct protoSvrUserLeaveRoomResponse resp;
	int32_t result = this->QueryResult(req, resp);
	if (result != 0)
	{
		return result;
	}
	return resp.result;
}

int32_t UserInfoMgr::UserAddCurrency(struct protoSvrAddCurrencyRequest &req)
{
	struct protoSvrAddCurrencyResponse resp;
	int32_t result = this->QueryResult(req, resp);
	if (result != 0)
	{
		return result;
	}
    UpdateCurrencyOpLog(req.opIndex, 4, resp.result);
	return resp.result;
}

int32_t UserInfoMgr::UserAddCurrency(uint32_t uin, uint32_t logType, uint32_t coin, uint32_t ticket, uint32_t money, vector<CurrencyInfo> &vecItem)
{
	struct protoSvrAddCurrencyRequest req;
	req.uin = uin;
	req.type = logType;

	if (coin > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Coin;
		info.num = coin;
		req.vecCurrencyInfo.push_back(info);
	}
	
	if (ticket > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Ticket;
		info.num = ticket;
		req.vecCurrencyInfo.push_back(info);
	}

	if (money > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Money;
		info.num = money;
		req.vecCurrencyInfo.push_back(info);
	}

    CMyString strItem;

	for (uint32_t i = 0; i < vecItem.size(); ++i)
	{
		CurrencyInfo &info = vecItem[i];
		info.type = CurrencyInfo::Item;
		req.vecCurrencyInfo.push_back(info);

        CMyString strItemTypeId("%u", vecItem[i].itemTypeId);
        CMyString strNum("%u", vecItem[i].num);
        strItem += strItemTypeId + "," + strNum + "|";
	}

    int64_t opIndex = 0;

    CMyRedis *pRedis = DB_MGR.GetRedis("main");
    if (pRedis)
    {        
        if (pRedis->IncBy(CURRENCY_OP_KEY, 1, opIndex) >= 1)
        {
            LogDebug2(LOG_MODOULE, uin, "user add currency, opIndex=[%ld] coin=[%u] ticket=[%u]", opIndex, coin, ticket);
            req.opIndex = opIndex;            
        }
        else
        {
            LogErr2(LOG_MODOULE, uin, "user add currency, inc opIndex failed");
        }        
    }
    else
    {
        LogErr2(LOG_MODOULE, uin, "user add currency, get redis connection failed");
    }
    
    ISimDB *pDbLog = DB_MGR.GetDbConnector("log");

    if (pDbLog)
    {
        pDbLog->Cmd("INSERT INTO tlogcurrencyoperation (uin, op_index, op_type, coin, money, ticket, item, step) VALUES (%u, %ld, %d ,%u, %u , %u, '%s', %u)", uin, opIndex, 1, coin, money, ticket, strItem.c_str(), 1);
        if (pDbLog->Exec() < 0)
        {
            LogErr2(LOG_MODOULE, uin, "user add currency, exec sql faild");
        }
    }
    else
    {
        LogErr2(LOG_MODOULE, uin, "user add currency, get db connection failed");
    }
	return this->UserAddCurrency(req);
}

int32_t UserInfoMgr::UserDecCurrency(struct protoSvrDecCurrencyRequest &req)
{
	struct protoSvrDecCurrencyResponse resp;
	int32_t result = this->QueryResult(req, resp);
	if (result != 0)
	{
		return result;
	}
    UpdateCurrencyOpLog(req.opIndex, 4, resp.result);
	return resp.result;
}


int32_t UserInfoMgr::UserDecCurrency(uint32_t uin, uint32_t coin, uint32_t ticket, uint32_t money, vector<CurrencyInfo> &vecItem, uint32_t decType)
{
	struct protoSvrDecCurrencyRequest req;
	req.uin = uin;
	req.type = decType;

	if (coin > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Coin;
		info.num = coin;
		req.vecCurrencyInfo.push_back(info);
	}

	if (ticket > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Ticket;
		info.num = ticket;
		req.vecCurrencyInfo.push_back(info);
	}

	if (money > 0)
	{
		CurrencyInfo info;
		info.type = CurrencyInfo::Money;
		info.num = money;
		req.vecCurrencyInfo.push_back(info);
	}

    CMyString strItem;
	for (uint32_t i = 0; i < vecItem.size(); ++i)
	{
		CurrencyInfo &info = vecItem[i];
		info.type = CurrencyInfo::Item;
		req.vecCurrencyInfo.push_back(info);

        CMyString strItemTypeId("%u", vecItem[i].itemTypeId);
        CMyString strNum("%u", vecItem[i].num);
        strItem += strItemTypeId + "," + strNum + "|";
	}

    int64_t opIndex = 0;
    CMyRedis *pRedis = DB_MGR.GetRedis("main");
    if (pRedis)
    {        
        if (pRedis->IncBy(CURRENCY_OP_KEY, 1, opIndex) >= 1)
        {
            LogDebug2(LOG_MODOULE, uin, "user dec currency, opIndex=[%ld] coin=[%u] ticket=[%u]", opIndex, coin, ticket);
            req.opIndex = opIndex; 
        }
        else
        {
            LogErr2(LOG_MODOULE, uin, "user dec currency, inc opIndex failed");
        }
    }
    else
    {
        LogErr2(LOG_MODOULE, uin, "user dec currency, get redis connection failed");
    }

    ISimDB *pDbLog = DB_MGR.GetDbConnector("log");

    if (pDbLog)
    {
        pDbLog->Cmd("INSERT INTO tlogcurrencyoperation (uin, op_index, op_type, coin, money, ticket, item, step) VALUES (%u, %ld, %d ,%u, %u , %u, '%s', %u)", uin, opIndex, -1, coin, money, ticket, strItem.c_str(), 1);
        if (pDbLog->Exec() < 0)
        {
            LogErr2(LOG_MODOULE, uin, "user dec currency, exec sql faild");
        }
    }
    else
    {
        LogErr2(LOG_MODOULE, uin, "user dec currency, get db connection failed");
    }

	return this->UserDecCurrency(req);
}

int32_t UserInfoMgr::UpdateCurrencyOpLog(int64_t index, int32_t step, uint32_t result)
{
    ISimDB *pDbLog = DB_MGR.GetDbConnector("log");

    if (pDbLog)
    {
        pDbLog->Cmd("UPDATE tlogcurrencyoperation SET step=%u, result=%u WHERE op_index=%ld", step, result, index);
        if (pDbLog->Exec() < 0)
        {
            LogErr2(LOG_MODOULE, 0, "update currency operation log, exec faile failed, opIndex=[%ld] step=[%d] result=[%u]", index, step, result);
        }
    }
    else
    {
        LogErr2(LOG_MODOULE, 0, "update currency operation log, get db connection failed, opIndex=[%ld] step=[%d] result=[%u]", index, step, result);
    }

    return 0;
}

bool UserInfoMgr::Authorize(IClientSocket* pConn)
{
	protoSysAuthorize auth(actorHallSvr, QW_FRAME->GetFrameConfig().GetSvrId() * 10, 0);
	//NOTE: token NOT used now

	char buffer[1024] = "";
	char *pData = (char*)&buffer[sizeof(struct Record)];
	size_t cbData = auth.getProtoLen();
	int32_t ret = auth.Serialize(pData, cbData);
	if (ret < 0)
	{
		LogFatal("UserInfoMgr::Authorize serialize protoAuthorize failure.");
		return false;
	}

	struct Record *pRecord = (struct Record*)&buffer[0];
	size_t cbBuf = sizeof(struct Record) + cbData;
	memcpy(pRecord->synccode, g_cSyncCode, sizeof(pRecord->synccode));
	pRecord->recordLen = cbBuf;
	pRecord->cmdId = protoSysAuthorize::IDD;
	pRecord->uin = 0;
	pRecord->sessionId = 0;
	pRecord->clientIp = 0;
	pRecord->flag = Record::FRM_ALL;
	pRecord->mask = Record::INNER_PROTOCOL;
	pRecord->Cast();

	pConn->SendMsg((void*)&buffer[0], cbBuf);
	LogDebug("UserInfoMgr::Authorize begin to authorize connector");

	return true;
}

