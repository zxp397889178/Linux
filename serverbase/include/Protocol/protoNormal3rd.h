/*
 * =====================================================================================
 *
 *       Filename:  protoNormal3rd.h
 *
 *    Description:  么么娱乐的协议接口
 *
 *        Version:  1.0
 *        Created:  2016年04月13日 03时49分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _GFX_PROXY_NORMAL3RD_PROTOCOL_H_
#define _GFX_PROXY_NORMAL3RD_PROTOCOL_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>

#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
using namespace std;

// 查询么么货币的数据
struct proto3rdWealthReq: MSG_HEAD
{
	enum {IDD = PROTO_GAME_QUERYWEALTH_3RD, }; 

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];

	proto3rdWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct proto3rdWealthResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTH_QUERYWEALTH_3RD ,};

	enum
	{
		errSucceed = 0,
		errTimeout,	// 请求超时，必须退出游戏重试
	};
	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	int32_t		result;		// 结果，非0表示失败
	int64_t		coin;		// 可代出的金币数，平台的金币，非捕鱼的金币
	int64_t		ticket;		// 可代出的奖券数，暂为0
	int64_t		money;		// 可代出的钻石数，暂为0

	proto3rdWealthResp();
	explicit proto3rdWealthResp(proto3rdWealthReq &req);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求代出柠檬币
struct proto3rdWithdrawReq: MSG_HEAD
{
	enum { IDD =  PROTO_GAME_DRAWWEALTH_3RD, };

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;	// 请求单号，由游服自行维护
	int64_t		fishCoin;	// 请求代出的平台金币数(不能超过用户实际的金币数)
	int64_t		fishTicket;	// 请求代出的奖券数，暂为0
	int64_t		fishMoney;	// 请求代出的钻石数，暂为0
	bool		isFirstReq;	// 是否首次请求

	proto3rdWithdrawReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct proto3rdWithdrawResp : MSG_HEAD
{
	enum  { IDD = PROTO_AUTH_DRAWWEALTH_3RD, }; 

	enum
	{
		errSucceed = 0,
		errExpired,		// 未发现记录，或超时,必须退出游戏
		errCoinValue,	// 金币数量超额兑换
		errSystem,		// HTTP请求失败，请重试
		errFail,		// 合作方拒绝兑换
		errReject,		// 合作方拒绝兑换
		errDataFault,	// 合作方返回的数据错误
		errTimeout,		// 兑换超时
		errPlatform,	// 平台错误
	};

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;		// 订单号
	int32_t		result;			// <0 表示失败
	int32_t		ret_code;		// 平台返回的错误号
	char		ret_msg[64];	// 平台返回的错误信息
	int64_t		fishCoin;		// 实际代出的捕鱼金币数
	int64_t		fishTicket;		// 实际代出的奖券数，暂为0
	int64_t		fishMoney;		// 实际代出的钻石数，暂为0
	int64_t		leftCoin;		// 平台剩余可代出的平台金币数
	int64_t		leftTicket;		// 平台剩余可代出的奖券，暂为0
	int64_t		leftMoney;		// 平台剩余可代出的钻石，暂为0
	bool		isFirstReq;	// 是否首次请求

	proto3rdWithdrawResp();
	explicit proto3rdWithdrawResp(proto3rdWithdrawReq &req);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求代入柠檬币
struct proto3rdDepositReq: MSG_HEAD
{
	enum {IDD = PROTO_GAME_DEPOSITWEALTH_3RD,};

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;		// 请求单号，由游服自行维护
	int64_t		fishCoin;		// 请求代入的捕鱼金币数,不能超过用户实际金币数量，由游服判断
	int64_t		fishTicket;		// 请求代入的奖券数，暂为0
	int64_t		fishMoney;		// 请求代入的钻石数，暂为0

	proto3rdDepositReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct proto3rdDepositResp : MSG_HEAD
{
	enum {IDD = PROTO_AUTH_DEPOSITWEALTH_3RD,};
	enum
	{
		errSucceed = 0,
		errExpired,		// 未发现记录，或超时,必须退出游戏
		errOverMaxCoin,	// 兑换金额超过最大值
		errSystem,		// HTTP请求失败，请重试
		errFail,		// 合作方拒绝兑换
		errReject,		// 合作方拒绝兑换
		errDataFault,	// 合作方返回的数据错误
		errTimeout,		// 兑换超时
		errPlatform,	// 平台错误
		errCoinValue,	// 金币数量超额兑换
		
	};

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;		// 订单号
	int32_t		result;			// <0 失败
	int32_t		ret_code;		// 平台返回的错误号
	char		ret_msg[64];	// 平台返回的错误信息
	int64_t		fishCoin;		// 实际代入的捕鱼金币数(因为有汇率差，取整)
	int64_t		fishTicket;		// 实际代入的奖券数，暂为0
	int64_t		fishMoney;		// 实际代入的钻石数，暂为0
	int64_t		leftCoin;		// 平台剩余可代入的平台金币数
	int64_t		leftTicket;		// 平台剩余可代入的奖券，暂为0
	int64_t		leftMoney;		// 平台剩余可代入的钻石，暂为0

	proto3rdDepositResp();
	explicit proto3rdDepositResp(proto3rdDepositReq &req);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询订单完成情况
struct proto3rdOrderQueryReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_QUERYORDER_3RD, };

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;

	proto3rdOrderQueryReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct proto3rdOrderQueryResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTH_QUERYORDER_3RD, };
	enum
	{
		errSucceed = 0,
		errSystem,		// HTTP请求失败，请重试
		errFail,		// 合作方拒绝兑换
		errInvalidSign,	// 签名错误
		errDataFault,	// 合作方返回的数据错误
		errTimeout,		// 兑换超时
		errPlatform,	// 平台错误
	};

	uint32_t	uin;
	uint32_t	sourceId;
	char		szChannel[64];
	uint32_t	orderId;
	int32_t		result;
	int32_t		ret_code;
	char		ret_msg[64];

	proto3rdOrderQueryResp();
	explicit proto3rdOrderQueryResp(proto3rdOrderQueryReq &req);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*---------------------------------------------丰游BEGIN-----------------------------------------------------------*/
