/*
 * =====================================================================================
 *
 *       Filename:  protoCompet.h
 *
 *    Description:  竞技场相关协议
 *
 *        Version:  1.0
 *        Created:  2015年11月14日 18时26分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_COMPET_PROTOCOL_H_
#define _QW_CRAZYFISH_COMPET_PROTOCOL_H_

#include "protoDef.h"
#include "protoBase.h"
#include "protoRank.h"
#include <vector>

// client -> gamesvr
struct CompetRankRequest : MSG_HEAD
{
	enum  { IDD = COMPET_RANK_REQUEST, };

	uint32_t uin;
	uint32_t room_id;
	uint32_t date_flag;

	CompetRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct CompetRankRespone : MSG_HEAD
{
	enum  { IDD = COMPET_RANK_RESPONE, };

	uint32_t date_flag;
	uint32_t reward_flag;
	int64_t score;
	int32_t rank;
	std::vector<NewRankInfo> rank_list;

	CompetRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// client -> gamesvr
struct CompetRankNewRequest : MSG_HEAD
{
	enum  { IDD = COMPET_RANK_NEW_REQUEST, };

	uint32_t uin;
	uint32_t room_id;
	uint32_t date_flag;

	CompetRankNewRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct CompetRankNewRespone : MSG_HEAD
{
	enum  { IDD = COMPET_RANK_NEW_RESPONE, };

	uint32_t date_flag;
	uint32_t reward_flag;
	int64_t score;
	int32_t rank;
	std::vector<NewRankInfoVip> rank_list;

	CompetRankNewRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// client -> gamesvr
struct CompetRewardRequest : MSG_HEAD
{
	enum  { IDD = COMPET_REWARD_REQUEST, };

	uint32_t uin;
	uint32_t room_id;

	CompetRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct CompetRewardRespone : MSG_HEAD
{
	enum  { IDD = COMPET_REWARD_RESPONE, };

	uint32_t reward_id;

	CompetRewardRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr (broadcast)
struct protoCompetSchedule: MSG_HEAD
{
	enum  { IDD = PROTO_BG_COMPET_SCHEDULE , };
	enum Status
	{
		competStart = 1,
		competEnd,
		competPending,		// 等待开启
		competRunning,		// 开启中
	};

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	competbegin;
	uint32_t	competend;
	int32_t		flag;		// 参见 Status 定义

	protoCompetSchedule();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoCompetGain : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_COMPET_GAIN, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	gain;	// gain coin
	char		nick[32];
	uint32_t	vip;

	protoCompetGain();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr (broadcast)
struct protoCompetNotify : MSG_HEAD
{
	enum  { IDD = PROTO_BG_COMPET_NOTIFY, };
	enum Notify
	{
		notifyRankRenew = 1,	// 排行有更新
		notifyRankFinal,		// 排行最终结果
		notifyRankReward,		// 开始排行奖励
	};

	int32_t		scheduleId;
	uint32_t	roomId;
	int32_t		flag;		// 参见Notify定义

	protoCompetNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> bgsvr
struct protoCompetRankRequestNew : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_COMPETRANK_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;

	protoCompetRankRequestNew(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr
struct protoCompetRankResponeNew : MSG_HEAD
{
	enum  { IDD = PROTO_BG_COMPETRANK_RESP, };
	struct RankData
	{
		uint32_t	uin;
		char		nick[32];
		int64_t		score;
		int32_t		rank;
		int32_t		reward_flag;	// 0 - not reward, 1 - reward, 2 -no reward, 3-expire
		uint32_t	vip;

		RankData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::vector<RankData>	RANK_VEC;

	int32_t		scheduleId;
	uint32_t	roomId;
	RANK_VEC	vecRank;

	protoCompetRankResponeNew(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoCompetRewardRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETREWARD_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;

	protoCompetRewardRequest(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr 
struct protoCompetRewardRespone : MSG_HEAD
{
	enum  { IDD = PROTO_BG_COMPETREWARD_RESP ,};
	enum Result
	{
		RewardOK = 1,
		SystemFalut,		// 系统错误，一般是未创建存储过程
		InvalidSchedule,	// 不存在该竞技场次
		RewardExpire,	 	// 该场次的奖励已过期
		NotEnterCompet,		// 未参加该场次的兑技
		NotInRank,			// 未进入排行
		AlreadyReward,		// 已奖励
		NoReward,			// 无奖励
		RewardFailure,		// 奖励处理失败，有可能是更新数据失败
	};

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		result;	// see enum Result

	protoCompetRewardRespone(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoCompetUserScoreRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETUSERSCORE_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	dateFlag;
	uint32_t	newFlag;

	protoCompetUserScoreRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr
struct protoCompetUserScoreRespone : MSG_HEAD
{
	enum  { IDD = PROTO_BG_COMPETUSERSCORE_RESP, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	dateFlag;
	int64_t		score;
	int32_t		rank;	// 0表示未排行，最多有30秒的误差
	uint32_t newFlag;

	protoCompetUserScoreRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr
struct protoSyncCompetUserNick : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_SYNCCOMPETUSERNICK, };

	uint32_t	uin;
	char		nick[64];

	protoSyncCompetUserNick();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// client -> gamesvr
struct protoWeeklyCompetRankRequest : MSG_HEAD
{
    enum  { IDD = PROTO_WEEKLY_COMPET_RANK_REQUEST, };

    uint32_t uin;
    uint32_t room_id;
    uint32_t date_flag;

    protoWeeklyCompetRankRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

// gamesvr -> client
struct protoWeeklyCompetRankRespone : MSG_HEAD
{
    enum  { IDD = PROTO_WEEKLY_COMPET_RANK_RESPONE, };

    uint32_t date_flag;
    uint32_t reward_flag;
    int64_t score;
    int32_t rank;
    std::vector<NewRankInfoVip> rank_list;

    protoWeeklyCompetRankRespone();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

// gamesvr->competsvr
struct protoSvrSyncCompetInfoRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_SYNC_COMPETINFO_REQ, };

	uint32_t reserve;

	protoSvrSyncCompetInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_CRAZYFISH_COMPET_PROTOCOL_H_


