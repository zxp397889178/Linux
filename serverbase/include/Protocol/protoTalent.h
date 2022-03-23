/*
 * =====================================================================================
 *
 *       Filename:  protoTalent.h
 *
 *    Description:  达人挑战相关协议
 *
 *        Version:  1.0
 *        Created:  2016年04月07日 11时00分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xiejiafeng
 *   Organization:  quwan
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_TALENT_PROTOCOL_H_
#define _QW_CRAZYFISH_TALENT_PROTOCOL_H_

#include "protoDef.h"
#include "protoBase.h"
#include "protoRank.h"
#include <vector>

// client -> gamesvr
struct TalentLookRequest : MSG_HEAD
{
	enum  { IDD = TALENT_LOOK_REQ, };

	uint32_t uin;
	uint32_t roomId;

	TalentLookRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct TalentLookRespone : MSG_HEAD
{
	enum  { IDD = TALENT_LOOK_RESP, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		flag;		// 参见 Status 定义
	uint32_t	time;		// 剩余时间
	int32_t		curNum;		// 当前人数
	int32_t		maxNum;		// 最大人数
	int32_t		costType;
	uint32_t	costNum;

	TalentLookRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// client -> gamesvr
struct TalentSignupRequest : MSG_HEAD
{
	enum { IDD = TALENT_SIGNUP_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	char		nick[32];

	TalentSignupRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct TalentSignupRespone : MSG_HEAD
{
	enum  { IDD = TALENT_SIGNUP_RESP, };
	enum Result
	{
		SignupOK		= 1,		// 报名成功
		SignupFalut		= 2,		// 系统错误
		SignupAlready	= 3,		// 已报名
		SignupMax		= 4,		// 人数已满
		SignupWait		= 5,		// 等待报名
	};

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		result;	// see enum Result

	TalentSignupRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// client -> gamesvr
struct TalentRankRequest : MSG_HEAD
{
	enum { IDD = TALENT_RANK_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;

	TalentRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct TalentRankRespone : MSG_HEAD
{
	enum  { IDD = TALENT_RANK_RESP, };

	std::vector<NewRankInfoVip> rankList;
	uint32_t uin;

	TalentRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> client
struct TalentStatus : MSG_HEAD
{
	enum  { IDD = TALENT_STATUS, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		result;
	uint32_t	time;

	TalentStatus();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoTalentLookRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_TALENT_LOOK_REQ, };

	uint32_t uin;
	uint32_t roomId;

	protoTalentLookRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr
struct protoTalentLookRespone: MSG_HEAD
{
	enum  { IDD = PROTO_BG_TALENT_LOOK_RESP, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		flag;		// 参见 Status 定义
	uint32_t	time;		// 剩余时间
	int32_t		curNum;		// 当前人数
	int32_t		maxNum;		// 最大人数

	protoTalentLookRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoTalentSignupRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_TALENT_SIGNUP_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	vip;
	char		nick[32];

	protoTalentSignupRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr 
struct protoTalentSignupRespone : MSG_HEAD
{
	enum  { IDD = PROTO_BG_TALENT_SIGNUP_RESP, };
	enum Result
	{
		SignupOK		= 1,		// 报名成功
		SignupFalut		= 2,		// 系统错误
		SignupAlready	= 3,		// 已报名
		SignupMax		= 4,		// 人数已满
		SignupWait		= 5,		// 等待报名
	};

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		result;	// see enum Result

	protoTalentSignupRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoTalentRankRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_TALENT_RANK_REQ, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;

	protoTalentRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr
struct protoTalentRankRespone : MSG_HEAD
{
	enum  { IDD = PROTO_BG_TALENT_RANK_RESP, };

	std::vector<NewRankInfoVip> rankList;
	int32_t scheduleId;
	uint32_t roomId;
	int32_t status;
	time_t tInterval;		// 当前场次间隔时间
	time_t tSignup;			// 当前场次报名时间
	time_t tStart;			// 当前场次开始时间
	time_t tEnd;			// 当前场次结束时间

	protoTalentRankRespone(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> bgsvr
struct protoTalentGain : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_TALENT_GAIN, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	gain;	// gain coin
	uint32_t	vip;

	protoTalentGain();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// bgsvr -> gamesvr 
struct protoTalentStatus : MSG_HEAD
{
	enum  { IDD = PROTO_BG_TALENT_STATUS, };

	int32_t		scheduleId;
	uint32_t	roomId;
	uint32_t	uin;
	int32_t		result;

	protoTalentStatus();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_CRAZYFISH_TALENT_PROTOCOL_H_