// 丰游金币奖券查询请求
struct QueryPhoneuWealthReq : MSG_HEAD
{
	enum { IDD = QUERY_PHONEU_WEALTH_REQUEST, };

	uint32_t	uin;

	QueryPhoneuWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游金币奖券查询应答
struct QueryPhoneuWealthResp : MSG_HEAD
{
	enum { IDD = QUERY_PHONEU_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int32_t result;

	QueryPhoneuWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游金币奖券代出请求
struct DrawPhoneuWealthReq : MSG_HEAD
{
	enum { IDD = DRAW_PHONEU_WEALTH_REQUEST, };

	uint32_t uin;
	int64_t coin;
	int64_t ticket;

	DrawPhoneuWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游金币奖券代出应答
struct DrawPhoneuWealthResp : MSG_HEAD
{
	enum { IDD = DRAW_PHONEU_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int64_t leftCoin;
	int64_t leftTicket;
	int32_t result;

	DrawPhoneuWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游金币奖券代入请求
struct DepositPhoneuWealthReq : MSG_HEAD
{
	enum { IDD = DEPOSIT_PHONEU_WEALTH_REQUEST, };

	uint32_t uin;
	int64_t coin;
	int64_t ticket;

	DepositPhoneuWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游金币奖券代入应答
struct DepositPhoneuWealthResp : MSG_HEAD
{
	enum { IDD = DEPOSIT_PHONEU_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int64_t leftCoin;
	int64_t leftTicket;
	int32_t result;

	DepositPhoneuWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*---------------------------------------------丰游END-----------------------------------------------------------*/


/*---------------------------------------------第三方BEGIN-----------------------------------------------------------*/
// 第三方金币奖券查询请求
struct Query3rdWealthReq : MSG_HEAD
{
	enum { IDD = QUERY_3RD_WEALTH_REQUEST, };

	uint32_t	uin;

