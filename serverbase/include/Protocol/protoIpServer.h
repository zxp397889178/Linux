/*
* =====================================================================================
*
*       Filename:  protoIpServer.h
*
*    Description:  ipsvr相关协议
*
*        Version:  1.0
*        Created:  2016年4月21日 13时30分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_IPSERVER_H_
#define _QW_PROTOCOL_IPSERVER_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

// 客户端IP推送
struct protoClientIpPush : MSG_HEAD
{
	enum { IDD = CLIENT_IP_PUSH, };

	uint32_t 		ip;

	protoClientIpPush();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//// IP列表请求
//struct protoIpListRequest : MSG_HEAD
//{
//	enum { IDD = CLIENT_IP_PUSH, };
//
//	uint32_t 		source;
//	char			channel[64];
//	uint32_t		version;
//
//	protoIpListRequest();
//	int32_t Serialize(void *pBuf, size_t cbBuf);
//	int32_t Deserialize(void *pBuf, size_t cbBuf);
//	int32_t getProtoLen(void);
//};
//
//// IP列表应答
//struct protoIpListRespone : MSG_HEAD
//{
//	enum { IDD = CLIENT_IP_PUSH, };
//
//	std::vector<string> ipList;
//
//	protoIpListRespone();
//	int32_t Serialize(void *pBuf, size_t cbBuf);
//	int32_t Deserialize(void *pBuf, size_t cbBuf);
//	int32_t getProtoLen(void);
//};

#endif	// _QW_PROTOCOL_IPSERVER_H_
