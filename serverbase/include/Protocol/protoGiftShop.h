/*
* =====================================================================================
*
*       Filename:  protoGiftShop.h
*
*    Description:  GiftShop协议
*
*        Version:  1.0
*        Created:  2016年5月25日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  zengyh
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_GIFTSHOP_H_
#define _PROTO_GIFTSHOP_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//繁体版登陆兑换礼品TOKEN请求
struct protoGiftShopLoginReq : MSG_HEAD
{
	enum { IDD = GIFTSHOP_LOGIN_REQUEST, };

	uint32_t 	uin;

	protoGiftShopLoginReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 繁体版登陆兑换礼品TOKEN请求回复
struct protoGiftShopLoginResp : MSG_HEAD
{
	enum { IDD = GIFTSHOP_LOGIN_RESPONSE, };
	uint32_t 	uin;
	char 	token[33];
	char 	reserve[512];

	protoGiftShopLoginResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//查询用户身上的奖券
struct protoTicketQueryRequest : MSG_HEAD
{
	enum { IDD = GIFTSHOP_QUERY_TICKET_REQUEST };
	uint32_t seq;					//请求的包序列号, 请求方递增
	uint32_t uin;					//查询的用户ID

	protoTicketQueryRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询用户身上的奖券回复
struct protoTicketQueryResponse : MSG_HEAD
{
	enum { IDD = GIFTSHOP_QUERY_TICKET_RESPONSE };

	uint32_t seq;					//请求中的包序列号，原值返回
	uint32_t result;				//查询结果, 0：成功，1：用户不在线，-1：查询失败
	uint32_t ticket;				//用户身上的奖券

	protoTicketQueryResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//扣除用户身上的奖券
struct protoTicketCostRequest : MSG_HEAD
{
	enum { IDD = GIFTSHOP_COST_TICKET_REQUEST };

	uint32_t seq;										//请求的包序列号， 请求方递增
	uint32_t uin;										//扣除奖券的用户ID
	uint32_t costTicket;							//扣除奖券数
	char szOrderId[32];							//扣除的订单ID
	uint32_t productid;							//购买的产品ID

	protoTicketCostRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//扣除用户身上的奖券回复
struct protoTicketCostResponse : MSG_HEAD
{
	enum { IDD = GIFTSHOP_COST_TICKET_RESPONSE };

	uint32_t seq;							//请求中的包序列号，原值返回
	uint32_t result;						//扣除结果， 0：成功， 1：用户不在线， -1：其它失败
	uint32_t restTicket;				//剩余的奖券

	protoTicketCostResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//商城购买道具扣款请求
struct protoSvrGiftShopBuyItemChargeRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ITEMCHARGE_REQ };

	uint32_t uin;										//扣除奖券的用户ID
	uint32_t costTicket;								//扣除奖券数
	uint32_t gainCoin;									//加的金币数
	uint32_t itemId;									//道具ID
	uint32_t num;
	char mobile[32];
	char consignee[64];
	char postalCode[16];
	char address[256];
	char channel[64];
	uint32_t svrId;
	uint32_t channelId;
	uint32_t sessionId;
	uint32_t language;
	uint32_t exchangeLimit;								// 限制购买次数
	uint32_t resupplyTime;								// 补货时间
	uint32_t resupplyBoostCharge;
	uint32_t resupplyBoostVip;
	uint32_t itemClass;
	uint32_t costMoney;									//扣除钻石数
	uint32_t costCoin;									//扣除金币数
	uint32_t version;									//版本号

	protoSvrGiftShopBuyItemChargeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//商城购买道具扣款应答
struct protoSvrGiftShopBuyItemChargeResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ITEMCHARGE_RESP };

	uint32_t result;					//扣除结果， 0：成功， 1：用户不在线， -1：其它失败
	uint32_t itemId;					//道具ID
	uint32_t num;
	char mobile[32];
	char consignee[64];
	char postalCode[16];
	char address[256];
	char channel[64];
	uint32_t svrId;
	uint32_t channelId;
	uint32_t sessionId;
	uint32_t language;
	uint32_t uin;
	char nick[64];
	uint32_t costTicket;								//扣除奖券数
	uint32_t exchangeLimit;								// 限制购买次数
	uint32_t resupplyTime;								// 补货时间
	uint32_t resupplyBoostCharge;
	uint32_t resupplyBoostVip;
	uint32_t costMoney;									//扣除钻石数
	uint32_t costCoin;									//扣除金币数
	uint32_t version;									//版本号

	protoSvrGiftShopBuyItemChargeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //_PROTO_GIFTSHOP_H_