	Query3rdWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 第三方金币奖券查询应答
struct Query3rdWealthResp : MSG_HEAD
{
	enum { IDD = QUERY_3RD_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int64_t money;
	int32_t result;

	Query3rdWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 第三方金币奖券代出请求
struct Draw3rdWealthReq : MSG_HEAD
{
	enum { IDD = DRAW_3RD_WEALTH_REQUEST, };

	uint32_t uin;
	int64_t coin;
	int64_t ticket;
	int64_t money;

	Draw3rdWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 第三方金币奖券代出应答
struct Draw3rdWealthResp : MSG_HEAD
{
	enum { IDD = DRAW_3RD_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int64_t money;
	int64_t leftCoin;
	int64_t leftTicket;
	int64_t leftMoney;
	int32_t result;
	int32_t	ret_code;		// 平台返回的错误号
	char ret_msg[64];		// 平台返回的错误信息

	Draw3rdWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 第三方金币奖券代入请求
struct Deposit3rdWealthReq : MSG_HEAD
{
	enum { IDD = DEPOSIT_3RD_WEALTH_REQUEST, };

	uint32_t uin;
	int64_t coin;
	int64_t ticket;
	int64_t money;

	Deposit3rdWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 第三方金币奖券代入应答
struct Deposit3rdWealthResp : MSG_HEAD
{
	enum { IDD = DEPOSIT_3RD_WEALTH_RESPONSE, };

	int64_t coin;
	int64_t ticket;
	int64_t money;
	int64_t leftCoin;
	int64_t leftTicket;
	int64_t leftMoney;
	int32_t result;
	int32_t	ret_code;		// 平台返回的错误号
	char ret_msg[64];		// 平台返回的错误信息

	Deposit3rdWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//第三方道具查询请求
struct Query3rdItemReq : MSG_HEAD
{
    enum { IDD = QUERY_3RD_ITEM_REQUEST, };

    uint32_t	uin;

    Query3rdItemReq();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

//第三方道具查询应答
struct Query3rdItemResp : MSG_HEAD
{
    enum { IDD = QUERY_3RD_ITEM_RESPONSE, };

    char itemList[32768];

    Query3rdItemResp();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

//第三方道具兑换请求
struct Exchange3rdItemReq : MSG_HEAD
{
    enum { IDD = EXCHANGE_3RD_ITEM_REQUEST, };

    uint32_t uin;
    uint32_t itemid;
    uint32_t num;

    Exchange3rdItemReq();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

//第三方道具兑换应答
struct Exchange3rdItemResp : MSG_HEAD
{
    enum { IDD = EXCHANGE_3RD_ITEM_RESPONSE, };

    int32_t	result;
    uint32_t coin;
    int32_t	ret_code;
    char ret_msg[64];

    Exchange3rdItemResp();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

struct CoinExchangeRecord
{
    uint32_t time;
    int32_t direct;
    uint32_t num;

    CoinExchangeRecord();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoCoinExchangeRecordReq : MSG_HEAD
{
    enum { IDD = COIN_EXCHANGE_RECORD_REQUEST, };

    uint32_t uin;
    uint32_t index;

    protoCoinExchangeRecordReq();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoCoinExchangeRecordResp : MSG_HEAD
{
    enum { IDD = COIN_EXCHANGE_RECORD_RESPONSE, };

    std::vector<CoinExchangeRecord> vecCoinExchangeRecord;

    protoCoinExchangeRecordResp();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct proto3rdBetResultNotify : MSG_HEAD
{
	enum { IDD = BETRESULT_3RD_NOTIFY, };

	uint32_t uin;
	uint32_t room_id;
	uint32_t bet_id;
	uint32_t bet_start;
	uint32_t bet_end;
	uint32_t bet_score;
	int64_t  gain;
	int64_t  cost;
	uint32_t beforefee;
	uint32_t afterfee;
	char	ip[32];
	char	gamedate[32];
	char	summarydate[32];
	char	bet_data[1024];
	uint32_t totalFire;

	proto3rdBetResultNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



/*---------------------------------------------第三方END-----------------------------------------------------------*/
#endif  //_GFX_PROXY_NORMAL3RD_PROTOCOL_H_ 



