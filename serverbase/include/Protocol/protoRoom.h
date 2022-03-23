/*
 * =====================================================================================
 *
 *       Filename:  protoRoom.h
 *
 *    Description:  在房间中战斗及相关协议
 *
 *        Version:  1.0
 *        Created:  2015年09月18日 21时48分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_GAMESVR_PROTO_ROOH_H_
#define _QW_GAMESVR_PROTO_ROOH_H_

#include <vector>
#include <map>
#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoUserData.h"

// 请求房间列表
struct protoRoomListReq : MSG_HEAD
{
	enum { IDD = ROOMLISTREQUEST, };
	uint32_t	uin;
	uint32_t	gameTypeId;
	uint32_t	areaTypeId;

	protoRoomListReq(uint32_t _uin=0, uint32_t _gameId=0, uint32_t _areaId=0);
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
};

// 请求房间列表应答
struct protoRoomListResp : MSG_HEAD
{
	enum { IDD = ROOMLISTRESPONE,};
	struct RoomData
	{
		uint32_t 	room_id;
		char 		room_name[64];
		uint8_t 	room_flag;	
		uint32_t 	online_numbers;	// 人数，有加上随机值
		uint32_t	mincoin;
		char 		gunlist[64];
		// 以下数据为挑战房的数据，普通房及pvp房不使用
		uint32_t 	chage_count;		// 挑战次数
		uint32_t 	maxchage_count;		// 最大挑战次数
		uint32_t 	openflag;			// 挑战房的开启状态: 1-未开启; 0-已开启
		uint32_t 	lefttime;			// 本场剩余时间,以秒计算; 若未开启，则为下一次开启的时间
		char 		endtime[20];		// 结束时间: HH:MM:SS
		char 		nexttime[20];		// 下次开放时间: HH:MM:SS
		uint32_t 	staytime;			// 停留时间
		uint32_t 	cur_cost;			// 当前参赛费 
		uint32_t 	cur_score;			// 当前房间最高分
		uint32_t 	user_score;			// 用户当最高分 
		uint32_t 	user_ranking;		// 用户排名

		RoomData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	uint32_t 				areaTypeId;
	std::vector<RoomData>	vecRoomData;

	protoRoomListResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 获取指定房间信息的请求
struct protoRoomInfoReq : MSG_HEAD
{
	enum {IDD = DESIGNROOMINFOREQUEST,};
	uint32_t		uin;
	uint32_t		roomId;

	protoRoomInfoReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 获得指定房间信息的应答
struct protoRoomInfoResp : MSG_HEAD
{
	enum {IDD = DESIGNROOMINFORESPONE,};
	typedef protoRoomListResp::RoomData		RoomData;
	RoomData		data;

	protoRoomInfoResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};


// 快速进入游戏请求
struct protoFastEnterRoomReq : MSG_HEAD
{
	enum {IDD = FASTINTOROOMREQUEST, };

	uint32_t		uin;
	uint32_t		gameTypeId;

	protoFastEnterRoomReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 进入房间请求
struct protoEnterRoomReq : MSG_HEAD
{
	enum { IDD = INTOGAMEREQUEST, };
	uint32_t		uin;
	uint32_t		roomId;
	uint32_t		isRobot;

	protoEnterRoomReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 进入房间应答
struct protoEnterRoomResp : MSG_HEAD
{
	enum { IDD = INTOGAMERESPONE, };
	enum Result
	{
		Success = 1,				// 成功进入房间
		NeedCostItem = 2,			// 需要向hallsvr请求扣道具
		LackOfCoin = -1,			// 金币不足
		NoChance = -2,				// 参赛次数不够
		RoomUnOpen = -3,			// 房间未开放
		SeatDisable = -4,			// 座位不可用
		LackOfMoney = -5,			// 趣玩币不足
		LackOfTicket = -6,			// 奖券不足
		LevelLimit = -7,			// 等级不足
		VipLimit = -8,				// VIP不足
		UserFull = -9,				// 房间人数已满
		LackOfItem = -10,			// 道具不足
		TooManyCoin = -11,			// 身上金币超过房间限额
		TooManyMoney = -12,			// 身上趣玩币超过房间限额
		TooManyTicket = -13,		// 奖券超过房间限额
		GunMountNotWeared = -14,	// 未装备对应炮座
	};

	struct GameUserData
	{
		uint32_t 	uin;
		char 		name[64];
		uint32_t 	seatid;
		uint32_t 	bullet;
		uint8_t 	gunid;
		uint32_t 	score;

		GameUserData();
		uint32_t getLength(void);
		int32_t Serialize(void *pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::vector<GameUserData>	USER_VEC;	

	int16_t			result;
	uint32_t		roomId;
	USER_VEC		vecUsers;
	uint32_t		maxScore;
	uint32_t		timeLeft;

	//返回是否允许发炮
	uint8_t		shooting;
	uint32_t	shootingCondition;
	
	protoEnterRoomResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 玩家进入房间通知(通知同桌子的其他玩家)
struct protoPlayerEnterRoomNotify : MSG_HEAD
{
 	enum { IDD = PLAYERJOINRESPONE, }; 
	uint32_t uin;
	char name[64];
	uint32_t seatid;
	uint32_t bullet;
	int16_t gunid;
	uint32_t score;

	protoPlayerEnterRoomNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 场景状态通知
struct protoSceneStateNotify : MSG_HEAD
{
	enum { IDD = SCENERESPONE,};
	typedef std::vector<uint32_t>	FISHID_VEC;	
	uint32_t		sceneId;
	uint32_t		timePass;
	FISHID_VEC		arFishId;

	protoSceneStateNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 场景暂停通知
struct protoScenePauseNotify: MSG_HEAD
{
	enum { IDD = SCENESTATERESPONE, };
	enum { PAUSE = 0, RESUME = 1, };

	uint32_t		sceneId;
	uint32_t		timePass;
	uint32_t		pauseState;

	protoScenePauseNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

#if 0
// 上下分请求
struct protoChangeBulletReq : MSG_HEAD
{
	enum { IDD = CHANGEBULLETREQUEST, };
	enum { PLAYER = 0, ROBOT =1, };

	uint32_t		uin;
	int16_t			action;
	uint32_t		isRobot;

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};
#endif

// 切换炮台请求
struct protoChangeGunReq : MSG_HEAD
{
	enum { IDD = CHANGEGUNREQUEST, };
	uint32_t		uin;
	int16_t			gunId;

	protoChangeGunReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 发射子弹请求
struct protoPlayerFireReq : MSG_HEAD
{
	enum { IDD = SHOOTREQUEST, };
	enum { PLAYER =0, ROBOT = 1, };

	uint32_t		uin;
	uint32_t		bulletOrder;
	int32_t			x;
	int32_t			y;
	int32_t			isRobot;

	protoPlayerFireReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 发射子弹应答
struct protoPlayerFireResp : MSG_HEAD
{
	enum { IDD = SHOOTSELFRESPONE, };
	uint32_t		flag;
	uint32_t		bulletOrder;
	int32_t			x;
	int32_t			y;
	uint32_t		leftCoin;

	protoPlayerFireResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 发射子弹通知(给同桌的基他玩家)
struct protoPlayerFireNotify : MSG_HEAD
{
	enum { IDD = SHOOTRESPONE, };
	
	uint32_t		uin;
	uint32_t		bulletOrder;
	int32_t			x;
	int32_t			y;
	int32_t			isCompute;	// 是否委托计算,机器人=1,其他=0

	protoPlayerFireNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 命中鱼请求
struct protoHitFishReq : MSG_HEAD
{
	enum { IDD = HITFISHREQUEST, };
	enum { PLAYER = 0, ROBOT = 1, }; 
	typedef std::vector<uint32_t>	ID_VEC;

	uint32_t		uin;
	uint32_t		bulletOrder;
	ID_VEC			arFishId;
	uint32_t		fishTypeId;
	ID_VEC			arFishTypeId;
	uint32_t		fishScreenData;
	uint32_t		fishBillData;
	int32_t			isRobot;

	protoHitFishReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 命中鱼应答(整张桌子广播）
struct protoHitFishNotify : MSG_HEAD
{
	enum { IDD = HITFISHRESPONE, };
	struct FishHitData
	{
		uint32_t fishid;
		uint32_t score;

		FishHitData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::vector<FishHitData> FISHDATA_VEC;

	uint32_t		uin;
	uint32_t		score;
	uint32_t		gunId;
	FISHDATA_VEC	vecFishData;

	protoHitFishNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 捕鱼未命中应答
struct protoHitFishResp : MSG_HEAD
{
	enum { IDD = HITFISHMISSRESPONE, };
	uint32_t		flag;

	protoHitFishResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 玩家信息同步(给同桌的其他玩家)
struct protoPlayerDataSyncNotify :  MSG_HEAD
{
	enum { IDD = PLAYERINFOCHANGERESPONE, };

	uint32_t		uin;
	int16_t			flag;
	uint32_t		score;
	uint32_t		bullet;
	uint32_t		gunId;

	protoPlayerDataSyncNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 退出房间请求
struct protoLeaveRoomReq : MSG_HEAD
{
	enum { IDD = OUTGAMEREQUEST, };

	uint32_t		uin;

	protoLeaveRoomReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 退出房间应答
struct protoLeaveRoomResp: MSG_HEAD
{
	enum { IDD = OUTGAMERESPONE, };
	uint32_t		uin;

	protoLeaveRoomResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 账号信息同步（仅发给用户自身）
struct protoAccountSyncNotify : MSG_HEAD
{
	enum { IDD = ACCOUNTRESPONE, };
	enum { COIN =1, BANK_COIN = 2, };

	uint32_t		uin;
	uint32_t		typeId;	// COIN or BANK_COIN
	uint32_t		number;

	protoAccountSyncNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 比赛房结束通知(发给玩家自身）
struct protoLeaveCompetRoomNotify : MSG_HEAD
{
	enum { IDD = OUTCOMPETRESPONE, };

	uint32_t		score;
	uint32_t		ranking;
	uint32_t		timeLeft;
	uint32_t		costNext;

	protoLeaveCompetRoomNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

#if 0
// PVP请求
struct protoPvpReq : MSG_HEAD
{
	enum { IDD = PVPROOMSTATUSREQUEST, };

	uint32_t		uin;

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// PVP应答
struct protoPvpResp : MSG_HEAD
{
	enum { IDD = PVPROOMSTATUSRESPONE, };
	enum STATUS {
		statusNotReady 	= 0,
		statusStart    	= 10,	// 0 vs 0
		statusLost		= 11,	// 0 vs 1 <END>
		statusWin1		= 20,	// 1 vs 0
		statusBalance	= 21,	// 1 vs 1 <END>
		statusWin2		= 30,	// 2 vs 0
		statusWin2_1	= 31,	// 2 vs 1 <END>
		stautsWin3		= 40,	// 3 vs 0 <END>
	};
	uint32_t	status;

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// PVP等待时请求终止
struct protoPvpLeaveReq : MSG_HEAD
{
	enum { IDD = PVPROOMWAITSTOPREQUEST, };
	uint32_t		uin;

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// PVP等待时终止的应答
struct protoPvpLeaveResp : MSG_HEAD
{
	enum { IDD = PVPROOMWAITSTOPRESPONE, };
	enum { FAIL = 0, SUCCEED =1, };
	uint32_t	flag;

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// PVP退出房间通知
struct protoPvpLeaveNotify : MSG_HEAD
{
	enum { IDD = PVPROOMOUTRESPONE, };
	enum { LOST = 0, WIN =1 };

	uint32_t	flag;
	uint32_t	scoreSef;
	uint32_t	scoreOpponent;
	char		opponentName[64];

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};
#endif

// 玩家获得奖励请求
struct protoGetRewardReq : MSG_HEAD
{
	enum { IDD = PVPROOMGETREWARDREQUEST, };

	uint32_t		uin;

	protoGetRewardReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 玩家获得奖励应答
struct protoGetRewardResp : MSG_HEAD
{
	enum { IDD = PVPROOMGETREWARDRESPONE, };
	enum { FAIL = 0, SUCCEED = 1};
	uint32_t		flag;

	protoGetRewardResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
	
};

// 系统广播（走马灯）
struct protoSystemBroadcastNotify : MSG_HEAD
{
	enum  { IDD = SYSTEMBROADCASTRESPONE, }; 
	enum BroadType{
        /// 喇叭消息
        Type_Trumpet = 0,
        /// 兑换炮座广播
        Type_ExchangeBarbette = 1,
        /// 赠送炮座广播
        Type_GiveBarbette = 2,
        /// 大厅、渔场广播
        Type_Hall = 3,
        /// 大厅、渔场、神秘商城广播
        Type_HallAndExchange = 4,
        /// VIP专场广播
        Type_Vip = 5,
		 /// 博饼广播通知
         Type_BoCake = 6,

	};
	uint32_t	type;
	char		szNote[1204];

	protoSystemBroadcastNotify(const char *pszNote = "");
	int32_t Serialize(void *pbuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

// 通用的错误提示
struct protoErrorResp : MSG_HEAD
{
	enum ERROR_CODE
	{
		BASE_CODE = 100,
		ITEM_NOT_EXIST = 101,							//道具不存在
		INSUFFICIENT_MONEY = 102,						//趣玩币不足
		INSUFFICIENT_COIN = 103,						//金币不足
		INSUFFICIENT_TICKET = 104,					//奖券不足
		USER_NOT_EXIST = 105,							//用户不存在
		FRIEND_NAME_NOT_ALLOW_EMPTY = 106,			//好友名称不能为空
		FRIEND_ALREADY_EXISTS = 107,					//好友已经存在
		FRIEND_NOT_EXIST = 108,						//好友不存在
		NOT_ALLOW_ADD_ONESELF = 109,					//不能添加自己为好友
		COIN_ALREADY_GIVE = 110,						//金币已经赠送
		GIVE_TEN_TIMES_ALREADY = 111,					//金币已经赠送10次
		FRIEND_NUM_REACH_THE_LIMIT = 112,				//好友数量达到上限
		WRONG_PASSWORD = 113,							//密码错误
		WRONG_OLD_PASSWORD = 114,						//旧密码错误
		PASSWORD_CANNOT_BE_THE_SAME = 115,			//新旧密码不能相同
		NEW_PASSWORD_LENGTH_ERROR = 116,				//新密码长度错误
		NEW_PASSWORD_FORMAT_ERROR = 117,				//新密码格式错误
		RECHARGE_ID_NOT_EXIST = 118,					//礼包ID不存在
		EXCEED_LIMIT_BUY_NUM = 119,					//超过限制购买次数
		MONTH_CARD_BUY_ONCE_WITHIN_ONE_MONTH = 120,	//月卡在一个月内仅能购买一次
		REWARD_NOT_EXIST = 121,						//VIP礼包不存在
		REWARD_HAS_BEEN_RECEIVED = 122,				//VIP礼包已经被领取
		TASK_NOT_FOUND = 123,							//任务未找到
		LESS_THAN_MIN_GIVE_COIN = 124,				//赠送金币少于最小赠送值
		MORE_THAN_MAX_COIN = 125,						//用户金币超过最大数
		ITEM_MORE_THAN_PKGSIZE = 126,					//道具数量超过背包大小
		VERIFY_CODE_ERROR = 127,						//验证码错误
		BANK_INSUFFICIENT_COIN = 128,					//仓库金币不足
		VERIFY_CODE_FREQUENTLY = 129,					//验证码请求太频繁
		SHARE_CHANNEL_EMPTY = 130,					//分享渠道不能为空
		ITEM_SOLDOUT = 131,							//商品已下架
		VERIFY_CODE_EXHAUST = 132,					//验证码耗尽（今日验证码发送数量已达到最大值）
		MAX_CODE = 133,
		INSUFFICIENT_BULLET = 201,				//子弹不足
		ROOM_LISTREQUEST_ERROR = 202,					//请求房间列表请求包数据错误
		INTOROOMREQUEST_ERROR = 203,					//进入房间请求包数据错误
		UIN_ERROR = 204,								//uin错误
		INSUFFICIENT_TRUMPET = 205,					//喇叭不足
		MOBILE_ALREADY_EXISTS = 206,					//手机号已被绑定
		MAIL_ALREADY_EXISTS = 207,					//邮箱已被绑定
		MOBILE_NUMBER_ILLEGAL = 208,					//手机号非法
		MAIL_ADDRESS_ILLEGAL = 209,					//邮箱地址非法

		INSUFFICIENT_VIP = 210,					//VIP等级不足

		BULLET_ORDER_INVALID = 211,					// 子弹序号不合号
		CURRENCY_INVALID = 212,						// 销售币种非法
		ITEM_CLASS_INVALID = 213,						// 道具类型非法

		NEW_USER_GIFT_NOT_ALLOW = 214,			//新手礼包错误
		
		NOT_ALLOW_PLAY = 215,					//赚太多不让玩游戏（大厅需求）
	};
	enum { IDD = ERROR_RESPONE, };

	int32_t		error_code;

	protoErrorResp(int32_t code = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 用户货币信息请求
struct protoUserCurrencyRequest : MSG_HEAD
{
	enum { IDD = USER_CURRENCY_REQUEST, };

	uint32_t uin;

	protoUserCurrencyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户货币信息应答
struct UserCurrencyRespone : MSG_HEAD
{
	enum { IDD = USER_CURRENCY_RESPONE, };
	uint32_t		uin;
	uint32_t		money;
	uint32_t		coin;
	uint32_t		ticket;
	uint32_t		trumpet;

	UserCurrencyRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoOutCompetNotify : MSG_HEAD
{
	enum { IDD =OUTCOMPETRESPONE, };

	uint32_t	score;
	uint32_t	ranking;
	uint32_t	lefttime;
	uint32_t	cost;

	protoOutCompetNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 房间桌子列表请求
struct protoRoomTableRequest : MSG_HEAD
{
	enum { IDD = ROOM_TABLE_REQUEST, };

	uint32_t uin;
	uint32_t roomId;

	protoRoomTableRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 房间桌子列表应答
struct protoRoomTableRespone : MSG_HEAD
{
	enum { IDD = ROOM_TABLE_RESPONE, };

	struct PlayerInfo
	{
		char nick[32];
		uint32_t coin;
		uint8_t seatId;

		PlayerInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::map<uint16_t, std::vector<PlayerInfo> > ROOMTABLE_MAP;  //桌子ID -> 桌子上玩家信息
	ROOMTABLE_MAP mapRoomTable;			

	protoRoomTableRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 进入指定座位请求
struct protoEnterSpecifySeatRequest : MSG_HEAD
{
	enum { IDD = ENTER_SPECIFY_SEAT_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t tableId;
	uint16_t seatId;

	protoEnterSpecifySeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoEnterSpecifySeatRespone : MSG_HEAD
{
	enum { IDD = ENTER_SPECIFY_SEAT_RESPONE, };

	int32_t result;

	protoEnterSpecifySeatRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 改变座位请求
struct protoChangeSeatRequest : MSG_HEAD
{
	enum { IDD = CHANGE_SEAT_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t tableId;
	uint8_t seatId;

	protoChangeSeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoChangeSeatRespone : MSG_HEAD
{
	enum { IDD = CHANGE_SEAT_RESPONE, };

	int32_t result;
	uint32_t roomId;
	uint32_t tableId;
	uint8_t seatId;

	protoChangeSeatRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 同步场景请求
struct protoSyncSceneRequest : MSG_HEAD
{
	enum { IDD = SYNC_SCENE_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t tableId;

	protoSyncSceneRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSyncSceneRespone : MSG_HEAD
{
	enum { IDD = SYNC_SCENE_RESPONE, };

	typedef std::vector<uint32_t>	FISHID_VEC;
	uint32_t roomId;
	uint32_t sceneId;
	int32_t status;
	uint32_t usetime;
	FISHID_VEC arFishId;

	protoSyncSceneRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRoomStatusRequest : MSG_HEAD
{
	enum { IDD = ROOM_STATUS_REQUEST, };

	uint32_t uin;
	uint32_t roomFlag;

	protoRoomStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRoomStatusRespone : MSG_HEAD
{
	enum { IDD = ROOM_STATUS_RESPONE, };

	enum RoomStatus
	{
		Close = 0,
		Open = 1,
	};
	int32_t status;

	protoRoomStatusRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRoomStatusNewRequest : MSG_HEAD
{
	enum { IDD = ROOM_STATUS_NEW_REQUEST, };

	uint32_t uin;
	uint32_t roomFlag;

	protoRoomStatusNewRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRoomStatusNewRespone : MSG_HEAD
{
	enum { IDD = ROOM_STATUS_NEW_RESPONE, };

	enum RoomStatus
	{
		Close = 0,
		Open = 1,
	};
	int32_t roomFlag;
	int32_t status;
	char nextTime[32];

	protoRoomStatusNewRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoLeaveCompetRespone : MSG_HEAD
{
	enum { IDD = LEAVE_COMPET_RESPONE, };

	uint32_t baseScore;
	uint32_t plusScore;
	uint32_t rank;
	int32_t costType;
	uint32_t costNum;
	uint32_t leftTime;
	uint32_t reverse1;
	char reverse2[128];

	protoLeaveCompetRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoFishGainItemsNotify : MSG_HEAD
{
	enum { IDD = FISH_GAIN_ITEMS_NOTIFY, };
	struct FishHitDataV1
	{
		uint32_t fishId;
		uint32_t itemTypeId;
		uint32_t num;

		FishHitDataV1();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::vector<FishHitDataV1> FISHDATA_VECV1;

	uint32_t		uin;
	FISHDATA_VECV1	vecFishData;

	protoFishGainItemsNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoVipRoomListRequest : MSG_HEAD
{
	enum { IDD = VIP_ROOMLIST_REQUEST, };

	uint32_t uin;

	protoVipRoomListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct VipRoomInfo
{
	uint32_t 	roomId;
	char 		beginTime[20];		// 开始时间: HHMMSS
	char 		endTime[20];		// 结束时间: HHMMSS
	uint32_t 	vip;				// 
	uint32_t 	level;				// 
	uint32_t 	minCoin;			// 进场金币限制
	char 		gunlist[64];		// 枪列表
	char 		roomName[64];		// 房间名称
	char 		url[128];			// 房间图片
	uint32_t	stayTime;			// 停留时间
	uint32_t	costCoin;			// 消耗金币
	uint32_t	costTicket;			// 消耗奖券
	uint32_t	costMoney;			// 消耗钻石
	char		costItems[128];		// 消耗道具
	char 		reserve1[256];		// 预留字段

	VipRoomInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoVipRoomListResponse : MSG_HEAD
{
	enum { IDD = VIP_ROOMLIST_RESPONSE, };

	std::vector<VipRoomInfo> vipRoomList;

	protoVipRoomListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoVipRoomSignUpStatusRequest : MSG_HEAD
{
	enum { IDD = VIPROOM_SIGNUP_STATUS_REQUEST, };

	uint32_t roomId;

	protoVipRoomSignUpStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoVipRoomSignUpStatusResponse : MSG_HEAD
{
	enum { IDD = VIPROOM_SIGNUP_STATUS_RESPONSE, };

	int32_t result;
	uint32_t roomId;

	protoVipRoomSignUpStatusResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*锁定鱼同步*/
