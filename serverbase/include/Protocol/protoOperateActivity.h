/*
* =====================================================================================
*
*       Filename:  protoOperateActivity.h
*
*    Description:  运营活动协议
*
*        Version:  1.0
*        Created:  2016年7月19日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_OPERATE_ACTIVITY_H_
#define _PROTO_OPERATE_ACTIVITY_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct OperateActivityData
{
	uint32_t acti_id;
	uint32_t data_id;
	char note[128];
	uint32_t type;
	uint32_t status;
	char complete_value[128];
	char target_value[128];
	uint32_t reward_id;

	OperateActivityData();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct OperateActivityTitle
{
	uint32_t acti_id;
	char title[128];
	uint32_t begin_time;
	uint32_t end_time;

	OperateActivityTitle();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct ActivityInfo
{
	uint32_t type;
	uint32_t type_id;
	char title[128];
	uint32_t priority;
	
	ActivityInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

/////////////////////////////////////////////////////////////////////

struct protoQueryOperateActivityRequest : MSG_HEAD
{
	enum { IDD = QUERY_OPERATE_ACTIVITY_REQUEST };

	uint32_t uin;

	protoQueryOperateActivityRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoQueryOperateActivityRespone : MSG_HEAD
{
	enum { IDD = QUERY_OPERATE_ACTIVITY_RESPONE };

	vector<OperateActivityTitle> title_list;
	vector<OperateActivityData> data_list;

	protoQueryOperateActivityRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/////////////////////////////////////////////////////////////////

struct protoGetOperateActivityRequest : MSG_HEAD
{
	enum { IDD = GET_OPERATE_ACTIVITY_REQUEST };

	uint32_t uin;
	uint32_t acti_id;
	uint32_t data_id;

	protoGetOperateActivityRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoGetOperateActivityRespone : MSG_HEAD
{
	enum { IDD = GET_OPERATE_ACTIVITY_RESPONE };

	uint32_t result;
	uint32_t acti_id;
	uint32_t data_id;

	protoGetOperateActivityRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////////

struct protoRuleOperateActivityRequest : MSG_HEAD
{
	enum { IDD = RULE_OPERATE_ACTIVITY_REQUEST };

	uint32_t uin;
	uint32_t acti_id;

	protoRuleOperateActivityRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoRuleOperateActivityRespone : MSG_HEAD
{
	enum { IDD = RULE_OPERATE_ACTIVITY_RESPONE };

	uint32_t acti_id;
	char text[1024];

	protoRuleOperateActivityRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//////////////////////////////////////////////////////////////////////////

struct protoActivityListRequest : MSG_HEAD
{
	enum { IDD = ACTIVITY_LIST_REQUEST };

	uint32_t uin;

	protoActivityListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoActivityListRespone : MSG_HEAD
{
	enum { IDD = ACTIVITY_LIST_RESPONE };

	vector<ActivityInfo> acti_list;

	protoActivityListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoDataOperateActivityRespone : MSG_HEAD
{
	enum { IDD = DATA_OPERATE_ACTIVITY_RESPONE };

	uint32_t acti_id;
	uint32_t data_id;
	uint32_t status;

	protoDataOperateActivityRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoActivityDataNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ACTIVITY_DATA_NOTIFY };

	enum eType
	{
		Recharge = 1,			// 充值
		Fish = 2,				// 捕鱼
		CostCoin = 3,			// 消耗金币
		Login = 4,              // 登入游戏
		SharpTime = 5,          // 整点礼包
		Signin = 6,             // 签到
		FishCoin = 7,           // 捕鱼获得金币
        OnlineTime = 8,         // 在线时长
        CompetGain = 9,			// 捕鱼达人获得分数
        ChallgengeGain = 10,	// 竞技赛获得分数
		RedPoint = 11,			// 红点
		BetFish = 12,			// 竞猜鱼
        RedPackage = 13,		// 红包鱼
		GrandPrix = 14,			// 大奖赛
		WeChatShare = 15,		// 微信分享
		RealName = 16,			// 实名认证通知
		GainCost = 17,			// 捕鱼消耗和获得金币数据
		RoomChange = 18,	    // 房间切换
		FishJackpot = 19,	    // 奖池功能捕获鱼种
		LogOut = 20,			// 退出游戏
		Pirateship = 21,		// 海盗船(包括鱼类型:14=卡片船, 15=宝船)
		RoomCoin = 22,			// 玩家在捕鱼房间内当前金币	
	};

	struct ActivityData
	{
		uint32_t source;
		uint64_t num;

		ActivityData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};

	uint32_t roomId;
	uint32_t notifyType;
	uint32_t uin;
	vector<ActivityData> vecActivityData;
	char nick[64];
	uint32_t vip;

	protoActivityDataNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoNewbeeLoginInfoRequest : MSG_HEAD
{
	enum { IDD = NEWBEE_LOGIN_INFO_REQUEST };

	uint32_t uin;

	protoNewbeeLoginInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoNewbeeLoginInfoResponse : MSG_HEAD
{
	enum { IDD = NEWBEE_LOGIN_INFO_RESPONSE };

	struct NewbeeLoginInfo
	{
		uint32_t day;
		uint32_t rewardId1;
		uint32_t status1;
		uint32_t rewardId2;
		uint32_t status2;
		uint32_t rechargeNum;

		NewbeeLoginInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};

	char url[256];
	uint32_t leftTime;
	uint32_t currentDay;
	uint32_t currentDayRecharge;
	vector<NewbeeLoginInfo> vecDailyInfo;

	protoNewbeeLoginInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoReceiveNewbeeRewardRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_NEWBEE_REWARD_REQUEST };

	uint32_t uin;
	uint32_t type;

	protoReceiveNewbeeRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoReceiveNewbeeRewardResponse : MSG_HEAD
{
	enum { IDD = RECEIVE_NEWBEE_REWARD_RESPONSE };

	int32_t result;

	protoReceiveNewbeeRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSevenDaysRechargeInfoRequest : MSG_HEAD
{
	enum { IDD = SEVENDAYS_RECHARGE_INFO_REQUEST };

	uint32_t uin;

	protoSevenDaysRechargeInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSevenDaysRechargeInfoResponse : MSG_HEAD
{
	enum { IDD = SEVENDAYS_RECHARGE_INFO_RESPONSE };

	struct SevenDaysRechargeInfo
	{
		enum eStauts
		{
			Init = 0,			// 初始
			Bougth = 1,			// 已购买
			CanBuy = 2,			// 可购买
		};

		uint32_t day;
		uint32_t status;
		uint32_t costType;
		uint32_t cost;
		char url[128];
		uint32_t rechargeId;

		SevenDaysRechargeInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);

		bool operator < (const SevenDaysRechargeInfo& info) const
		{
			return day < info.day;
		}
	};

	char url[128];
	uint32_t leftTime;
	vector<SevenDaysRechargeInfo> vecDailyInfo;

	protoSevenDaysRechargeInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoLotteryDrawRequest : MSG_HEAD
{
	enum { IDD = LOTTERY_DRAW_REQUEST };

	uint32_t uin;
	uint32_t actiId;

	protoLotteryDrawRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoLotteryDrawResponse : MSG_HEAD
{
	enum { IDD = LOTTERY_DRAW_RESPONSE };

	int32_t result;
	uint32_t actiId;
	uint32_t rewardId;
	uint32_t nextCostType;
	uint32_t nextCost;
	uint32_t rechargeId;
	uint32_t rewardIdEx;
	char reserve2[256];

	protoLotteryDrawResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoFortuneCatInfoRequest : MSG_HEAD
{
	enum { IDD = FORTUNECAT_INFO_REQUEST };

	uint32_t uin;

	protoFortuneCatInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoFortuneCatInfoResponse : MSG_HEAD
{
	enum { IDD = FORTUNECAT_INFO_RESPONSE };

	uint32_t costType;
	uint32_t cost;
	uint32_t leftTimes;
	uint32_t rechargeId;
	char url[128];
	vector<uint32_t> vecRewardInfo;

	protoFortuneCatInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



//add by meilinfeng for treasure chest lottery
struct protoQueryTreasureChestTitleRequest : MSG_HEAD
{
	enum { IDD = QUERY_TREASURE_CHEST_TITLE_REQUEST };

	uint32_t uin;

	protoQueryTreasureChestTitleRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct TreasureChestTitleInfo
{
    uint32_t lottery_id;
    uint32_t is_default;
    char title[128];

    TreasureChestTitleInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoQueryTreasureChestTitleRespone : MSG_HEAD
{
	enum { IDD = QUERY_TREASURE_CHEST_TITLE_RESPONE };

	vector<TreasureChestTitleInfo> title_list;

    protoQueryTreasureChestTitleRespone();
    int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

};




struct protoQueryTreasureChestInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_TREASURE_CHEST_INFO_REQUEST };

    uint32_t uin;
    uint32_t lottery_id;

    protoQueryTreasureChestInfoRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};



struct TreasureChestCostInfo
{
    uint32_t costType;
    uint32_t cost;
    
    TreasureChestCostInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};



struct protoQueryTreasureChestInfoResponse : MSG_HEAD
{
    enum { IDD = QUERY_TREASURE_CHEST_INFO_RESPONSE };

    uint32_t lottery_id;
    uint32_t leftTimes;
    uint32_t limitVipMin;
    uint32_t limitLevelMin;
    uint32_t limitTotalRechargeMin;

    vector<uint32_t> vecRewardInfo;
    vector<TreasureChestCostInfo> constInfo;

    protoQueryTreasureChestInfoResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);


};


struct protoTreasureChestLotteryRequest : MSG_HEAD
{
	enum { IDD = TREASURE_CHEST_LOTTERY_REQUEST };

    uint32_t uin;
    uint32_t lottery_id;

    protoTreasureChestLotteryRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};




struct protoTreasureChestLotteryResponse : MSG_HEAD
{
    enum { IDD = TREASURE_CHEST_LOTTERY_RESPONSE };

    uint32_t rewardid;
    uint32_t error_code;

    protoTreasureChestLotteryResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);


};







struct protoSigninInfoRequest : MSG_HEAD
{
	enum { IDD = SIGNIN_INFO_REQUEST };

	uint32_t uin;

	protoSigninInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSigninInfoResponse : MSG_HEAD
{
	enum { IDD = SIGNIN_INFO_RESPONSE };

	struct SigninExtraRewardInfo
	{
		uint32_t day;
		uint32_t rewardId;
		uint32_t status;

		SigninExtraRewardInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};

	enum eStatus
	{
		CanReceive = 0,			// 可领取
		Received = 1,			// 已领取
	};

	uint32_t status;
	uint32_t totalDays;
	char url[128];
	vector<uint32_t> vecRewardInfo;
	vector<SigninExtraRewardInfo> vecExtraRewardInfo;

	protoSigninInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoWinLotteryInfoRequest : MSG_HEAD
{
	enum { IDD = WINLOTTERY_INFO_REQUEST };

	uint32_t uin;
	uint32_t actiId;

	protoWinLotteryInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoWinLotteryInfoResponse : MSG_HEAD
{
	enum { IDD = WINLOTTERY_INFO_RESPONSE };

	vector<string> vecWinLotteryInfo;

	protoWinLotteryInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 消息订阅
struct protoSvrSubscribeRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SUBSCRIBE_REQUEST };

	vector<uint32_t> vecNotifyType;

	protoSvrSubscribeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrSubscribeResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SUBSCRIBE_RESPONSE };

	int32_t result;
	vector<uint32_t> vecNotifyType;

	protoSvrSubscribeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoNewbieSigninInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_NEWBIE_SIGNIN_INFO_REQUEST };

	uint32_t uin;

	protoNewbieSigninInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoNewbieSigninInfoResponse : MSG_HEAD
{
	enum { IDD = PROTO_NEWBIE_SIGNIN_INFO_RESPONSE };

	enum eStatus
	{
		CanReceive = 0,			// 可领取
		Received = 1,			// 已领取
		CanNotReceive = 2,		// 不可领取
		Finish = 3,				// 已结束，开启常规签到
	};

	uint32_t status;
	vector<protoSigninInfoResponse::SigninExtraRewardInfo> vecRewardInfo;

	protoNewbieSigninInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoNewbieSigninRequest : MSG_HEAD
{
	enum { IDD = PROTO_NEWBIE_SIGNIN_REQUEST };

	uint32_t uin;

	protoNewbieSigninRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoNewbieSigninResponse : MSG_HEAD
{
	enum { IDD = PROTO_NEWBIE_SIGNIN_RESPONSE };

	int32_t result;
	uint32_t rewardId;

	protoNewbieSigninResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*捕鱼奖池活动*/
