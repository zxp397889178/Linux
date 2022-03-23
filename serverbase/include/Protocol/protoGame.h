/*
 * =====================================================================================
 *
 *       Filename:  protoGame.h
 *
 *    Description:  游戏逻辑里的通讯协议
 *
 *        Version:  1.0
 *        Created:  2016年01月19日 15时47分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_PROTOCOL_GAME_H_
#define _QW_CRAZYFISH_PROTOCOL_GAME_H_

#include "BaseCode.h"
#include "protoBase.h"
#include "protoDef.h"

// 双倍奖券通知(能源炮)
struct protoDoubleRewardNotice : MSG_HEAD
{
	enum { IDD = PROTO_DOUBLE_REWARD, };

	int16_t		seatId;
	int32_t		timeLeft;	// 剩余时间，以秒计算，<=0 表示双倍奖励结束	

	protoDoubleRewardNotice();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 双倍奖励准备（能源炮）
struct protoDoubleRewardReady : MSG_HEAD
{
	enum { IDD = PROTO_DOUBLE_REWARD_READY, };

	int16_t		seatId;
	uint32_t	fishInsId;

	protoDoubleRewardReady();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoTreasureRewardNotify : MSG_HEAD
{
	enum { IDD = PROTO_TREASURE_REWARD_NOTIFY, };

	uint32_t	uin;
	std::vector<uint32_t> vecReward;
	char		    key[16];			//验证码

	protoTreasureRewardNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoTreasureRewardRequest : MSG_HEAD
{
	enum { IDD = PROTO_TREASURE_REWARD_REQUEST, };

	uint32_t	uin;
	int32_t		rewardId;
	char		    key[16];			//验证码

	protoTreasureRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoTreasureRewardResponse : MSG_HEAD
{
	enum { IDD = PROTO_TREASURE_REWARD_RESPONSE, };

	uint32_t	result;
	int32_t		rewardCoin;

	protoTreasureRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_CRAZYFISH_PROTOCOL_GAME_H_


