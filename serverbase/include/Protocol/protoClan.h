/*
* =====================================================================================
*
*       Filename:  protoClan.h
*
*    Description:  家族协议
*
*        Version:  1.0
*        Created:  2017年4月19日16:08:08
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  laiwj
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_CLAN_H_
#define _PROTO_CLAN_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"


// 帮派信息
struct ClanInfo
{
	uint32_t		clanId;				// 家族ID
	char			clanName[64];		// 家族名字
	char			clanImg[64];		// 家族图标
	char			passwd[64];			// 密码
	uint32_t		level;				// 家族等级 
	uint32_t		elderId;			// 族长ID
	char			elderNick[64];		// 族长名字
	uint32_t		perRate;			// 个人基金率
	uint32_t		perRateMax;			// 个人基金率上限
	uint32_t		perRateMin;			// 个人基金率下限
	uint32_t		totalRate;			// 家族总基金率
	uint32_t		fishprice;			// 鱼宝收购价
	uint32_t		fishpriceMax;		// 鱼宝收购价上限
	uint32_t		fishpriceMin;		// 鱼宝收购价下限
	uint32_t		fundPool;			// 家族基金池
	uint32_t		num;				// 家族人数
	uint32_t		toplimit;			// 家族上限人数
	uint32_t		status;				// 状态
	uint32_t		active;				// 活跃度
	char			notice[256];		// 公告
	uint64_t		makeover;			// 转让时间
	uint64_t		assessFlag;			// 考核标识
	uint32_t		getflag;			// 家族基金每日剩余提取次数
	uint32_t		getMax;				// 家族基金每日剩余提取金额
	uint32_t		fishVitality;		// 兑换一鱼宝获得的活跃度
	uint32_t		totalActive;		// 总活跃度

	ClanInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 玩家信息
struct MemberInfo
{
	uint32_t		uin;
	char			img[64];
	char			name[64];
	uint32_t		value;					//排序值，排序方式不同，值也不同
	uint32_t		activehistory;
	uint32_t		activetoday;
	uint32_t		vip;
	uint64_t		time;

	enum eStatusType
	{
		Offline = 0,				// 离线
		Online	= 1,				// 在线
		Other	= -1,				// 其他
	};

	MemberInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};
typedef std::vector<MemberInfo>	MEMBER_VEC;

// 聊天信息
struct MessageInfo
{
	uint32_t		uinFrom;
	uint32_t		uinTo;
	uint64_t		time;
	char			msg[128];

	MessageInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//创建家族-------------------------------
struct protoCreateClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_CREATE_REQUEST };

	uint32_t	uin;
	char		clanName[64];
	char		clanImg[64];
	char		notice[256];
	char		passwd[64];
	

	protoCreateClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//创建家族应答
struct protoCreateClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_CREATE_RESPONSE };

	int32_t result;
	uint32_t clanId;
	
	protoCreateClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询创建家族条件------------------------------
struct protoCreateClanConditionReq : MSG_HEAD
{
	enum { IDD = CLAN_CREATE_CONDITION_REQ };

	uint32_t	uin;

	protoCreateClanConditionReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询创建家族条件应答
struct protoCreateClanConditionResp : MSG_HEAD
{
	enum { IDD = CLAN_CREATE_CONDITION_RESP };

	int32_t	vip;
	int32_t	costType;
	int32_t	costNum;
	uint32_t joinVip;

	protoCreateClanConditionResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询家族列表数据----------------------------
struct protoSearchClanDataReq : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_DATA_REQUEST };

	uint32_t	uin;
	uint32_t	page;
	char		name[64];
	uint32_t	type;

	enum eApplyType
	{
		isApply = 1,				// 已申请
		noApply	= 2,				// 未申请
		All		= 0,				// 全部
	};

	protoSearchClanDataReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询家族列表数据应答
struct protoSearchClanDataResp : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_DATA_RESPONSE };

	int32_t	result;
	int32_t	totalpage;
	int32_t	page;

	std::vector<ClanInfo> infoList;

	protoSearchClanDataResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询在线玩家列表数据----------------------------
struct protoSearchPlayerDataReq : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_PALYER_DATA_REQUEST };

	uint32_t	uin;
	uint32_t	page;

	protoSearchPlayerDataReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询在线玩家列表数据应答
struct protoSearchPlayerDataResp : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_PALYER_DATA_RESPONSE };

	int32_t	result;
	int32_t	totalpage;
	int32_t	page;

	std::vector<MemberInfo> infoList;

	protoSearchPlayerDataResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//请求加入家族----------------------
struct protoJoinClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_JOIN_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;

	protoJoinClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//请求加入家族应答
struct protoJoinClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_JOIN_RESPONSE };

	int32_t	result;
	uint32_t clanId;

	protoJoinClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取请求加入家族列表----------------------
struct protoAskJoinClanListRequest : MSG_HEAD
{
	enum { IDD = CLAN_ASK_JOINLIST_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	page;

	protoAskJoinClanListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取请求加入家族列表应答
struct protoAskJoinClanListResponse : MSG_HEAD
{
	enum { IDD = CLAN_ASK_JOINLIST_RESPONSE };

	int32_t	result;
	int32_t	page;
	int32_t	totalpage;
	std::vector<MemberInfo> infoList;

	protoAskJoinClanListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长处理加入家族请求---------------------
struct protoAllowJoinClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_ALLOW_JOIN_REQUEST };

	enum eAccept
	{
		Reject = 0,			// 拒绝		
		Accept = 1,		// 接受
	};

	uint32_t	clanId;
	uint32_t	accept;
	std::vector<uint32_t> infoList;

	protoAllowJoinClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长处理加入家族请求应答
struct protoAllowJoinClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_ALLOW_JOIN_RESPONSE };

	int32_t	result;
	uint32_t	accept;
	std::vector<uint32_t> infoList;

	protoAllowJoinClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//邀请加入家族-------------------------------
struct protoInviteJoinClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOIN_REQUEST };

	uint32_t	clanId;
	std::vector<uint32_t> infoList;

	protoInviteJoinClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//邀请加入家族应答
struct protoInviteJoinClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOIN_RESPONSE };

	int32_t	result;

	protoInviteJoinClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取邀请加入家族列表-------------------------
struct protoInviteJoinClanListRequest : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOINLIST_REQUEST };

	uint32_t	uin;
	uint32_t	page;
	char		name[64];

	protoInviteJoinClanListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取邀请加入家族应答列表
struct protoInviteJoinClanListResponse : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOINLIST_RESPONSE };

	int32_t	result;
	int32_t	page;
	int32_t	totalpage;
	

	std::vector<ClanInfo> infoList;

	protoInviteJoinClanListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//玩家处理族长邀请加入家族请求----------------------
struct protoInviteJoinClanOperRequest : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOINOPER_REQUEST };

	uint32_t	clanId;
	uint32_t	uin;
	uint32_t	accept;

	enum eAccept
	{
		Reject = 0,			// 拒绝		
		Accept = 1,			// 接受
	};

	protoInviteJoinClanOperRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//玩家处理族长邀请加入家族请求应答
struct protoInviteJoinClanOperResponse : MSG_HEAD
{
	enum { IDD = CLAN_INVITE_JOINOPER_RESPONSE };

	int32_t	result;
	uint32_t clanId;
	uint32_t accept;

	protoInviteJoinClanOperResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//提取个人基金请求----------------------
struct protoGetoutPerFundRequest : MSG_HEAD
{
	enum { IDD = CLAN_GETOUT_PERFUND_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	coin;

	protoGetoutPerFundRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//提取个人基金应答
struct protoGetoutPerFundResponse : MSG_HEAD
{
	enum { IDD = CLAN_GETOUT_PERFUND_RESPONSE };

	int32_t	result;
	uint32_t leftCoin;

	protoGetoutPerFundResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//提取家族基金请求----------------------
struct protoGetoutClanFundRequest : MSG_HEAD
{
	enum { IDD = CLAN_GETOUT_CLANFUND_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		passwd[64];
	uint32_t	coin;

	protoGetoutClanFundRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//提取家族基金应答
struct protoGetoutClanFundResponse : MSG_HEAD
{
	enum { IDD = CLAN_GETOUT_CLANFUND_RESPONSE };

	int32_t	result;
	uint32_t fundPool;
	uint32_t leftCount;
	uint32_t canGetoutFund;

	protoGetoutClanFundResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改基金比率----------------------
struct protoModifyFundRateRequest : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_FUNDRATE_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	fundrate;

	protoModifyFundRateRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改基金比率应答
struct protoModifyFundRateResponse : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_FUNDRATE_RESPONSE };

	int32_t	result;
	uint32_t perRate;
	uint32_t totalRate;

	protoModifyFundRateResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改鱼宝价格----------------------
struct protoModifyFishPriceRequest : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_FISHPRICE_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	fishPrice;

	protoModifyFishPriceRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改鱼宝价格应答
struct protoModifyFishPriceResponse : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_FISHPRICE_RESPONSE };

	int32_t	result;
	uint32_t fishPrice;

	protoModifyFishPriceResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//兑换鱼宝----------------------
struct protoExchangeFishCoinRequest : MSG_HEAD
{
	enum { IDD = CLAN_EXCHANGE_FISHCONIN_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	fishCoin;
	uint32_t	fishPrice;

	protoExchangeFishCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//兑换鱼宝应答
struct protoExchangeFishCoinResponse : MSG_HEAD
{
	enum { IDD = CLAN_EXCHANGE_FISHCONIN_RESPONSE };

	int32_t	result;
	uint32_t leftFishCoin;
	uint32_t fundPool;
	uint32_t fishPrice;
	uint32_t activity;			// 家族活跃度

	protoExchangeFishCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长存入金币----------------------
struct protoDepositCoinRequest : MSG_HEAD
{
	enum { IDD = CLAN_DEPOSIT_COIN_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	coin;

	protoDepositCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长存入金币应答
struct protoDepositCoinResponse : MSG_HEAD
{
	enum { IDD = CLAN_DEPOSIT_COIN_RESPONSE };

	int32_t	result;
	uint32_t fundPool;

	protoDepositCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//玩家退出家族请求----------------------
struct protoQuitClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_QUIT_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;

	protoQuitClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//玩家退出家族应答
struct protoQuitClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_QUIT_RESPONSE };

	int32_t	result;

	protoQuitClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询家族信息----------------------------
struct protoGetClanInfoRequest : MSG_HEAD
{
	enum { IDD = CLAN_GET_INFO_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;

	protoGetClanInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询家族信息应答
struct protoGetClanInfoResponse : MSG_HEAD
{
	enum { IDD = CLAN_GET_INFO_RESPONSE };

	int32_t	result;
	uint32_t fishCoin;
	uint32_t fund;

	std::vector<ClanInfo> infoList;

	protoGetClanInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//编辑家族信息-------------------------------
struct protoModifyClanInfoRequest : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_INFO_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		clanName[64];
	char		clanImg[64];
	char		notice[256];


	protoModifyClanInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//编辑家族信息应答
struct protoModifyClanInfoResponse : MSG_HEAD
{
	enum { IDD = CLAN_MODIFY_INFO_RESPONSE };

	int32_t result;

	protoModifyClanInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//转让家族-------------------------------
struct protoMakeoverClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_MAKEOVER_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		passwd[64];
	uint32_t	other;

	protoMakeoverClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//转让家族应答
struct protoMakeoverClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_MAKEOVER_RESPONSE };

	int32_t result;

	protoMakeoverClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//解散家族-------------------------------
struct protoDisbandClanRequest : MSG_HEAD
{
	enum { IDD = CLAN_DISBAND_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		passwd[64];

	protoDisbandClanRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//解散家族应答
struct protoDisbandClanResponse : MSG_HEAD
{
	enum { IDD = CLAN_DISBAND_RESPONSE };

	int32_t result;

	protoDisbandClanResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//家族守护---------------------
struct protoGuardClanInfoRequest : MSG_HEAD
{
	enum { IDD = CLAN_GUARD_INFO_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;

	protoGuardClanInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//家族守护应答
struct protoGuardClanInfoResponse : MSG_HEAD
{
	enum { IDD = CLAN_GUARD_INFO_RESPONSE };

	int32_t	result;
	int32_t	vitality;
	int32_t	duration;

	protoGuardClanInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取家族成员列表----------------------
struct protoClanMemberListRequest : MSG_HEAD
{
	enum { IDD = CLAN_MEMBER_LIST_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		name[64];
	uint32_t	page;
	uint32_t	type;

	enum eType
	{
		Normal = 0,		// 正常的家族成员列表	
		Transfer = 1,		// 可转让家族的成员列表
	};
	
	protoClanMemberListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取家族成员列表应答
struct protoClanMemberListResponse : MSG_HEAD
{
	enum { IDD = CLAN_MEMBER_LIST_RESPONSE };

	int32_t	result;
	int32_t	elderId;
	int32_t	page;
	int32_t	totalpage;
	std::vector<MemberInfo> infoList;

	protoClanMemberListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取家族成员列表----------------------
struct protoClanOrderMemberRequest : MSG_HEAD
{
	enum { IDD = CLAN_ORDER_MEMBER_RESQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		name[64];
	uint32_t	page;
	uint32_t	orderField;
	uint32_t	orderType;

	enum eOrderField					// 请求类型
	{
		PresentedCrystal = 0,			// 获赠水晶数
		PresentCrystal = 1,				// 赠送水晶数
		Online = 2,						// 在线状态
		Vip = 3,						// 会员等级
	};

	enum eOrderType
	{
		Asc = 0,						// 升序
		Desc = 1,						// 降序
	};

	protoClanOrderMemberRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//获取家族成员列表应答
struct protoClanOrderMemberResponse : MSG_HEAD
{
	enum { IDD = CLAN_ORDER_MEMBER_RESPONSE };

	int32_t	result;
	int32_t	elderId;
	int32_t	page;
	int32_t	totalpage;
	uint32_t	orderField;
	uint64_t svrTime;
	std::vector<MemberInfo> infoList;

	protoClanOrderMemberResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//族长踢人----------------------
struct protoClanExpelMemberRequest : MSG_HEAD
{
	enum { IDD = CLAN_EXPEL_MEMBER_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	other;

	protoClanExpelMemberRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长踢人应答
struct protoClanExpelMemberResponse : MSG_HEAD
{
	enum { IDD = CLAN_EXPEL_MEMBER_RESPONSE };

	int32_t	result;

	protoClanExpelMemberResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改家族密码----------------------
struct protoChangeClanPasswdRequest : MSG_HEAD
{
	enum { IDD = CLAN_CHANGE_PASSWD_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		passwd[64];
	char		newpasswd[64];

	protoChangeClanPasswdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改家族密码应答
struct protoChangeClanPasswdResponse : MSG_HEAD
{
	enum { IDD = CLAN_CHANGE_PASSWD_RESPONSE };

	int32_t	result;

	protoChangeClanPasswdResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询用户角色
struct protoUserClanRoleRequest : MSG_HEAD
{
	enum { IDD = CLAN_USER_ROLE_REQUEST };

	uint32_t	uin;

	protoUserClanRoleRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//修改家族密码应答
struct protoUserClanRoleResponse : MSG_HEAD
{
	enum { IDD = CLAN_USER_ROLE_RESPONSE };

	uint32_t	clanId;
	uint32_t	elderId;

	protoUserClanRoleResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//创建家族扣款请求 hallsvr->gamesvr
struct protoSvrCreateClanCostRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_CREATE_COST_REQ };

	uint32_t	uin;
	char		clanName[64];
	char		clanImg[64];
	char		notice[256];
	char		passwd[32];
	uint32_t	costType;
	uint32_t	costNum;
	uint32_t	svrId;
	uint32_t	channelId;

	protoSvrCreateClanCostRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//创建家族应答
struct protoSvrCreateClanCostResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_CREATE_COST_RESP };

	uint32_t	uin;
	char		clanName[64];
	char		clanImg[64];
	char		notice[256];
	char		passwd[32];
	int32_t		result;
	uint32_t	svrId;
	uint32_t	channelId;

	protoSvrCreateClanCostResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//通知gameSvr增加金币----------------------
struct protoSvrAddItemNotifyEx : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_ADDITEMEX_NOTIFY };

	uint32_t	uin;
	uint32_t	itemId;
	uint32_t	num;
	uint32_t	reason;

	enum eType
	{
		TREASURE_ITEM = 1,			// 欢乐守宝
	};

	protoSvrAddItemNotifyEx();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长存入金币----------------------
struct protoSvrClanDepositCoinRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_DEPOSIT_COIN_REQ };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	coin;
	uint32_t	svrId;
	uint32_t	channelId;

	protoSvrClanDepositCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长存入金币应答
struct protoSvrClanDepositCoinResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_DEPOSIT_COIN_RESP };

	uint32_t	uin;
	uint32_t	clanId;
	uint32_t	coin;
	uint32_t	svrId;
	uint32_t	channelId;
	int32_t		result;

	protoSvrClanDepositCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//通知玩家家族信息变更相关消息----------------------
struct protoUserClanInfoNotify : MSG_HEAD
{
	enum { IDD = CLAN_USER_CLANINFO_NOTIFY };

	uint32_t	clanId;
	uint32_t	msgType;
	char		content[64];

	enum eType
	{
		AllowJoinRest	= 1,			// 族长通过加入申请
		ExpelMember		= 2,			// 族长踢人
		DisbandClan		= 3,			// 族长解散家族
		MakeOverClan	= 4,			// 族长转让家族
		ReqJoinClan = 5,				// 请求加入家族
		InviteClan = 6,					// 邀请加入家族
	};

	protoUserClanInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长发送群消息----------------------
struct protoClanMessageSendRequest : MSG_HEAD
{
	enum { IDD = CLAN_SEND_MESSAGE_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;
	char		msg[128];

	protoClanMessageSendRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长发送群消息应答
struct protoClanMessageSendResponse : MSG_HEAD
{
	enum { IDD = CLAN_SEND_MESSAGE_RESPONSE };

	int32_t	result;

	protoClanMessageSendResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长与成员聊天----------------------
struct protoClanMemberMsgSendRequest : MSG_HEAD
{
	enum { IDD = CLAN_SEND_MEMBERMSG_REQUEST };

	uint32_t	uinFrom;
	uint32_t	uinTo;
	uint32_t	clanId;
	char		msg[128];

	protoClanMemberMsgSendRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//族长与成员聊天应答
struct protoClanMemberMsgSendResponse : MSG_HEAD
{
	enum { IDD = CLAN_SEND_MEMBERMSG_RESPONSE };

	int32_t	result;
	char msg[128];
	uint64_t time;

	protoClanMemberMsgSendResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询聊天记录----------------------
struct protoSearchClanMsgRequest : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_MESSAGE_REQUEST };

	uint32_t	uin;
	uint32_t	other;
	uint32_t	clanId;

	protoSearchClanMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询聊天记录应答
struct protoSearchClanMsgResponse : MSG_HEAD
{
	enum { IDD = CLAN_SEARCH_MESSAGE_RESPONSE };

	int32_t	result;
	char	nick[64];
	char	img[64];
	std::vector<MessageInfo> infoList;

	protoSearchClanMsgResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//推送玩家聊天消息----------------------
struct protoClanMessageNotify : MSG_HEAD
{
	enum { IDD = CLAN_USER_CLANMESSAGE_NOTIFY };

	uint32_t	uinFrom;
	uint32_t	clanId;
	uint64_t	time;
	uint32_t	status;
	char		nick[64];
	char		img[64];
	char		msg[128];

	protoClanMessageNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//退出家族通知
struct protoQuitClanNotify : MSG_HEAD
{
	enum { IDD = CLAN_USER_QUIT_NOTIFY };

	vector<uint32_t> vecUin;

	protoQuitClanNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//取消申请加入家族----------------------
struct protoCancelClanJoinRequest : MSG_HEAD
{
	enum { IDD = CLAN_CANCEL_JOINCLAN_REQUEST };

	uint32_t	uin;
	uint32_t	clanId;

	protoCancelClanJoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//取消申请加入家族应答
struct protoCancelClanJoinResponse : MSG_HEAD
{
	enum { IDD = CLAN_CANCEL_JOINCLAN_RESPONSE };

	int32_t	result;
	uint32_t	clanId;

	protoCancelClanJoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //  _PROTO_CORNUCOPIA_H_

