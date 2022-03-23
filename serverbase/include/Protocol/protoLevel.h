/*
* =====================================================================================
*
*       Filename:  protoLevel.h
*
*    Description:  等级协议
*
*        Version:  1.0
*        Created:  2017年2月28日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/


#ifndef _QW_PROTOCOL_LEVEL_H_
#define _QW_PROTOCOL_LEVEL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

// 玩家等级请求
struct protoUserLevelRequest : MSG_HEAD
{
	enum { IDD = USER_LEVEL_REQUEST };

	uint32_t uin;

	protoUserLevelRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 玩家等级应答
struct protoUserLevelResponse : MSG_HEAD
{
	enum { IDD = USER_LEVEL_RESPONSE };

	uint32_t level;
	uint64_t exp;
	uint64_t needExp;

	protoUserLevelResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 玩家升级通知
struct protoUserLevelUpNotify : MSG_HEAD
{
	enum { IDD = USER_LEVEL_UP_NOTIFY };

	uint32_t uin;
	uint32_t level;
	uint64_t exp;
	uint32_t rewardId;

	protoUserLevelUpNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 玩家等级信息
struct LevelInfo
{
	uint32_t uin;
	uint32_t level;

	LevelInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoUserLevelSync : MSG_HEAD
{
	enum { IDD = USER_LEVEL_SYNC };

	std::vector<LevelInfo> vecUserLevel;

	protoUserLevelSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif // _QW_PROTOCOL_LEVEL_H_
