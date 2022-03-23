/*
* =====================================================================================
*
*       Filename:  protoChat.h
*
*    Description:  聊天协议
*
*        Version:  1.0
*        Created:  2015年10月15日 16时45分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_CHATPROTOCOL_H_
#define _GFX_PROXY_CHATPROTOCOL_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//SendMsgRequest
struct protoSendMsgRequest : MSG_HEAD
{
	enum { IDD = SENDMSGREQUEST, };
	char		send_name[128];
	char		recv_name[128];
	char		data[128];
	
	protoSendMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//SendMsgRespone
struct protoSendMsgRespone : MSG_HEAD
{
	enum { IDD = SENDMSGRESPONE, };
	uint32_t	flag;

	protoSendMsgRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//公聊发送消息请求
struct protoSendPublicMsgRequest : MSG_HEAD
{
	enum { IDD = SENDPUBLICMSGREQUEST, };
	uint32_t	uin;
	char		data[128];

	protoSendPublicMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//公聊发送消息应答
struct protoSendPublicMsgRespone : MSG_HEAD
{
	enum { IDD = SENDPUBLICMSGRESPONE, };
	uint32_t	flag;

	protoSendPublicMsgRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取免费聊天次数请求
struct protoGetFreeChatCountRequest : MSG_HEAD
{
	enum { IDD = GETFREECHATCOUNTREQUEST, };
	uint32_t	uin;

	protoGetFreeChatCountRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取免费聊天次数应答
struct protoGetFreeChatCountRespone : MSG_HEAD
{
	enum { IDD = GETFREECHATCOUNTRESPONE, };
	uint32_t	number;

	protoGetFreeChatCountRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//世界聊天应答
struct protoSendMsgToClientRespone : MSG_HEAD
{
	enum { IDD = SENDMSGTOCLIENTRESPONE, };
	char	name[128];
	char	data[128];

	protoSendMsgToClientRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif // _GFX_PROXY_CHATPROTOCOL_H_
