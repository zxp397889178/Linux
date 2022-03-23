/*
* =====================================================================================
*
*       Filename:  protoSignIn.h
*
*    Description:  每日签到协议
*
*        Version:  1.0
*        Created:  2016年6月27日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_SIGNIN_H_
#define _PROTO_SIGNIN_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//每日签到查询请求
struct protoQuerySignInRequest : MSG_HEAD
{
	enum { IDD = QUERY_SIGNIN_REQUEST };

	uint32_t uin;

	protoQuerySignInRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//每日签到查询响应
struct protoQuerySignInRespone : MSG_HEAD
{
	enum { IDD = QUERY_SIGNIN_RESPONE };

	int64_t nowtime;
	uint32_t contiTimes;
	uint32_t totalTimes;
	uint32_t detail;

	protoQuerySignInRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//每日签到请求
struct protoSignInRequest : MSG_HEAD
{
	enum { IDD = SIGNIN_REQUEST };

	uint32_t uin;

	protoSignInRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//每日签到响应
struct protoSignInRespone : MSG_HEAD
{
	enum { IDD = SIGNIN_RESPONE };

	uint32_t signInReward;
	uint32_t contiReward;
	uint32_t totalReward;

	protoSignInRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_SIGNIN_H_

