/*
* =====================================================================================
*
*       Filename:  protoLevel.h
*
*    Description:  �ȼ�Э��
*
*        Version:  1.0
*        Created:  2017��2��28�� 9ʱ0��0��
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

// ��ҵȼ�����
struct protoUserLevelRequest : MSG_HEAD
{
	enum { IDD = USER_LEVEL_REQUEST };

	uint32_t uin;

	protoUserLevelRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// ��ҵȼ�Ӧ��
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

// �������֪ͨ
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

// ��ҵȼ���Ϣ
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
