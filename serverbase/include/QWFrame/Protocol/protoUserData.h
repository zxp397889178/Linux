/*
* =====================================================================================
*
*       Filename:  protoUserData.h
*
*    Description:  用户数据协议
*
*        Version:  1.0
*        Created:  2018年1月29日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_USERDATA_H_
#define _PROTO_USERDATA_H_

#include <string.h>
#include <vector>
#include <stdint.h>
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoSysDef.h"
#include "protoBase.h"
#include "Serialize.hpp"


struct UserItemInfo
{
	uint32_t	itemId;
	uint32_t	itemTypeId;
	uint32_t	num;
	uint32_t	isWeared;			// 是否已装备

	UserItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 用户装备道具信息
struct UserWearItemInfo
{
	uint32_t	itemTypeId;
	uint32_t	status;				// 0-未装备  1-已装备
	uint64_t	wearTime;			// 装备时间
	uint64_t	expireTime;			// 失效时间

	UserWearItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 用户上下分信息
struct UserFishInfo
{
	uint32_t roomId;
	int64_t totalPutScore;
	int64_t totalGetScore;
	int64_t lostScore;

	UserFishInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 用户连续任务信息
struct UserContiTaskInfo
{
	uint32_t roomId;
	uint32_t taskId;
	uint32_t taskNum;
	char completeValue[64];
	uint32_t status;
	int64_t gainCoin;
	int64_t costCoin;

	UserContiTaskInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

/*用户信息查询*/
struct protoUserInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USERINFO_REQUEST };

	uint32_t	uin;
	uint32_t	roomId;

	protoUserInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoUserInfoResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USERINFO_RESPONSE };

	uint32_t uin;
	uint32_t roomId;

	char nick[64];
	uint32_t vip;
	uint32_t level;
	uint64_t exp;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	uint32_t trumpet;
	uint32_t bankCoin;

	std::vector<UserItemInfo> vecUserItemInfo;
	int64_t totalPutScore;
	int64_t totalGetScore;
	int64_t lostScore;

	std::vector<UserWearItemInfo> vecUserWearItemInfo;

	uint32_t language;
	bool isSimulator;

	protoUserInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


/*用户离开房间信息同步请求*/
struct protoSvrUserLeaveRoomRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USERLEAVEROOM_REQUEST };

	uint32_t uin;
	uint32_t roomId;
	uint32_t level;
	uint64_t exp;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;

	std::vector<UserItemInfo> vecUserItemInfo;
	int64_t totalPutScore;
	int64_t totalGetScore;
	int64_t lostScore;
	std::vector<UserWearItemInfo> vecUserWearItemInfo;

	protoSvrUserLeaveRoomRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrUserLeaveRoomResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USERLEAVEROOM_RESPONSE };

	int32_t result;

	protoSvrUserLeaveRoomResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*货币信息*/
struct CurrencyInfo
{
	enum eType
	{
		Money = 1,				//趣玩币
		Coin = 2,				//金币
		Ticket = 3,				//奖券
		Trumpet = 4,			//喇叭
		Item = 5,				//道具
		BankCoin = 6,			//银行金币
	};

	uint32_t type;			// 1-趣玩币 2-金币 3-奖券 4-喇叭 5-道具
	uint32_t itemTypeId;	// 道具ID
	uint32_t num;

	CurrencyInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

/*增加货币请求*/
struct protoSvrAddCurrencyRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_CURRENCY_REQUEST };

	uint32_t uin;
	uint32_t type;	
	std::vector<CurrencyInfo> vecCurrencyInfo;
	char szInfo[64];
	uint64_t timestamp;
    int64_t opIndex;

	protoSvrAddCurrencyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrAddCurrencyResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_CURRENCY_RESPONSE };

	int32_t result;
    int64_t opIndex;

	protoSvrAddCurrencyResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*扣除货币请求*/
struct protoSvrDecCurrencyRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEC_CURRENCY_REQUEST };

	uint32_t uin;	
	std::vector<CurrencyInfo> vecCurrencyInfo;
	int32_t type;
	char szInfo[64];
	uint64_t timestamp;
    int64_t opIndex;

	protoSvrDecCurrencyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDecCurrencyResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEC_CURRENCY_RESPONSE };

	int32_t result;
    int64_t opIndex;

	protoSvrDecCurrencyResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoDCSvrLoadUserDataResponse : MSG_HEAD
{
	enum { IDD = PROTO_DCSVR_LOAD_USERDATA_RESPONSE };

	int32_t		result;
	uint32_t	gamesvrId;
	uint32_t	uin;
	int32_t		new_user;

	char		szDevice[64];
	char		szChannel[64];
	uint32_t	source;
	uint32_t	version;
	uint32_t	sessionId;
	uint32_t	clientIp;
	uint32_t	proxyId;
	uint32_t	channelId;

	// 用户数据信息
	char account[64];
	char nick[64];
	uint32_t vip;
	uint32_t level;
	uint64_t exp;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	uint32_t trumpet;
	uint32_t bankCoin;
	uint32_t verified;
	uint64_t createTime;
	uint64_t totalRecharge;
	char registerChannel[64];
	char reserve1[128];
	char reserve2[128];

	std::vector<UserItemInfo> vecUserItemInfo;
	std::vector<UserWearItemInfo> vecUserWearItemInfo;
	std::vector<UserFishInfo> vecUserFishInfo;
	std::vector<UserContiTaskInfo> vecUserContiTaskInfo;

	protoDCSvrLoadUserDataResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户数据同步请求
struct protoDCSvrUserDataSyncRequest : MSG_HEAD
{
	enum { IDD = PROTO_DCSVR_USERDATA_SYNC_REQUEST };

	// 用户数据信息
	uint32_t index;
	uint32_t uin;
	char nick[64];
	uint32_t vip;
	uint32_t level;
	uint64_t exp;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	uint32_t trumpet;
	uint32_t bankCoin;
	uint32_t verified;	
	uint64_t nTotalRecharge;

	std::vector<UserItemInfo> vecUserItemInfo;
	std::vector<UserWearItemInfo> vecUserWearItemInfo;
	std::vector<UserFishInfo> vecUserFishInfo;
	std::vector<UserContiTaskInfo> vecUserContiTaskInfo;

	protoDCSvrUserDataSyncRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户数据同步应答
struct protoDCSvrUserDataSyncResponse : MSG_HEAD
{
	enum { IDD = PROTO_DCSVR_USERDATA_SYNC_RESPONSE };

	// 用户数据信息
	uint32_t index;
	int32_t result;

	protoDCSvrUserDataSyncResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 转发消息
struct protoRepostMsgRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_REPOST_MSG_REQUEST, };

	uint32_t uin;
	bool bBroadcast;
	uint32_t cbData;
	void* pData;

	protoRepostMsgRequest();
	~protoRepostMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoRepostMsgResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_REPOST_MSG_RESPONSE, };

	int32_t result;

	protoRepostMsgResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif	// _PROTO_USERDATA_H_
