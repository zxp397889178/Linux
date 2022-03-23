/*
* =====================================================================================
*
*       Filename:  protoVip.h
*
*    Description:  vip协议
*
*        Version:  1.0
*        Created:  2016年5月25日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_VIP_H_
#define _PROTO_VIP_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//查询vip捕鱼鱼分奖励请求
struct protoQueryVipFishRewardRequest : MSG_HEAD
{
	enum { IDD = QUERY_VIPFISHREWARD_REQUEST };

	uint32_t uin;

	protoQueryVipFishRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询vip捕鱼鱼分奖励应答
struct protoQueryVipFishRewardRespone : MSG_HEAD
{
	enum { IDD = QUERY_VIPFISHREWARD_RESPONE };

	uint32_t ticket;

	protoQueryVipFishRewardRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//接收VIP礼包请求
struct protoReceiveVipRewardRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_VIP_REWARD_REQUEST };

	uint32_t uin;
	uint32_t vip_level;

	protoReceiveVipRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//接收vip礼包应答
struct protoReceiveVipRewardRespone : MSG_HEAD
{
	enum { IDD = RECEIVE_VIP_REWARD_RESPONE };

	enum Result
	{
		OK = 1,						// 成功
		INSUFFICIENT_TICKET = 2,	// 奖券不足
	};

	uint32_t result;
	uint32_t reward_id;

	protoReceiveVipRewardRespone(int32_t i_result = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询VIP礼包请求
struct protoQueryVipRewardRequest : MSG_HEAD
{
	enum { IDD = QUERY_VIP_REWARD_REQUEST };

	uint32_t uin;

	protoQueryVipRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询VIP礼包应答
struct protoQueryVipRewardRespone : MSG_HEAD
{
	enum { IDD = VIP_REWARD_RESPONE };

	struct VipRewardInfo
	{
		uint32_t vip_level;
		uint32_t status;

		VipRewardInfo();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};	
	
	vector<VipRewardInfo> vecVipRewardInfo;

	protoQueryVipRewardRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_VIP_H_

