/*
 * =====================================================================================
 *
 *       Filename:  protoRank.h
 *
 *    Description:  用户排名信息查询
 *
 *        Version:  1.0
 *        Created:  2015年10月06日 10时40分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_PROTO_RANK_H_
#define _QW_CRAZYFISH_PROTO_RANK_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoDef.h"

/*
 * 注意，所有排名信息，只查询前50名
 */

// 查询排行榜数据
struct protoAllRankRequest : MSG_HEAD
{
	enum { IDD = ALL_RANK_REQUEST, };

	uint32_t		uin;
	uint32_t		roomId;

	protoAllRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 排行榜数据应答
struct protoAllRankRespone : MSG_HEAD
{
	enum  { IDD = ALL_RANK_RESPONE ,};

	struct RankInfo
	{
		uint32_t		uin;
		uint32_t		rank;
		char			nick[128];
		uint32_t		score;
		char			begin_time[16];

		RankInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};

	std::vector<struct RankInfo>	vecRank;

	protoAllRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询用户的排名信息
struct protoUserRankRequest : MSG_HEAD
{
	enum { IDD = USER_RANK_REQUEST , };
	uint32_t		uin;
	uint32_t		roomId;

	protoUserRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询用户排名信息应答
struct protoUserRankRespone : MSG_HEAD
{
	enum { IDD = USER_RANK_RESPONE , };

	uint32_t		rank;
	uint32_t		score;

	protoUserRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询用户金币排行
struct protoCoinRankRequest : MSG_HEAD
{
	enum  { IDD = COIN_RANK_REQUEST, };
	uint32_t		uin;

	protoCoinRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户金币排行应答
struct protoCoinRankRespone : MSG_HEAD
{
	enum  { IDD = COIN_RANK_RESPONE, };
	typedef protoAllRankRespone::RankInfo	RankInfo;

	std::vector<RankInfo>		vecRank;
	uint32_t					rank;

	protoCoinRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询用户奖券排行信息
struct protoTicketRankRequest : MSG_HEAD
{
	enum  { IDD = TICKET_RANK_REQUEST, };
	uint32_t					uin;

	protoTicketRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户奖券排行信息结果
struct protoTicketRankRespone : MSG_HEAD
{
	enum { IDD = TICKET_RANK_RESPONE, };
	typedef protoAllRankRespone::RankInfo	RankInfo;
	
	std::vector<RankInfo>		vecRank;
	uint32_t					rank;

	protoTicketRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct NewRankInfo
{
	uint32_t	uin;
	char		nick[32];
	int64_t		score;
	int32_t		rank;

	NewRankInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct NewRankInfoVip
{
	uint32_t	uin;
	char		nick[32];
	int64_t		score;
	int32_t		rank;
	uint32_t	vip;

	NewRankInfoVip();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


// 查询用户悬赏排行 OAR->offer a reward
struct protoOARRankRequest : MSG_HEAD
{
	enum  { IDD = OFFERAREWARD_RANK_REQUEST, };
	uint32_t		uin;

	protoOARRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户悬赏排行应答
struct protoOARRankRespone : MSG_HEAD
{
	enum  { IDD = OFFERAREWARD_RANK_RESPONSE, };
	typedef protoAllRankRespone::RankInfo	RankInfo;

	std::vector<RankInfo>		vecRank;
	uint32_t					rank;

	protoOARRankRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct WealthRankInfo
{
	uint32_t uin;
	char nick[64];
	char pic[128];
	uint64_t wealth;
	uint32_t vip;
	char signature[256];

	WealthRankInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct WealthRankInfoEx
{
	WealthRankInfo rankInfo;
	int32_t sign_show_flag;
};

typedef std::vector<WealthRankInfo> WEALTH_RANK_VEC;
typedef std::vector<WealthRankInfoEx> WEALTH_RANK_VECEX;

// 财富排行榜请求
struct protoWealthRankRequest : MSG_HEAD
{
	enum  { IDD = WEALTH_RANK_REQUEST, };
	uint32_t uin;
	uint32_t type;

	enum QueryType
	{
		TopThree = 0,			// 金币排行榜前3名
		All = 1,				// 金币排行榜全部
		GainRank = 10,			// 玩家赚钱排行榜
		CrystalTopThree = 20,	// 黄金炮座排行榜前3
		CrystalAll = 21,		// 黄金炮座排行榜全部
		Coin = 30,				// 金币排行(所有财富换算成金币)
		Ticket = 31,			// 奖券排行
		Level = 32,				// 等级排行
	};

	protoWealthRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 财富排行榜应答
struct protoWealthRankResponse : MSG_HEAD
{
	enum  { IDD = WEALTH_RANK_RESPONSE, };

	uint32_t type;
	std::vector<WealthRankInfo>	infoList;
	
	protoWealthRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct ClanCrystalRankInfo
{
	uint32_t clanId;
	char clanName[64];
	char clanPic[128];
	uint64_t crystal;
	char clanNotice[256];
	uint8_t isApply;
	char reverse1[128];

	ClanCrystalRankInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

typedef std::vector<ClanCrystalRankInfo> CLAN_CRYSTAL_RANK_VEC;

// 财富排行榜请求
struct protoClanCrystalRankRequest : MSG_HEAD
{
	enum  { IDD = CLAN_CRYSTAL_RANK_REQUEST, };
	uint32_t type;

	enum QueryType
	{
		TopThree = 0,		// 前3名
		All = 1,			// 全部
	};

	protoClanCrystalRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 财富排行榜应答
struct protoClanCrystalRankResponse : MSG_HEAD
{
	enum  { IDD = CLAN_CRYSTAL_RANK_RESPONSE, };

	uint32_t type;
	CLAN_CRYSTAL_RANK_VEC	infoList;
	uint32_t canApplyNum;

	protoClanCrystalRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



// 财富排行榜请求V2
struct protoWealthRankRequestV2 : MSG_HEAD
{
	enum  { IDD = WEALTH_RANK_REQUEST_V2, };
	uint32_t uin;
	uint32_t type;

	enum QueryType
	{
		Torpedo = 0,			// 鱼雷排行
		Liveness = 1,			// 活跃度排行
	};

	protoWealthRankRequestV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 财富排行榜应答V1
struct protoWealthRankResponseV2 : MSG_HEAD
{
	enum  { IDD = WEALTH_RANK_RESPONSE_V2, };

	uint32_t type;
	std::vector<WealthRankInfo>	normalList;		// 普通玩家
	std::vector<WealthRankInfo>	specialList;	// 黑金用户

	protoWealthRankResponseV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_CRAZYFISH_PROTO_RANK_H_


