/*
* =====================================================================================
*
*       Filename:  protoRedPoint.h
*
*    Description:  机器人协议
*
*        Version:  1.0
*        Created:  2018年01月12日 16时33分49秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_REDPOINT_H_
#define _QW_PROTOCOL_REDPOINT_H_


#include <vector>
#include "protoBase.h"
#include "protoDef.h"

struct protoGetRedPointStatusRequest : MSG_HEAD
{
	enum { IDD = GET_RED_POINT_STATUS_REQUEST};
	uint32_t	uin;

	protoGetRedPointStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



struct RedPonitInfo
{
	enum eType
	{
		Mail = 1,			// 邮件
		Activity = 2,		// 活动
		FreeCoin = 3,		// 免费金币
		NewbieSignin = 4,	// 新手签到
		Jackpot = 5,		// 游戏奖池
		RechargeFund = 6,	// 充值基金
	};

	enum eFreeCoinType		// 对应 redPointId
	{
		Task = 2,				// 任务
		Signin = 3,				// 签到
		SharpTimeGift = 4,		// 整点礼包
		BindMobile = 5,			// 绑定手机
	};

	enum ePointId
	{
		CheckInGameGold = 15,			// 签到赛(金币)
    	CheckInGameDiamond = 16,		// 签到赛(钻石)
    	CheckInGameLottery = 17,		// 签到赛(奖券)

    	JackpotWarPrimary = 18,			// 奖池战争(初级)
    	JackpotWarAdvanced = 19,		// 奖池战争(高级)
		
		Turnplate2 = 66,				// 转盘2

		ActivityButton = 101,
		FreeCoinButton = 102,
		MooncakeButton = 103,

		CheckInGameButton = 104,		// 宝藏抢夺赛

		JackpotWarButton = 105,			// 奖池战争
		JackpotWarPrimaryWin = 106,		// 奖池战争(初级-中奖)
		JackpotWarPrimaryBox = 107,		// 奖池战争(初级-宝箱)
		JackpotWarAdvancedWin = 108,	// 奖池战争(高级-中奖)
		JackpotWarAdvancedBox = 109,	// 奖池战争(高级-宝箱)

		DailyTask2 = 110,				// 每日任务2
		DailyTask2_Box = 111,			// 每日任务2_宝箱
		DailyTask2_Turnplate = 112,		// 每日任务2_转盘
		
		DailyRecharge = 114,			// 每日充值
		Auction = 115,					// 一元夺宝
		Auction_money= 116,				// 一元夺宝(钻石)
		Auction_coin= 117,				// 一元夺宝(金币)
		Auction_ticket= 118,			// 一元夺宝(奖券)
		
		
		NewbieSigninButton = 200,		// 新手签到
		JackpotButton = 201,			// 游戏奖池
		RechargeFundButton = 202,		// 充值基金
	};

	enum eStatus
	{
		Close = 0,
		Open = 1,
	};


	uint32_t  redPointType;	// 红点类型： 1为邮件，2为活动，3 免费金币
	// 红点ID：邮件为1，2-任务  3-签到  4-整点礼包 5-绑定手机  活动为活动类型   
	// ActivityButton = 101   FreeCoinButton = 102
	uint32_t  redPointId;	
	uint32_t  status;		// 状态:0/1
	uint32_t  clearFlag;
	char reserve[256];		// 保留字段

	RedPonitInfo();
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	uint32_t getLength(void);
};



struct protoGetRedPointStatusResponse : MSG_HEAD
{
	enum { IDD = GET_RED_POINT_STATUS_RESPONSE };

	
	std::vector<RedPonitInfo> vecRedPointStatus;

	protoGetRedPointStatusResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};





struct protoSetRedPointStatusRequest : MSG_HEAD
{
	enum { IDD = SET_RED_POINT_STATUS_REQUEST};
	uint32_t	uin;
	uint32_t  redPointType;
	uint32_t  redPointId;
	uint32_t  status;
	uint32_t  clearFlag;
	char reserve[256];

	protoSetRedPointStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};




struct protoSetRedPointStatusResponse : MSG_HEAD
{
	enum { IDD = SET_RED_POINT_STATUS_RESPONSE};
	uint32_t	result;


	protoSetRedPointStatusResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrRedPointNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_RED_POINT_NOTIFY, };

	uint32_t uin;
	uint32_t type;
	uint32_t id;
	uint32_t status;

	protoSvrRedPointNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif


