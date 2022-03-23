/*
* =====================================================================================
*
*       Filename:  protoAccount.h
*
*    Description:  用户信息相关协议
*
*        Version:  1.0
*        Created:  2015年10月15日 16时08分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_ACCOUNTPROTOCOL_H_
#define _GFX_PROXY_ACCOUNTPROTOCOL_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//修改账号信息请求
struct protoUpdateAcctInfoRequest : MSG_HEAD
{
	enum { IDD = UPDATEACCTINFOREQUEST, };
	uint32_t	uin;
	uint32_t	type;
	char		data[128];
	char		value[128];

	protoUpdateAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//修改账号信息应答
struct protoUpdateAcctInfoRespone : MSG_HEAD
{
	enum { IDD = UPDATEACCTINFORESPONE, };
	uint32_t	flag;
	uint32_t	type;

	protoUpdateAcctInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取头像名称请求
struct protoGetPlayerPicRequest : MSG_HEAD
{
	enum { IDD = GETPLAYERPICREQUEST, };
	uint32_t	type;
	char		data[128];

	protoGetPlayerPicRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取头像名称应答
struct protoGetPlayerPicRespone : MSG_HEAD
{
	enum { IDD = GETPLAYERPICRESPONE, };
	enum FlagType
	{
		GetPictureFailure = 0,					// 获取头像失败
		Success = 1,							// 成功
		GetPicreqTypeError = 2,					// 请求包中type值错误
		PicVerify = 3							// 新头像审核中
	};

	uint32_t	flag;
	char		data[128];

	protoGetPlayerPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取昵称是否修改请求
struct protoGetNickIfUpdateRequest : MSG_HEAD
{
	enum { IDD = GETNICKIFUPDATEREQUEST, };
	uint32_t	uin;

	protoGetNickIfUpdateRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取昵称是否修改应答
struct protoGetNickIfUpdateRespone : MSG_HEAD
{
	enum { IDD = GETNICKIFUPDATERESPONE, };
	uint32_t	number;

	protoGetNickIfUpdateRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//AccountRespone
struct protoAccountRespone : MSG_HEAD
{
	enum { IDD = ACCOUNTRESPONE, };
	uint32_t	uin;
	uint32_t	type;
	uint32_t	number;

	protoAccountRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//语言版本请求
struct protoLanguageVersionRequest : MSG_HEAD
{
	enum { IDD = LANGUAGE_VERSION_REQUEST, };
	uint32_t uin;
	int32_t language;
	int32_t reserve1;
	int32_t reserve2;
	int32_t reserve3;
	int32_t reserve4;

	protoLanguageVersionRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoFindPasswdVerifyCodeRequest: MSG_HEAD
{
	enum { IDD = FINDPASSWD_VERIFYCODE_NEW_REQUEST, };
	char msisdn[32];
	char channel[32];
	uint32_t source;

	protoFindPasswdVerifyCodeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoFindPasswdVerifyCodeRespone: MSG_HEAD
{
	enum { IDD = FINDPASSWD_VERIFYCODE_NEW_RESPONE, };
	int32_t result;

	protoFindPasswdVerifyCodeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};



struct protoResetPasswdRequest: MSG_HEAD
{
	enum { IDD = RESET_PASSWD_NEW_REQUEST, };
	char msisdn[16];
	char channel[32];
	char verify_code[10];
	char new_passwd[33];
	uint32_t source;

	protoResetPasswdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoResetPasswdRespone: MSG_HEAD
{
	enum { IDD = RESET_PASSWD_NEW_RESPONE, };
	int32_t result;

	protoResetPasswdRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


//玩家昵称请求
struct protoUserNickRequest : MSG_HEAD
{
	enum { IDD = USER_NICK_REQUEST, };
	uint32_t	uin;

	protoUserNickRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//玩家昵称应答
struct protoUserNickResponse : MSG_HEAD
{
	enum { IDD = USER_NICK_RESPONSE, };
	int32_t	result;
	char nick[128];

	protoUserNickResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币请求
struct protoGiveCoinRequest : MSG_HEAD
{
	enum { IDD = GIVE_COIN_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t id;

	protoGiveCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币应答
struct protoGiveCoinResponse: MSG_HEAD
{
	enum { IDD = GIVE_COIN_RESPONSE, };
	int32_t	result;

	protoGiveCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//头像同步
struct protoUserPicSync : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PIC_SYNC, };
	uint32_t uin;
	char pic[256];

	protoUserPicSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//赠送金币请求
struct protoPresentCoinRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	char passwd[64];
	uint32_t coin;

	protoPresentCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币应答
struct protoPresentCoinResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RESPONSE, };
	int32_t	result;
	uint32_t value;

	protoPresentCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送条件请求
struct protoPresentCondRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_COND_REQUEST, };
	uint32_t type;

	protoPresentCondRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送条件应答
struct protoPresentCondResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_COND_RESPONSE, };
	uint32_t vip;
	uint32_t presentLimitEveryTime;
	uint32_t type;
	uint32_t reverse2;
	uint32_t level;
	uint32_t totalRecharge;

	protoPresentCondResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct PresentRecordInfo
{
	uint32_t uin;
	char nick[64];
	uint32_t time;
	int32_t num;

	PresentRecordInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//赠送金币记录请求
struct protoPresentRecordRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RECORD_REQUEST, };
	uint32_t uin;
	uint32_t type;

	enum eType
	{
		ReceiveCoin = 0,			// 收到的金币
		SendOutCoin = 1,			// 送出的金币
		ReceiveCrystal = 2,			// 收到的水晶
		SendOutCrystal = 3,			// 送出的水晶
		AllCrystal = 4,				// 全部水晶
		ReceiveGun = 5,				// 收到的炮座
		SendOutGun = 6,				// 送出的炮座
	};

	protoPresentRecordRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币记录应答
struct protoPresentRecordResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RECORD_RESPONSE, };

	uint32_t type;
	std::vector<PresentRecordInfo> infoList;

	protoPresentRecordResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//修改账号信息请求(hallsvr)
struct protoHallUpdateAcctInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_UPDATE_ACCT_INFO_REQUEST, };
	uint32_t	uin;
	uint32_t	type;
	char		data[512];
	char		value[512];

	enum OP_TYPE
	{
		UPDATE_NICK = 1,						// 修改昵称
		UPDATE_PASSWD = 2, 						// 修改密码
		BIND_MSISDN = 3,						// 绑定手机号
		BIND_EMAIL = 4,							// 绑定邮箱
		SET_FACE = 5,							// 设置头像名称
		GET_MSISDN_VERIFYCODE = 6,				// 获取手机验证码
		GET_EMAIL_VERIFYCODE = 7,				// 获取邮箱验证码
		UPDATE_NICK_NEW = 8,					// 修改昵称(新) 扣金币
		UPDATE_SIGNATURE = 9,					// 修改签名
	};

	protoHallUpdateAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//修改账号信息应答(hallsvr)
struct protoHallUpdateAcctInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_UPDATE_ACCT_INFO_RESPONSE, };
	int32_t	result;
	uint32_t type;
	char data[512];
	char value[512];

	protoHallUpdateAcctInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询账号信息请求(hallsvr)
struct protoQueryAcctInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_ACCT_INFO_REQUEST, };
	uint32_t	uin;
	uint32_t	type;

	enum eType
	{
		Signature = 1,					// 签名
		Nick = 2,						// 昵称
		ClanMemberNick = 3,				// 家族成员昵称
	};

	protoQueryAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询账号信息应答(hallsvr)
struct protoQueryAcctInfoResponse : MSG_HEAD
{
	enum { IDD = QUERY_ACCT_INFO_RESPONSE, };
	uint32_t type;
	char data[256];

	protoQueryAcctInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币请求(hallsvr->gamesvr)
struct protoSvrPresentCoinRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_COIN_REQ, };
	uint32_t uin;
	uint32_t targetUin;
	char passwd[64];
	uint32_t coin;
	uint32_t tax;

	protoSvrPresentCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币应答(gamesvr->hallsvr)
struct protoSvrPresentCoinResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_COIN_RESP, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t coin;
	int32_t	result;
	uint32_t tax;

	protoSvrPresentCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct PresentInfo
{
	uint32_t id;
	uint32_t num;

	PresentInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//赠送请求
struct protoPresentRequest : MSG_HEAD
{
	enum { IDD = PRESENT_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	char passwd[64];

	enum eType
	{
		Coin = 1,			// 金币
		Crystal = 2,		// 水晶
		Gun = 3,
	};

	protoPresentRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送应答
struct protoPresentResponse : MSG_HEAD
{
	enum { IDD = PRESENT_RESPONSE, };
	int32_t	result;

	protoPresentResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币请求(hallsvr->gamesvr)
struct protoSvrPresentRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_REQ, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	char passwd[64];
	uint32_t tax;

	protoSvrPresentRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币应答(gamesvr->hallsvr)
struct protoSvrPresentResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_RESP, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	uint32_t tax;
	int32_t	result;

	protoSvrPresentResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送增加道具(hallsvr->gamesvr)
struct protoSvrAddItemForPresentNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_ADD_ITEM_PRESENT_NOTIFY, };
	uint32_t uin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;

	protoSvrAddItemForPresentNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct PresentRecordInfoV1
{
	uint32_t uin;
	char nick[64];
	uint32_t time;
	int32_t num;
	uint32_t itemTypeId;
	char reserve1[64];

	PresentRecordInfoV1();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//赠送金币记录请求
struct protoPresentRecordRequestV1 : MSG_HEAD
{
	enum { IDD = PRESENT_RECORD_REQUEST_V1, };
	uint32_t uin;
	uint32_t type;

	enum eType
	{
		ReceiveCoin = 0,			// 收到的金币
		SendOutCoin = 1,			// 送出的金币
		ReceiveCrystal = 2,			// 收到的水晶
		SendOutCrystal = 3,			// 送出的水晶
		AllCrystal = 4,				// 全部水晶
		ReceiveGun = 5,				// 收到的炮座
		SendOutGun = 6,				// 送出的炮座
	};

	protoPresentRecordRequestV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送金币记录应答
struct protoPresentRecordResponseV1 : MSG_HEAD
{
	enum { IDD = PRESENT_RECORD_RESPONSE_V1, };

	uint32_t type;
	std::vector<PresentRecordInfoV1> infoList;

	protoPresentRecordResponseV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送炮座积分条件请求
struct protoPresentGunPointRequest : MSG_HEAD
{
	enum { IDD = PRESENT_GUN_POINT_REQUEST, };
	uint32_t uin;
	
	protoPresentGunPointRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送炮座积分条件应答
struct protoPresentGunPointResponse : MSG_HEAD
{
	enum { IDD = PRESENT_GUN_POINT_RESPONSE, };

	uint32_t value;
	uint32_t point;
	uint32_t recharge;

	protoPresentGunPointResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//用户当前可用积分查询
struct protoAccountPointRequest : MSG_HEAD
{
	enum { IDD = HALL_ACCOUNT_POINT_REQUEST, };
	uint32_t uin;

	protoAccountPointRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 用户当前可用积分查询应答
struct protoAccountPointResponse : MSG_HEAD
{
	enum { IDD = HALL_ACCOUNT_POINT_RESPONSE, };
	uint32_t canUsePointToday;
	uint32_t userPoint;
	
	protoAccountPointResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct MonthCardRewardInfo
{
	uint32_t type;
	uint32_t num;

	enum eType
	{
#if 0
		Coin = 0,					// 金币	
		Item = 1,					// 道具
		Probability = 2,			// 概率
		SignIn = 3,					// 签到加成
		Gun	= 4,					// 炮座
#endif
        
        MONEY = 1,
        COIN = 2,
        TICKET = 3
	};

	MonthCardRewardInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//用户月卡信息查询请求
struct protoUserMonthCardRequest : MSG_HEAD
{
	enum { IDD = USER_MONTH_CARD_REQUEST, };
	uint32_t uin;

	protoUserMonthCardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 用户月卡信息查询应答
struct protoUserMonthCardResponse : MSG_HEAD
{
	enum { IDD = USER_MONTH_CARD_RESPONSE, };
	int32_t leftDay;
	int32_t isReceive;
	std::vector<MonthCardRewardInfo> infoList;

	protoUserMonthCardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//领取月卡奖励请求
struct protoReceiveMonthCardRewardRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_MONTH_CARD_REWARD_REQUEST, };
	uint32_t uin;

	protoReceiveMonthCardRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//领取月卡奖励应答
struct protoReceiveMonthCardRewardResponse : MSG_HEAD
{
	enum { IDD = RECEIVE_MONTH_CARD_REWARD_RESPONSE, };
	int32_t result;

	protoReceiveMonthCardRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//签到倍数通知
struct protoSvrSigninBonusNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_SIGNIN_BONUS_NOTIFY, };

	uint32_t uin;
	uint32_t bonus;

	protoSvrSigninBonusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送校验请求
struct protoPresentCheckRequest : MSG_HEAD
{
	enum { IDD = PRESENT_CHECK_REQUEST, };
	uint32_t uin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;

	protoPresentCheckRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//赠送校验应答
struct protoPresentCheckResponse : MSG_HEAD
{
	enum { IDD = PRESENT_CHECK_RESPONSE, };
	uint32_t result;
	uint32_t value;

	protoPresentCheckResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户赠送信息请求
struct protoUserPresentInfoRequest : MSG_HEAD
{
	enum { IDD = USER_PRESENT_INFO_REQUEST, };
	uint32_t uin;

	protoUserPresentInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户赠送信息应答
struct protoUserPresentInfoResponse : MSG_HEAD
{
	enum { IDD = USER_PRESENT_INFO_RESPONSE, };
	uint32_t leftTotalTimes;
	uint32_t leftTodayTimes;
	uint32_t presentDailyMaxTimes;
	uint32_t presentSingleMaxPoint;
	uint32_t curLevelMaxPoint;
	uint32_t costPoint;
	uint32_t leftPresentPriceToday;			// 今日剩余赠送价值
	uint32_t leftPointToday;				// 今日剩余可用积分

	protoUserPresentInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 获取上传自定义头像token请求
struct protoReqGetUploadToken : MSG_HEAD
{
    enum { IDD = PROTO_REQ_GETUPLOADTOKEN, };

    uint32_t uin;

    protoReqGetUploadToken();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取上传自定义头像token请求应答
struct protoRspGetUploadToken : MSG_HEAD
{
    enum { IDD = PROTO_RSP_GETUPLOADTOKEN, };

    uint32_t result;
    char token[16];
    char desc[64];

    protoRspGetUploadToken();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 上传阿里收集的用户认证模块数据
struct protoClientAliAuthReq : MSG_HEAD
{
	enum { IDD = PROTO_CLI_ALIAUTH_INFO, };

	uint32_t uin;
	char szRawData[64];
	char szSign[1024];

	protoClientAliAuthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 上传阿里收集的用户认证模块数据
struct protoClientAliAuthResp : MSG_HEAD
{
	enum { IDD = PROTO_CLI_ALIAUTH_INFO_RESP, };

	uint32_t uin;
	int32_t result;
	// 0: 正常 1: 审核中 2: 模拟器 3: IDC IP 4: 手机非法

	protoClientAliAuthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//用户信息查询请求
struct protoQueryUserInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_USER_INFO_REQUEST, };
	uint32_t uin;

	protoQueryUserInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户信息查询应答
struct protoQueryUserInfoResponse : MSG_HEAD
{
	enum { IDD = QUERY_USER_INFO_RESPONSE, };
	uint32_t uin;
	uint32_t vip;
	char nick[64];
	char pic[256];
	char signature[256];

	protoQueryUserInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//获取头像名称请求
struct protoHallGetPlayerPicRequest : MSG_HEAD
{
    enum { IDD = HALL_GET_PLAYER_PIC_REQUEST, };
    uint32_t	type;
    char		data[128];

    protoHallGetPlayerPicRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//获取头像名称应答
struct protoHallGetPlayerPicRespone : MSG_HEAD
{
    enum { IDD = HALL_GET_PLAYER_PIC_RESPONSE, };
    enum FlagType
    {
        GetPictureFailure = 0,			// 获取头像失败
        Success = 1,							// 成功
        GetPicreqTypeError = 2,		// 请求包中type值错误
        PicVerify = 3							// 新头像审核中
    };

    uint32_t	flag;
    char		data[512];

    protoHallGetPlayerPicRespone();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//提交自定义头像请求
struct protoHallSubmitCustomPicRequest : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_CUSTOM_PIC_REQUEST, };
	uint32_t	type;
	char		data[512];

	protoHallSubmitCustomPicRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//提交自定义头像应答
struct protoHallSubmitCustomPicRespone : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_CUSTOM_PIC_RESPONE, };
	enum FlagType
	{
		SubmitCustomPicFailure = 0,			// 提交头像失败
		Success = 1,						// 成功
		PicVerify = 2						// 新头像审核中
	};

	uint32_t	flag;
	char		data[512];

	protoHallSubmitCustomPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//自定义头像审核结果通知
struct protoHallAuditingCustomPicRespone : MSG_HEAD
{
	enum { IDD = HALL_AUDITING_CUSTOM_PIC_RESPONE, };
	enum FlagType
	{
		SubmitCustomPicFailure = 0,			// 审核通过
		Success = 1,						// 审核中
		PicVerify = 2						// 审核未通过
	};

	uint32_t	flag;

	protoHallAuditingCustomPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrSyncVip : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SYNC_VIP, };
	uint32_t uin;
	uint32_t vip;

	protoSvrSyncVip();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoVerifyCodeRequestV2 : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_REQUEST_V2, };
	char msisdn[32];
	char channel[32];
	uint32_t source;
	uint32_t type;

	protoVerifyCodeRequestV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoVerifyCodeResponseV2 : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_RESPONSE_V2, };
	int32_t result;
	uint32_t type;

	protoVerifyCodeResponseV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


struct protoDriftBottleConditionRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CONDITION_REQUEST, };
    uint32_t uin;
    
    protoDriftBottleConditionRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleConditionResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CONDITION_RESPONSE, };
    uint32_t vip;
    uint32_t totalRecharge;
    int32_t maxBottleCount;
    int32_t	curBottleCount;
    int32_t maxErrCount;
    int32_t curErrCount;
    int32_t duration;
	int32_t maxChangePwdCount;	

    protoDriftBottleConditionResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct BottleItemInfo
{
    enum eType
    {
        Money = 1,				//趣玩币
        Coin = 2,				//金币
        Ticket = 3,				//奖券
        Trumpet = 4,			//喇叭
        Item = 5,				//道具
        BankCoin = 6,			//银行金币
        GoldDiamon = 7,         //金钻
    };

    uint32_t type;			// 1-趣玩币 2-金币 3-奖券 4-喇叭 5-道具
    uint32_t id;	        // 道具ID
    int64_t num;

    BottleItemInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoDriftBottleMakeRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_MAKE_REQUEST, };
    std::vector<BottleItemInfo> vecBottleItem;
    char pwd[8];

    protoDriftBottleMakeRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleMakeResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_MAKE_RESPONSE, };
    int32_t result;
    char id[24];

    protoDriftBottleMakeResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleOpenRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_OPEN_REQUEST, };
    char id[24];
    char pwd[8];

    protoDriftBottleOpenRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleOpenResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_OPEN_RESPONSE, };
    int32_t result;
    int32_t leftErrCount;
    std::vector<BottleItemInfo> vecBottleItem;

    protoDriftBottleOpenResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleTakebackRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_TAKEBACK_REQUEST, };
    char id[24];    

    protoDriftBottleTakebackRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleTakebackResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_TAKEBACK_RESPONSE, };
    int32_t result;
    
    protoDriftBottleTakebackResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct BottleRecordInfo
{
    uint32_t uin;			
    char  nick[32];	        
    int64_t time;
    char id[24];
    std::vector<BottleItemInfo> vecBottleItem;
    char pwd[8];
    uint32_t status;
    int32_t curChangePwdCount;

    BottleRecordInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


struct protoDriftBottleRecordRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_RECORD_REQUEST, };
    uint32_t type;

    protoDriftBottleRecordRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleRecordResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_RECORD_RESPONSE, };
    std::vector<BottleRecordInfo> vecBottleRecord;
    uint32_t type;

    protoDriftBottleRecordResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoSvrDriftBottleMakeRequest : MSG_HEAD
{
    enum { IDD = PROTO_HALL_DRIFTBOTTLE_REQ, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;
    char passwd[8];
    uint32_t tax;

    protoSvrDriftBottleMakeRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoSvrDriftBottleMakeResponse : MSG_HEAD
{
    enum { IDD = PROTO_HALL_DRIFTBOTTLE_RESP, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;
    char passwd[8];
    int32_t	result;
    uint32_t tax;

    protoSvrDriftBottleMakeResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoSvrAddBottleItemNotify : MSG_HEAD
{
    enum { IDD = PROTO_HALL_ADD_BOTTLEITEM_NOTIFY, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;

    protoSvrAddBottleItemNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleChangePwdRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CHANGEPWD_REQUEST, };
    char id[24];
    char pwd[8];

    protoDriftBottleChangePwdRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleChangePwdResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CHANGEPWD_RESPONSE, };
    int32_t result;
	int32_t leftChangePwdCount;

    protoDriftBottleChangePwdResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleQueryRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_QUERY_REQUEST, };
    char id[24];

    protoDriftBottleQueryRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleQueryResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_QUERY_RESPONSE, };
    int32_t result;

    protoDriftBottleQueryResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};
#endif // _GFX_PROXY_ACCOUNTPROTOCOL_H_
