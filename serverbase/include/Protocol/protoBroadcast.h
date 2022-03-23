/*
* =====================================================================================
*
*       Filename:  protoBroadcast.h
*
*    Description:  广播协议
*
*        Version:  1.0
*        Created:  2016年5月18日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/


#ifndef _QW_PROTOCOL_BROADCAST_H_
#define _QW_PROTOCOL_BROADCAST_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

// 捕鱼广播
struct protoFishBroadcastRespone : MSG_HEAD
{
	enum { IDD = FISH_BROADCAST_RESPONE, };
	char nick[64];
	uint32_t roomId;
	uint32_t fishId;
	uint32_t coin;

	protoFishBroadcastRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


// 实物订单广播请求
struct protoOrderBroadcastRequest : MSG_HEAD
{
	enum { IDD = ORDER_BROADCAST_REQUEST, };
	
	uint32_t uin;

	protoOrderBroadcastRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct OrderBroadcastInfo
{
	char nick[64];
	char time[16];
	uint32_t itemId;

	OrderBroadcastInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 订单兑换广播
struct protoOrderBroadcastRespone : MSG_HEAD
{
	enum { IDD = ORDER_BROADCAST_RESPONE, };

	std::vector<OrderBroadcastInfo> vecOrderBroadcast;

	protoOrderBroadcastRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSystemBroadcastResponse : MSG_HEAD
{
	enum { IDD = SYSTEMBROADCASTRESPONE, };

	enum eType
	{
		FlyingPigeon = 0,				// 飞鸽广播
		ExchangeCrystal = 1,			// 兑换水晶/炮座 广播
		PresentCrystal = 2,				// 赠送水晶/炮座 广播
		HitItem = 3,					// 打中道具        
		VipRoom = 5,					// vip专场广播
        AuctionWin = 6,             	// 一元购中奖

		DailyReset = 1000,				// 跨天通知(每日0点)
		LanguageUpdate = 1010,			// 语言表更新

	};

	uint32_t type;
	char data[1024];

	protoSystemBroadcastResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//WEB兑换商城广播
struct protoGiftShopBroadcastRequest : MSG_HEAD
{
	enum { IDD = GIFTSHOP_BRAODCAST_REQUEST };

	uint32_t seq;
	uint32_t uin;
	char title[256];
	char content[1024];
	uint32_t cType;

	protoGiftShopBroadcastRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//WEB兑换商城广播
struct protoGiftShopBroadcastResponse : MSG_HEAD
{
	enum { IDD = GIFTSHOP_BRAODCAST_RESPONSE};

	uint32_t seq;							//请求中的包序列号，原值返回
	int32_t result;						//扣除结果， 0：成功， 1：用户不在线， -1：其它失败


	protoGiftShopBroadcastResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//弹窗请求
struct protoQueryAdRequest : MSG_HEAD
{
	enum { IDD = QUERY_AD_REQUEST};
	uint32_t uin;						// 1: web链接
		


	protoQueryAdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//弹窗应答
struct protoQueryAdResponse : MSG_HEAD
{
	enum { IDD = QUERY_AD_RESPONSE};
	char icon[128];
	uint32_t action;						// 1: web链接
	char content[512];						


	protoQueryAdResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 发送飞鸽广播请求
struct protoSendBroadcastRequest : MSG_HEAD
{
	enum { IDD = SEND_BROADCAST_REQUEST, };

	uint32_t uin;
	char msg[512];

	protoSendBroadcastRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 发送飞鸽广播请求
struct protoSendBroadcastResponse : MSG_HEAD
{
	enum { IDD = SEND_BROADCAST_RESPONSE, };

	int32_t result;
	uint32_t cd;

	protoSendBroadcastResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


// 发送飞鸽广播请求
struct protoSvrSendBroadcastCostRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_SEND_BROADCAST_COST_REQ, };

	uint32_t uin;
	char msg[512];
	uint32_t trumpet;
	uint32_t type;
	uint32_t cd;

	protoSvrSendBroadcastCostRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 发送飞鸽广播请求
struct protoSvrSendBroadcastCostResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_SEND_BROADCAST_COST_RESP, };

	uint32_t uin;
	char msg[512];
	uint32_t trumpet;
	uint32_t type;
	uint32_t cd;
	int32_t result;

	protoSvrSendBroadcastCostResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct ItemBroadcastInfo
{
    char nick[64];
    char time[16];
    char itemName[128];
    uint32_t num;

    ItemBroadcastInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 道具兑换广播
struct protoItemBroadcastNotify : MSG_HEAD
{
    enum { IDD = ITEM_BROADCAST_RESPONE, };

    std::vector<ItemBroadcastInfo> vecItemBroadcast;

    protoItemBroadcastNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


// 捕鱼广播 to authsvr
struct protoSvrFishBroadcastNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_FISH_BROADCAST_NOTIFY, };

	uint32_t uin;
	char nick[64];
	uint32_t roomId;
	uint32_t fishId;
	uint32_t coin;
	char fishName[64];

	protoSvrFishBroadcastNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


// 客户端统一广播 to client
struct protoSystemUnifyBroadcast : MSG_HEAD
{
	enum { IDD = PROTO_SYSTEM_UNIFY_BROADCAST, };
	
	enum BROADCAST_POSITION
	{
		POS_HALL = 0x01,				// 大厅显示
		POS_ROOM = 0x02,				// 捕鱼房间显示
		POS_MALL = 0x04,				// 兑换商城		
		
		POS_NULL = 0xFFFF				//  不显示
	};

	enum BROADCAST_STYLE
	{
		STYLE_NORMAL = 1,				// 普通样式，拼完整的字符串
		STYLE_FISH = 2,					// 捕鱼广播 3=海盗船宝船, 4=海盗船卡片船
        SKILL_FISH = 5,                 // 技能
		VIP_LOGIN  = 6,                 // vip登录
		DAILY_RECHARGE_REWARD = 7,			// 每日充值奖池固定金额奖励
		DAILY_RECHARGE_JACKPOT_REWARD = 8,	// 每日充值奖池比例金额奖励
		TREASURE_POT_REWARD	= 9,			//聚宝盆
		
		STYLE_NULL = 0xFFFF				//  不显示
	};

	enum BROADCAST_FUNC
	{
		FUNC_NORMAL 	= 1,				// 通用
		FUNC_ROOM 		= 2,				// 捕鱼
		FUNC_MALL 		= 3,				// 兑换	
		FUNC_MOONCATE 	= 4,				// 博饼	
		FUNC_TURNPLATE 	= 5,				// 转盘		6=奖池战争, 7=周年活动, 8=海盗船
		FUNC_LASER      = 9,                // 激光炮
        FUNC_BLACKHOLE  = 10,               // 黑洞
		FUNC_VIP_LOGIN	= 11,				// vip登录
		FUNC_DAILY_RECHARGE =12,				// 每日充值奖池奖励
		FUNC_TREASURE_POT = 13, 			//聚宝盆
        FUNC_LIGHTNING  = 14,               // 闪电(鱼)
        FUNC_BOMB       = 15,               // 炸弹(鱼)
		FUNC_DRILL		= 16,				// 钻头鱼
		FUNC_MACHINEGUN = 17,				// 机枪鱼
		FUNC_GOLDENEGG	= 18,				// 金蛋鱼
		FUNC_CARD		= 19,				// 卡牌鱼
		FUNC_GIFT		= 20,				// 礼物鱼
		FUNC_SLOTMACHINE = 21,				// 老虎机鱼
		FUNC_NULL = 0xFFFF					//  不显示
		
	};
	
	uint32_t postion;
	uint32_t style;
	uint32_t func;
	char content[512];
	vector<string> contentList;
	

	protoSystemUnifyBroadcast();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif		// _QW_PROTOCOL_BROADCAST_H_
