/*
* =====================================================================================
*
*       Filename:  protoChat.h
*
*    Description:  ����Э��
*
*        Version:  1.0
*        Created:  2015��10��15�� 16ʱ45��0��
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

//���ķ�����Ϣ����
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

//���ķ�����ϢӦ��
struct protoSendPublicMsgRespone : MSG_HEAD
{
	enum { IDD = SENDPUBLICMSGRESPONE, };
	uint32_t	flag;

	protoSendPublicMsgRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ��������������
struct protoGetFreeChatCountRequest : MSG_HEAD
{
	enum { IDD = GETFREECHATCOUNTREQUEST, };
	uint32_t	uin;

	protoGetFreeChatCountRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ����������Ӧ��
struct protoGetFreeChatCountRespone : MSG_HEAD
{
	enum { IDD = GETFREECHATCOUNTRESPONE, };
	uint32_t	number;

	protoGetFreeChatCountRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��������Ӧ��
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
