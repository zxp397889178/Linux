/*
* =====================================================================================
*
*       Filename:  protoWebProxy.cpp
*
*    Description:  webproxy protocol
*
*        Version:  1.0
*        Created:  2016年8月24日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  zengyh
*   Organization:  none

*
* =====================================================================================
*/

#ifndef _PROTO_WEBPROXY_H_
#define _PROTO_WEBPROXY_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"



//查询奖励信息请求
struct protoWebProxyRequest : MSG_HEAD
{
	enum { IDD = WEBPROXY_REQUEST };

	char method[16];
	char cmd[128];
	char data[10240];
	uint32_t data_len;

	protoWebProxyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询奖励信息响应
struct protoWebProxyResponse : MSG_HEAD
{
	enum { IDD = WEBPROXY_RESPONSE};

	uint32_t result;
	uint32_t returncode;
	char data[51200];
	
	protoWebProxyResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_WEBPROXY_H_

