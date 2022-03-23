/*
* =====================================================================================
*
*       Filename:  protoWealth.h
*
*    Description:  玩家财富相关协议
*
*        Version:  1.0
*        Created:  2018年03月09日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_WEALTH_H_
#define _QW_PROTO_WEALTH_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoDef.h"
#include <vector>
using namespace std;

struct ProtoItemInfo
{
	uint32_t itemTypeId;
	uint32_t num;

	ProtoItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 扣财富请求
struct protoSvrDeductWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEDUCT_WEALTH_REQUEST, };

	uint32_t uin;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	uint32_t trumpet;
	uint32_t indexId;
	uint32_t type;
	int64_t opIndex;
	
	vector<ProtoItemInfo> vecItemInfo;

	protoSvrDeductWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDeductWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEDUCT_WEALTH_RESPONSE, };

	enum eResult
	{
		Success = 0,		// 成功
		Failure = 1,		// 失败
	};

	int32_t result;
	uint32_t uin;
	uint32_t indexId;
	int64_t opIndex;
	
	protoSvrDeductWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//通知gameSvr增加金币----------------------
struct protoSvrAddCoinNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_ADDCOIN_NOTIFY };

	uint32_t	uin;
	uint32_t	coin;
	uint32_t	type;
	uint32_t	ticket;
	uint32_t	money;

	vector<ProtoItemInfo> vecItemInfo;

	enum eType
	{
		TicketExchangeCoin = 1,				//奖券兑换金币
		TicketExchangeItem = 2,				//奖券兑换道具
		Recharge = 3,						//充值
		SevenGift = 4,						//七日礼包
		OnlineGift = 5,						//在线礼包
		Mail = 6,							//邮件
		MoneyExchangeCoin = 7,				//钻石换金币
		MoidfyNick = 8,						//修改昵称
		SharpTimeGift = 9,					//整点礼包
		Talent = 10,						//达人挑战
		Compet = 11,						//竞技赛
		VipGift = 12,						//VIP礼包
		SignIn = 13,						//每日签到
		GiftKey = 14,						//礼包兑换码
		OperateActivity = 15,				//运营活动
		Alms = 16,							//救济金
		BindMsisdnReward = 17,				//绑定手机
		Cornucopia = 18,					//聚宝盆
		OfferAReward = 19,					//悬赏任务
		LevelUp = 20,						//升级
		CreateClan = 21,					//创建家族
		ExchangeFishCoin = 22,				//兑换鱼宝
		GetoutPerFund = 23,					//提取个人基金
		GetoutClanFund = 24,				//提取家族基金
		DepositClanCoin = 25,				//存入家族金币
		PresentCoinCost = 26,				//赠送金币消耗
		PresentCoinGain = 27,				//赠送金币获得
		Broadcast = 28,						//飞鸽广播
		BuyItem = 29,						//购买道具
		UseItem = 30,						// 使用道具
		PresentCrystalCost = 31,			//赠送水晶消耗
		Treasure = 32,						//宝箱
		MonthCard = 50,					// 月卡
		HappyTreasure = 60,		    //欢乐夺宝金币
		HappyTreasureItem = 61,		    //欢乐夺宝道具
		LivenessReward = 62,              //活跃度任务
		H5LoginGift = 63,					// H5登录礼包
		ExchangeOrderRefund = 64,					// 取消兑换订单后返还
		TreasureChestReward = 70,           //宝箱抽奖中奖奖励
	};

	protoSvrAddCoinNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//增加财富请求----------------------
struct protoSvrAddWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_WEALTH_REQUEST };

	uint32_t	uin;
	uint32_t	coin;
	uint32_t	type;
	uint32_t	ticket;
	uint32_t	money;
	uint32_t 	trumpet;
	uint32_t 	indexId;
	int64_t 	opIndex;
	
	vector<ProtoItemInfo> vecItemInfo;

	enum eType
	{
		TicketExchangeCoin = 1,				//奖券兑换金币
		TicketExchangeItem = 2,				//奖券兑换道具
		Recharge = 3,						//充值
		SevenGift = 4,						//七日礼包
		OnlineGift = 5,						//在线礼包
		Mail = 6,							//邮件
		MoneyExchangeCoin = 7,				//钻石换金币
		MoidfyNick = 8,						//修改昵称
		SharpTimeGift = 9,					//整点礼包
		Talent = 10,						//达人挑战
		Compet = 11,						//竞技赛
		VipGift = 12,						//VIP礼包
		SignIn = 13,						//每日签到
		GiftKey = 14,						//礼包兑换码
		OperateActivity = 15,				//运营活动
		Alms = 16,							//救济金
		BindMsisdnReward = 17,				//绑定手机
		Cornucopia = 18,					//聚宝盆
		OfferAReward = 19,					//悬赏任务
		LevelUp = 20,						//升级
		CreateClan = 21,					//创建家族
		ExchangeFishCoin = 22,				//兑换鱼宝
		GetoutPerFund = 23,					//提取个人基金
		GetoutClanFund = 24,				//提取家族基金
		DepositClanCoin = 25,				//存入家族金币
		PresentCoinCost = 26,				//赠送金币消耗
		PresentCoinGain = 27,				//赠送金币获得
		Broadcast = 28,						//飞鸽广播
		BuyItem = 29,						//购买道具
		UseItem = 30,						// 使用道具
		PresentCrystalCost = 31,			//赠送水晶消耗
		Treasure = 32,						//宝箱
		MonthCard = 50,						// 月卡
		HappyTreasure = 60,		    		//欢乐夺宝金币
		HappyTreasureItem = 61,		    	//欢乐夺宝道具
		LivenessReward = 62,              	//活跃度任务
		H5LoginGift = 63,					// H5登录礼包
		ExchangeOrderRefund = 64,			// 取消兑换订单后返还
		TreasureChestReward = 70,           //宝箱抽奖中奖奖励
	};

	protoSvrAddWealthRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrAddWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_WEALTH_RESPONSE, };

	enum eResult
	{
		Success = 0,		// 成功
		Failure = 1,		// 失败
	};

	int32_t result;
	uint32_t uin;
	uint32_t indexId;
	int64_t opIndex;
	
	protoSvrAddWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询财富请求hallsvr->gamesvr
struct protoSvrQueryWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_WEALTH_REQUEST, };

	uint32_t uin;
	uint32_t actor;
	uint32_t svrId;
	uint32_t channelId;

	protoSvrQueryWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrQueryWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_WEALTH_RESPONSE, };

	uint32_t uin;
	uint32_t actor;
	uint32_t svrId;
	uint32_t channelId;

	uint32_t coin;
	uint32_t ticket;
	uint32_t money;

	vector<ProtoItemInfo> vecItemInfo;

	protoSvrQueryWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDrawWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DRAW_WEALTH_REQUEST, };

	uint32_t uin;

	protoSvrDrawWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 外部添加下分请求(如海盗船宝船、卡牌船最终收入，要添加到下分)
struct protoExternalAddCurScoreReq : MSG_HEAD
{
	enum { IDD = PROTO_EXTERNAL_ADDCURSCORE, };

	uint32_t 		uin;
	int64_t			coin;

	protoExternalAddCurScoreReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif	// _QW_PROTO_WEALTH_H_
