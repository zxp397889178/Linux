/*
* =====================================================================================
*
*       Filename:  protoHall.h
*
*    Description:  fishsvr protocol
*
*        Version:  1.0
*        Created:  2017年3月29日10:39:08
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_FISH_H_
#define _PROTO_FISH_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"



//同步用户数据
struct protoHallUserInfoNotify : MSG_HEAD
{
	enum { IDD = USER_INFO_NOTIFY };

	uint32_t uin;
	uint32_t coin;
	uint32_t money; // 砖石
	uint32_t ticket;
	uint32_t vip;
	uint32_t level;
	uint32_t exp;
//	uint32_t GetScore; // 总上分
//	uint32_t PutScore; // 总下分
	

	protoHallUserInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//同步打鱼数据
struct protoHallHitFishNotify : MSG_HEAD
{
	enum { IDD = HIT_FISH_NOTIFY };

	uint32_t uin;
	vector<uint32_t> vecFishTypeId; //命中鱼
	
	protoHallHitFishNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //  _PROTO_CORNUCOPIA_H_

