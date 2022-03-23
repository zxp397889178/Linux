/*
* =====================================================================================
*
*       Filename:  protoIpServer.h
*
*    Description:  ipsvr���Э��
*
*        Version:  1.0
*        Created:  2016��4��21�� 13ʱ30��0��
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

// �ͻ���IP����
struct protoClientIpPush : MSG_HEAD
{
	enum { IDD = CLIENT_IP_PUSH, };

	uint32_t 		ip;

	protoClientIpPush();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//// IP�б�����
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
//// IP�б�Ӧ��
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
