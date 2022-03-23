/*
* =====================================================================================
*
*       Filename:  protoRobot.h
*
*    Description:  机器人协议
*
*        Version:  1.0
*        Created:  2017年4月17日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_ROBOT_H_
#define _QW_PROTOCOL_ROBOT_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoRobotCreateRequest : MSG_HEAD
{
	enum { IDD = PROTO_ROBOT_CREATE_REQ, };
	uint32_t roomId;
	uint32_t tableId;
	uint8_t seatId;

	protoRobotCreateRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoRobotCreateResponse : MSG_HEAD
{
	enum { IDD = PROTO_ROBOT_CREATE_RESP, };
	int32_t	result;
	uint32_t uin;
	uint32_t roomId;
    uint32_t coin;
    uint32_t level;
	char nick[64];

	protoRobotCreateResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoRobotDropRequest : MSG_HEAD
{
	enum { IDD = PROTO_ROBOT_DROP_REQ, };
	uint32_t uin;

	protoRobotDropRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoRobotDropResponse : MSG_HEAD
{
	enum { IDD = PROTO_ROBOT_DROP_RESP, };
	int32_t	result;
	uint32_t uin;

	protoRobotDropResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

#endif	// _QW_PROTOCOL_ROBOT_H_

