/*
* =====================================================================================
*
*       Filename:  protoMoneyTree.h
*
*    Description:  摇钱树协议
*
*        Version:  1.0
*        Created:  2015年10月13日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_MONEYTREE_H_
#define _QW_PROTO_MONEYTREE_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoMoneyTreeInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_MONEYTREE_INFO_REQUEST, };
	uint32_t		uin;

	protoMoneyTreeInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoMoneyTreeInfoResponse : MSG_HEAD
{
	enum { IDD = PROTO_MONEYTREE_INFO_RESPONSE, };
	uint32_t		coin;
	uint32_t		firstFlag;
	uint32_t		leftTimes;
	uint32_t		yesterdayCoin;

	protoMoneyTreeInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoShakeMoneyTreeRequest : MSG_HEAD
{
	enum { IDD = PROTO_SHAKE_MONEYTREE_REQUEST, };
	uint32_t		uin;
	uint32_t		times;

	protoShakeMoneyTreeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoShakeMoneyTreeResponse : MSG_HEAD
{
	enum { IDD = PROTO_SHAKE_MONEYTREE_RESPONSE, };
	int32_t		result;
	uint32_t	gainCoin;

	protoShakeMoneyTreeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


struct protoMoneyTreeRuleRequest : MSG_HEAD
{
	enum { IDD = PROTO_MONEYTREE_RULE_REQUEST, };
	uint32_t		uin;

	protoMoneyTreeRuleRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoMoneyTreeRuleResponse : MSG_HEAD
{
	enum { IDD = PROTO_MONEYTREE_RULE_RESPONSE, };
	int64_t		costCoin;
	uint32_t	costCoinRate;			// 消耗金币配置
	uint32_t	gainCoinRate;			// 获得金币配置
	uint32_t	gainCoinOneTimes;		// 每次摇树获得金币
	uint32_t	minCoin;				// 最小摇树金币
	int32_t		reserve1;
	char		reserve2[128];

	protoMoneyTreeRuleResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


#endif	//_QW_PROTO_MONEYTREE_H_
