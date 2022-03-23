/*
* =====================================================================================
*
*       Filename:  protoHeartbeat.h
*
*    Description:  心跳相关协议
*
*        Version:  1.0
*        Created:  2016年3月7日 13时30分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi 
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_HEARTBEAT_H_
#define _QW_PROTOCOL_HEARTBEAT_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//暂停心跳请求
struct protoPauseHeartbeatRequest : MSG_HEAD
{
	enum { IDD = PAUSE_HEARTBEAT_REQUEST, };
	uint32_t	uin;

	protoPauseHeartbeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//恢复心跳请求
struct protoResumeHeartbeatRequest : MSG_HEAD
{
	enum { IDD = RESUME_HEARTBEAT_REQUEST, };
	uint32_t	uin;

	protoResumeHeartbeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 客户端心跳请求
struct protoClientHeartbeatRequest : MSG_HEAD
{
	enum { IDD = CLIENT_HEARTBEAT_REQUEST, };
	int64_t	time;
	int32_t	field1;
	int32_t	field2;

	protoClientHeartbeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 客户端心跳应答
struct protoClientHeartbeatRespone : MSG_HEAD
{
	enum { IDD = CLIENT_HEARTBEAT_RESPONE, };
	int64_t	time;
	int32_t	field1;
	int32_t	field2;

	protoClientHeartbeatRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif	// _QW_PROTOCOL_HEARTBEAT_H_
