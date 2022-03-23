/*
* =====================================================================================
*
*       Filename:  protoPointKill.h
*
*    Description:  点杀协议
*
*        Version:  1.0
*        Created:  2018年03月25日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_POINTKILL_H_
#define _QW_PROTOCOL_POINTKILL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoSvrPointKillNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_POINTKILL_NOTIFY, };
	uint32_t uin;
	uint32_t value;

	protoSvrPointKillNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoSvrPointKillValueNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_POINTKILL_VALUE_NOTIFY, };
	uint32_t uin;
	uint32_t leftValue;
	int32_t killedValue;

	protoSvrPointKillValueNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


#endif	// _QW_PROTOCOL_POINTKILL_H_

