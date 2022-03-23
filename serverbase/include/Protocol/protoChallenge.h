/*
* =====================================================================================
*
*       Filename:  protoChallenge.h
*
*    Description:  新竞技赛相关协议
*
*        Version:  1.0
*        Created:  2016年12月1日 13时30分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_CHALLENGE_H_
#define _QW_PROTOCOL_CHALLENGE_H_

#include "protoDef.h"
#include "protoBase.h"
#include "protoRank.h"
#include <vector>
using namespace std;

// 新竞技赛报名请求
struct protoChallengeSignUpRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_SIGNUP_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeSignUpRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛报名应答
struct protoChallengeSignUpResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_SIGNUP_RESPONSE, };

	int32_t result;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t cost;
	uint32_t costType;
	uint32_t number;

	protoChallengeSignUpResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛状态通知
struct protoChallengeStatusNotify : MSG_HEAD
{
	enum  { IDD = CHALLENGE_STATUS_NOTIFY, };

	uint32_t roomId;
	uint32_t scheduleId;
	int32_t status;
	char beginTime[32];
	uint32_t duration;
	uint32_t number;
	uint32_t roomType;

	protoChallengeStatusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛比赛列表请求
struct protoChallengeListRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_LIST_REQUEST, };

	uint32_t uin;
	uint32_t roomFlag;

	protoChallengeListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct ChallengeRoomData
{
	uint32_t roomId;
	uint32_t scheduleId;
	int32_t status;
	uint32_t cost;
	uint32_t costType;
	char beginTime[32];
	uint32_t duration;
	uint32_t number;
	uint32_t roomType;
	uint32_t minNumber;
	uint32_t maxNumber;
	char icon[128];
	char roomName[64];

	ChallengeRoomData();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);

};

// 新竞技赛比赛列表应答
struct protoChallengeListResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_LIST_RESPONSE, };

	uint32_t roomFlag;
	vector<ChallengeRoomData> roomList;

	protoChallengeListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 新竞技赛排行榜请求
struct protoChallengeRankRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛排行榜应答
struct protoChallengeRankResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_RANK_RESPONSE, };

	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;

	protoChallengeRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 新竞技赛排行榜请求
struct protoChallengeDynamicRankRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_DYNAMIC_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeDynamicRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛排行榜应答
struct protoChallengeDynamicRankResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_DYNAMIC_RANK_RESPONSE, };

	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;

	protoChallengeDynamicRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*-------------------------------------------------------------------------------------------------------------*/

struct SignUpUserInfo
{
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;

	SignUpUserInfo()
	{
		uin = 0;
		tableId = 0;
		seatId = 0;
	}

	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};
// competsvr -> gamesvr 新竞技赛场次信息同步
struct protoSvrChallengeInfoSync : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_INFO_SYNC, };

	uint32_t roomId;
	uint32_t scheduleId;
	time_t signUpBeginTime;
	time_t signUpEndTime;
	time_t gameBeginTime;
	time_t gameEndTime;
	uint32_t roomFlag;
	uint32_t minNum;
	uint32_t maxNum;
	uint32_t status;
	uint32_t gamesvrId;	
	uint32_t signUpNum;
	vector<SignUpUserInfo> vecSignUpUser;		// 报名的用户
	char twRoomName[64];				// 繁体名称
	char engRoomName[64];				// 英文名称
	char chnRoomName[64];				// 中文名称
	char icon[256];						// 房间图标URL

	protoSvrChallengeInfoSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr 新竞技赛报名请求
struct protoSvrChallengeSignUpRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SIGNUP_REQ, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t vip;
	char nick[64];

	protoSvrChallengeSignUpRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr 新竞技赛报名应答
struct protoSvrChallengeSignUpResponse : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SIGNUP_RESP, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	int32_t result;

	enum eResult
	{
		Success = 0,
		Full,					// 已满
		SignUped,				// 已经报名
		ScheduleNotFound,		// 场次没找到
		StatusError,			// 不是报名状态
		CurrencyNotEnough,		// 货币不足
	};

	protoSvrChallengeSignUpResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr 新竞技赛排行榜请求
struct protoSvrChallengeRankReq : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RANK_REQ, };

	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;

	protoSvrChallengeRankReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr 新竞技赛排行榜应答
struct protoSvrChallengeRankResp : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RANK_RESP, };

	vector<NewRankInfoVip> vecRank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;
	uint32_t rank;
	uint32_t score;

	protoSvrChallengeRankResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr 新竞技赛比赛分数登记
struct protoSvrChallengeScoreReg : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SCORE_REG, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;
	uint32_t vip;
	char nick[64];

	protoSvrChallengeScoreReg();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> competsvr 新竞技赛动态排行榜请求
struct protoSvrChallengeDynamicRankReq : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_DYNAMIC_RANK_REQ, };

	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;

	protoSvrChallengeDynamicRankReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr 新竞技赛动态排行榜应答
struct protoSvrChallengeDynamicRankResp : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_DYNAMIC_RANK_RESP, };

	vector<NewRankInfoVip> vecRank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;
	uint32_t rank;
	uint32_t score;

	protoSvrChallengeDynamicRankResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛我的比赛请求
struct protoSvrChallengeMyselfRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_MYSELF_REQ, };

	uint32_t uin;
	uint32_t roomFlag;
	uint32_t language;

	protoSvrChallengeMyselfRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛我的比赛应答
struct protoSvrChallengeMyselfResponse : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_MYSELF_RESP, };

	uint32_t uin;
	uint32_t roomFlag;
	vector<ChallengeRoomData> roomList;

	protoSvrChallengeMyselfResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新竞技赛重置比赛通知
struct protoSvrChallengeResetNotify : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RESET_NOTIFY, };

	protoSvrChallengeResetNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*比赛活动信息*/
struct protoHallCompeteInfoRequest : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_INFO_REQUEST, };

	uint32_t uin;

	protoHallCompeteInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoHallCompeteInfoResponse : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_INFO_RESPONSE, };

	uint32_t uin;
	uint32_t beginTime;
	uint32_t svrTime;
	uint32_t roomId;
	vector<uint32_t> enterRoomIds;
	uint32_t curRewardRoomId;
	vector<uint32_t> rewardRoomIds;

	protoHallCompeteInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*比赛活动排行榜*/
struct protoHallCompeteRankRequest : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t type;

	enum eType
	{
		TopSix = 0,			// 前6名
		TopTen = 1,			// 前10名
	};

	protoHallCompeteRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoHallCompeteRankResponse : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_RANK_RESPONSE, };

	uint32_t uin;
	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint64_t score;
	uint32_t beginTime;
	uint32_t endTime;
	uint32_t type;

	protoHallCompeteRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> center 获得和消耗统计
struct protoSvrGainNotify : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_GAIN_NOTIFY, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t gain;
	uint32_t cost;
	uint32_t vip;
	char nick[64];

	protoSvrGainNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoPonitCompeteStatusNotify : MSG_HEAD
{
	enum  { IDD = POINT_COMPETE_STATUS_NOTIFY, };

	uint32_t status;
	uint32_t time;
	uint32_t svrTime;
	uint32_t roomId;
	vector<uint32_t> enterRoomIds;
	uint32_t scheduleId;

	protoPonitCompeteStatusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif	// _QW_PROTOCOL_CHALLENGE_H_
