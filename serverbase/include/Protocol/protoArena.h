/*
* =====================================================================================
*
*       Filename:  protoArena.h
*
*    Description:  竞技赛协议
*
*        Version:  1.0
*        Created:  2016年5月27日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_ARENA_H_
#define _PROTO_ARENA_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
#include "protoRank.h"

//用户竞技赛信息请求, client->gamesvr
struct protoUserArenaInfoRequest : MSG_HEAD
{
	enum { IDD = USER_ARENAINTO_REQUEST };

	uint32_t	uin;
	uint32_t	roomId;

	protoUserArenaInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoUserArenaInfoRespone : MSG_HEAD
{
	enum { IDD = USER_ARENAINTO_RESPONE };

	int32_t costType;
	uint32_t costNum;
	uint32_t rank;
	uint32_t maxScore;
	uint32_t reserve;

	protoUserArenaInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 查询用户竞技赛信息请求，gamesvr->比赛服务器
struct protoQueryUserArenaInfoRequest : MSG_HEAD
{
	enum  { IDD = PROTO_QUERY_USERARENAINFO_REQ, };
	uint32_t	uin;
	uint32_t	roomId;

	protoQueryUserArenaInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询用户竞技赛信息应答
struct protoQueryUserArenaInfoRespone : MSG_HEAD
{
	enum  { IDD = PROTO_QUERY_USERARENAINFO_RESP, };

	uint32_t	uin;
	uint32_t	roomId;
	uint32_t	rank;
	uint32_t	maxScore;

	protoQueryUserArenaInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



//竞技赛排名请求, client->gamesvr
struct protoArenaRankRequest : MSG_HEAD
{
	enum { IDD = ARENA_RANK_REQUEST };

	uint32_t uin;
	uint32_t roomId;

	protoArenaRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct ArenaRankInfo
{
	uint32_t uin;
	char nick[32];
	int64_t score;
	int32_t rank;

	ArenaRankInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 竞技赛排名响应
struct protoArenaRankRespone : MSG_HEAD
{
	enum { IDD = ARENA_RANK_RESPONE, };


	vector<NewRankInfoVip> vecArenaRankInfo;

	protoArenaRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//竞技赛前3名请求
struct protoArenaTopThreeRequest : MSG_HEAD
{
	enum { IDD = ARENA_TOPTHREE_REQUEST };

	uint32_t uin;
	uint32_t roomId;

	protoArenaTopThreeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//竞技赛排名响应
struct protoArenaTopThreeRespone : MSG_HEAD
{
	enum { IDD = ARENA_TOPTHREE_RESPONE };

	vector<NewRankInfoVip> vecArenaRankInfo;

	protoArenaTopThreeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoArenaRoomInfoNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETROOMINFO_NOTIFY };

	uint32_t	roomId;
	time_t		openTime;
	time_t		endTime;
	int32_t		status;
	uint32_t	stayTime;
	time_t		nextTime;

	protoArenaRoomInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoArenaRankRenewRespone : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETRANK_RESPONE };

	uint32_t	roomId;
	vector<NewRankInfoVip> vecArenaRankInfo;

	protoArenaRankRenewRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoArenaRenewNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETRENEW_NOTIFY };

	uint32_t	roomId;

	protoArenaRenewNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoArenaRankRenewRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETRANK_REQUEST };

	uint32_t	roomId;

	protoArenaRankRenewRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_ARENA_H_

