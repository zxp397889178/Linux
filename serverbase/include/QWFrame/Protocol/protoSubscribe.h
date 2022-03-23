/*
* =====================================================================================
*
*   Filename:  protoSubscribe.h
*
*   Description:  订阅消息协议
*
*   Version:  1.0
*   Created:  2018-06-14
*   Revision:  none
*   Compiler:  gcc
*
*   Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _PROTO_SUBSCRIBE_H_
#define _PROTO_SUBSCRIBE_H_

#include <vector>
#include <stdint.h>
#include "protoBase.h"
#include "protoSysDef.h"

struct protoSubscribeMsgRequest : MSG_HEAD
{
	enum { IDD = PROTO_CENTERSVR_SUBSCRIBE_REQUEST };

	uint32_t svrId;
	char svrName[128];
    std::vector<uint32_t> vecProtoId;


	protoSubscribeMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};






#endif

