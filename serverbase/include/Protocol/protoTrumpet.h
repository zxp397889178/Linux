/*
* =====================================================================================
*
*       Filename:  protoTrumpet.h
*
*    Description:  喇叭协议
*
*        Version:  1.0
*        Created:  2018-11-05
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/


#ifndef _QW_PROTOCOL_TRUMPET_H_
#define _QW_PROTOCOL_TRUMPET_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"




struct protoBroadcastConfigRequest : MSG_HEAD
{
	enum { IDD = PROTO_BROADCAST_CONFIG_REQUEST, };
	
	uint32_t uin;

	protoBroadcastConfigRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};




struct protoBroadcastConfigResponse : MSG_HEAD
{
	enum { IDD = PROTO_BROADCAST_CONFIG_RESPONSE, };

    uint32_t maxMsgLength;
    
	protoBroadcastConfigResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



struct protoBroadcastMsgRequest : MSG_HEAD
{
	enum { IDD = PROTO_BROADCAST_MSG_REQUEST, };

    uint32_t uin;
    char message[128];
	protoBroadcastMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



struct protoBroadcastMsgResponse : MSG_HEAD
{
	enum { IDD = PROTO_BROADCAST_MSG_RESPONSE, };

    uint32_t result;
	protoBroadcastMsgResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoBroadcastMsgNotify : MSG_HEAD
{
	enum { IDD = PROTO_BROADCAST_MSG_NOTIFY, };
    uint32_t fromUin;
    char fromNickName[64];
    char message[128];

	protoBroadcastMsgNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};




#endif		