struct protoUserTrackFishNotify : MSG_HEAD
{
	enum { IDD = USER_TRACK_FISH_NOTIFY, };

	uint32_t uin;
	uint32_t fishInsId;

	protoUserTrackFishNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoUserTrackFishNotifySync : MSG_HEAD
{
	enum { IDD = USER_TRACK_FISH_NOTIFY_SYNC, };

	uint32_t uin;
	uint32_t fishInsId;

	protoUserTrackFishNotifySync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*锁定技能持续时间通知*/
struct protoUserSkillDurationNotify : MSG_HEAD
{
	enum { IDD = USER_SKILL_DURATION_NOTIFY, };

	uint32_t uin;
	uint32_t itemTypeId;
	uint32_t cd;
	uint32_t duration;

	protoUserSkillDurationNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoUserSkillContinueReq : MSG_HEAD
{
	enum { IDD = USER_SKILL_CONTINUE_REQ, };

	uint32_t uin;
	uint32_t itemTypeId;

	protoUserSkillContinueReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*用户技能暂停后继续通知*/
struct protoUserSkillContinueNotify : MSG_HEAD
{
	enum { IDD = USER_SKILL_CONTINUE_NOTIFY, };

	uint32_t uin;
	uint32_t itemTypeId;
	uint32_t cd;
	uint32_t duration;

	protoUserSkillContinueNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*锁定技能结束通知*/
struct protoUserTrackSkillEndNotify : MSG_HEAD
{
	enum { IDD = USER_TRACKSKILL_END_NOTIFY, };

	uint32_t uin;

	protoUserTrackSkillEndNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*锁定技能暂停通知*/
struct protoUserTrackSkillPauseNotify : MSG_HEAD
{
	enum { IDD = USER_TRACKSKILL_PAUSE_NOTIFY, };

	uint32_t uin;

	protoUserTrackSkillPauseNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*打鱼获得钻石通知*/
struct protoUserGainMoneyNotify : MSG_HEAD
{
	enum { IDD = USER_GAIN_MONEY_NOTIFY, };

	uint32_t uin;
	uint32_t money;
	uint32_t fishInsId;

	protoUserGainMoneyNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/*重置发炮时间通知*/
struct protoResetFireTimeNotify : MSG_HEAD
{
	enum { IDD = RESET_FIRE_TIME_NOTIFY, };

	uint32_t uin;

	protoResetFireTimeNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrUserLeaveRoomNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USERLEAVEROOM_NOTIFY };

	uint32_t uin;
	uint32_t roomId;
	uint32_t level;
	uint64_t exp;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;

	protoSvrUserLeaveRoomNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 激光炮/黑洞准备
struct protoSkillReady : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_READY, };

	int16_t		seatId;
	uint32_t	fishInsId;
	int32_t 	timeLeft;
	uint32_t	bulletScore;
	uint32_t	type;
	
	protoSkillReady();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 激光炮/黑洞已经准备好
struct protoSkillPrepared : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_PREPARED, };

	int16_t		seatId;
	uint32_t	type;
	
	protoSkillPrepared();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 发射激光炮/黑洞请求
struct protoPlayerSkillFireReq : MSG_HEAD
{
	enum { IDD = SHOOTSKILLREQUEST, };
	enum { PLAYER =0, ROBOT = 1, };

	uint32_t		uin;
	uint32_t		bulletOrder;
	int32_t			x;
	int32_t			y;
	int32_t			isRobot;
	uint32_t		type;
	
	protoPlayerSkillFireReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 发射激光炮/黑洞通知(给同桌的基他玩家)
struct protoPlayerSkillFireNotify : MSG_HEAD
{
	enum { IDD = SHOOTSKILLNOTIFY, };
	
	uint32_t		uin;
	uint32_t		bulletOrder;
	int32_t			x;
	int32_t			y;
	int32_t			isCompute;	// 是否委托计算,机器人=1,其他=0
	uint32_t		type;
	
	protoPlayerSkillFireNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 激光炮/黑洞拖动请求
struct protoPlayerSkillDragReq : MSG_HEAD
{
	enum { IDD = DRAGSKILLREQUEST, };
	
	uint32_t		uin;
	int32_t			x;
	int32_t			y;
	uint32_t		type;
	
	protoPlayerSkillDragReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 激光炮/黑洞拖动同步(给同桌的基他玩家)
struct protoPlayerSkillDragNotify : MSG_HEAD
{
	enum { IDD = DRAGSKILLNOTIFY, };
	
	uint32_t		uin;
	int32_t			x;
	int32_t			y;
	int32_t			isCompute;	// 是否委托计算,机器人=1,其他=0
	uint32_t		type;
	
	protoPlayerSkillDragNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 激光炮/黑洞命中鱼请求
struct protoSkillHitFishReq : MSG_HEAD
{
	enum { IDD = SKILLHITFISHREQUEST, };
	enum { PLAYER = 0, ROBOT = 1, }; 
	typedef std::vector<uint32_t>	ID_VEC;

	uint32_t		uin;
	uint32_t		bulletOrder;
	ID_VEC			arFishIdBomb;
	uint32_t		fishTypeId;
	ID_VEC			arFishTypeId;
	uint32_t		fishScreenData;
	uint32_t		fishBillData;
	int32_t			isRobot;
	ID_VEC			arFishIdNormal;
	uint32_t		type;
	
	protoSkillHitFishReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 激光炮/黑洞命中鱼应答(整张桌子广播）
struct protoSkillHitFishNotify : MSG_HEAD
{
	enum { IDD = SKILLHITFISHRESPONE, };
	struct FishHitData
	{
		uint32_t fishid;
		uint32_t score;

		FishHitData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	typedef std::vector<FishHitData> FISHDATA_VEC;

	uint32_t		uin;
	uint32_t		score;
	uint32_t		gunId;
	FISHDATA_VEC	vecFishData;
	uint32_t		type;
	uint8_t			isTotalGet;
	
	protoSkillHitFishNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴积分同步
struct protoBerserkScoreNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_SCORE_NOTIFY, };
	
	int64_t			score;
	int64_t			baseScore;
		
	protoBerserkScoreNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 使用狂暴技能请求
struct protoUseBerserkTrackSkillReq : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_SKILL_REQ, };
	
	uint32_t	uin;
		
	protoUseBerserkTrackSkillReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 使用狂暴技能请求回复
struct protoUseBerserkTrackSkillResp : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_SKILL_RESP, };
	
	int32_t	result;
	int64_t		score;
	
	protoUseBerserkTrackSkillResp();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 获得狂暴技能通知
struct protoGetBerserkSkillNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_SKILL_NOTIFY, };
	
	uint32_t	uin;
	uint32_t	cd;
	uint32_t	duration;
	uint32_t	fishInsId;
	uint64_t 	timeLeft;
	
	protoGetBerserkSkillNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能持续/冷却时间同步
struct protoBerserkSkillDurationNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_DURATION_NOTIFY, };
	
	uint32_t	uin;
	uint32_t	cd;
	uint32_t	duration;
	uint64_t 	timeLeft;
	
	protoBerserkSkillDurationNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能锁定鱼通知
struct protoUserBerserkTrackFishNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_TRACK_NOTIFY, };
	
	uint32_t	uin;
	uint32_t	mainFishInsId;
	uint32_t	subFishInsId;
	
	protoUserBerserkTrackFishNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能锁定鱼同步
struct protoUserBerserkTrackFishNotifySync : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_TRACK_SYNC, };
	
	uint32_t	uin;
	uint32_t	mainFishInsId;
	uint32_t	subFishInsId;
	
	protoUserBerserkTrackFishNotifySync();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能结束同步
struct protoUserBerserkTrackSkillEndNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_END_NOTIFY, };
	
	uint32_t	uin;
	
	protoUserBerserkTrackSkillEndNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能暂停通知
struct protoUserBerserkTrackSkillPauseReq : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_PAUSE_NOTIFY, };
	
	uint32_t	uin;
	
	protoUserBerserkTrackSkillPauseReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 狂暴技能继续通知
struct protoUserBerserkTrackSkillContinueReq : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_CONTINUE_REQ, };
	
	uint32_t	uin;
	uint32_t	mainFishInsId;
	uint32_t	subFishInsId;
	
	protoUserBerserkTrackSkillContinueReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

//狂暴技能继续同步
struct protoBerserkSkillContinueNotify : MSG_HEAD
{
	enum { IDD = PROTO_BERSERK_CONTINUE_NOTIFY, };
	
	uint32_t	uin;
	uint32_t	cd;
	uint32_t	duration;
	
	protoBerserkSkillContinueNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 快捷聊天请求
struct protoUserQuickChatReq : MSG_HEAD
{
	enum { IDD = PROTO_QUICK_CHAT_REQ, };
	
	uint32_t	chantId;
	char		content[128];
	
	protoUserQuickChatReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

// 快捷聊天同步
struct protoUserQuickChatSync : MSG_HEAD
{
	enum { IDD = PROTO_QUICK_CHAT_SYNC, };
	
	uint32_t	uin;
	uint32_t	chantId;
	uint32_t	type;
	char		content[128];
	
	protoUserQuickChatSync();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct ExtendScene
{
	uint32_t sceneId;
	uint32_t shoalId;
	uint32_t timePass;
	std::vector<uint32_t> fishData;

	ExtendScene();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoExtendSceneStateNotify : MSG_HEAD
{
	enum { IDD = PROTO_EXTEND_SCENE_NOTIFY, };
	std::vector<ExtendScene> vecExtendScene;

	protoExtendSceneStateNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSkillStageChangeReq : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_STAGE_CHANGE_REQ, };
	
	uint32_t	uin;
	uint32_t	skillType;
		
	protoSkillStageChangeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct protoSkillStageChangeNotify : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_STAGE_CHANGE_NOTIFY, };
	
	uint32_t	uin;
	uint32_t	skillType;
		
	protoSkillStageChangeNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct SkillBulletPos
{
	uint32_t uin;
	uint32_t skillType;
	char xy[64];
	char angle[64];
	int32_t stage;

	SkillBulletPos();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoSkillBulletPosReq : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_BULLET_POS_REQ, };
	
	std::vector<SkillBulletPos> vecPos;
		
	protoSkillBulletPosReq();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct protoSkillBulletPosNotify : MSG_HEAD
{
	enum { IDD = PROTO_SKILL_BULLET_POS_NOTIFY, };
	
	std::vector<SkillBulletPos> vecPos;
		
	protoSkillBulletPosNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct protoExtendEventNotify : MSG_HEAD
{
	enum { IDD = PROTO_EXTEND_EVENT_NOTIFY, };
	
	uint32_t uin;
	uint32_t eventType;
	uint32_t countdown;
	uint32_t duration;
	uint32_t doCount;
	
	std::vector<uint32_t> vecType;
	std::vector<uint32_t> vecScore;
		
	protoExtendEventNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct protoExtendEventDo : MSG_HEAD
{
	enum { IDD = PROTO_EXTEND_EVENT_DO, };
	
	uint32_t eventType;
	uint32_t index;
		
	protoExtendEventDo();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct protoExtendEventDoResult : MSG_HEAD
{
	enum { IDD = PROTO_EXTEND_EVENT_RESULT, };
	
	uint32_t uin;
	uint32_t eventType;
	std::vector<uint32_t> vecIndex;
	uint32_t getScore;
	uint32_t leftCount;
	
	std::vector<CurrencyInfo> giftInfo;
	std::vector<uint32_t> vecType;
		
	protoExtendEventDoResult();
	int32_t Serialize(void *pBuf, size_t cbBuf); 
	int32_t Deserialize(void *pBuf, size_t cbBuf); 
	uint32_t getProtoLen(void); 
		
};

struct SceneWarn
{
	uint32_t sceneId;
	uint32_t showTime;
	uint32_t fishType;

	SceneWarn();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoSceneWarnNotify : MSG_HEAD
{
	enum { IDD = PROTO_SCENE_WARN_NOTIFY, };
	std::vector<SceneWarn> vecSceneWarn;

	protoSceneWarnNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};
#endif // _QW_GAMESVR_PROTO_ROOH_H_