struct protoJackpotInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_JACKPOT_INFO_REQUEST };

	uint32_t uin;
	uint32_t roomId;

	protoJackpotInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoJackpotInfoResponse : MSG_HEAD
{
	enum { IDD = PROTO_JACKPOT_INFO_RESPONSE };

	struct FishJackpotInfo
	{
		uint32_t fishId;
		uint32_t num;
		uint32_t coin;

		FishJackpotInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};

	char beginTime[32];
	char endTime[32];
	char openTime[32];
	char closeTime[32];
	uint32_t todayGain;
	uint32_t todayReceived;
	uint32_t todayUnReceived;
	vector<FishJackpotInfo> vecFishJackpotInfo;

	protoJackpotInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoReceiveJackpotRequest : MSG_HEAD
{
	enum { IDD = PROTO_RECEIVE_JACKPOT_REQUEST };

	uint32_t uin;
	uint32_t roomId;

	protoReceiveJackpotRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoReceiveJackpotResponse : MSG_HEAD
{
	enum { IDD = PROTO_RECEIVE_JACKPOT_RESPONSE };

	int32_t result;
	uint32_t todayGain;
	uint32_t todayReceived;
	uint32_t todayUnReceived;

	protoReceiveJackpotResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoTotalJackpotRequest : MSG_HEAD
{
	enum { IDD = PROTO_TOTAL_JACKPOT_REQUEST };

	uint32_t uin;
	uint32_t roomId;

	protoTotalJackpotRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoTotalJackpotResponse : MSG_HEAD
{
	enum { IDD = PROTO_TOTAL_JACKPOT_RESPONSE };

	uint32_t roomId;
	int32_t status;
	uint32_t totalCoin;

	protoTotalJackpotResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 海盗船打爆卡片鱼桌面同步
struct protoPirateshipCardFishSync : MSG_HEAD
{
    enum { IDD = PIRATESHIP_CARDFISH_SYNC, };

	uint32_t uin;
	uint32_t fishId;		// 鱼类型
	uint32_t instFishId;	// 实例鱼id
	uint32_t cardtype;		// 花色[1=黑桃,2=红心,3=梅花,4=方块]
	uint32_t cardnum;		// 数量

    protoPirateshipCardFishSync();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};


// 海盗船宝藏鱼领奖桌面同步
struct protoPirateshipTreasureFishSync : MSG_HEAD
{
    enum { IDD = PIRATESHIP_TREASUREFISH_SYNC, };

	uint32_t uin;
	uint32_t fishId;		// 鱼类型
	uint32_t instFishId;	// 实例鱼id
	uint32_t coin;			// 最终获取金币

    protoPirateshipTreasureFishSync();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};



#endif //_PROTO_OPERATE_ACTIVITY_H_

