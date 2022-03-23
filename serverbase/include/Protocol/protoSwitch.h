/*
* =====================================================================================
*
*       Filename:  protoSwitch.h
*
*    Description:  功能开关协议
*
*        Version:  1.0
*        Created:  2016年4月27日 18时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  xiejiafeng
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_SWITCH_H_
#define _QW_PROTO_SWITCH_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct SwitchInfo
{
	uint32_t id;
	uint32_t flag;
	
	SwitchInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//更新服务器功能开关
struct protoUpdateSvrSwitchRequest : MSG_HEAD
{
	enum { IDD = UPD_SWITCH_REQUEST, };
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoUpdateSvrSwitchRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoUpdateSvrSwitchRespone : MSG_HEAD
{
	enum { IDD = UPD_SWITCH_RESPONSE, };
	std::vector<SwitchInfo> switchList;
    char customer[512];
    char serviceTime[512];

	protoUpdateSvrSwitchRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//游戏服务器功能开关
struct protoGameSvrSwitchRequest : MSG_HEAD
{
	enum { IDD = GAME_SWITCH_REQUEST, };
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoGameSvrSwitchRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoGameSvrSwitchRespone : MSG_HEAD
{
	enum { IDD = GAME_SWITCH_RESPONSE, };
	std::vector<SwitchInfo> switchList;

	protoGameSvrSwitchRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif	//_QW_PROTO_SWITCH_H_
