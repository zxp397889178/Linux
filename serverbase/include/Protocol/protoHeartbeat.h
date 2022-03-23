/*
* =====================================================================================
*
*       Filename:  protoHeartbeat.h
*
*    Description:  �������Э��
*
*        Version:  1.0
*        Created:  2016��3��7�� 13ʱ30��0��
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

//��ͣ��������
struct protoPauseHeartbeatRequest : MSG_HEAD
{
	enum { IDD = PAUSE_HEARTBEAT_REQUEST, };
	uint32_t	uin;

	protoPauseHeartbeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�ָ���������
struct protoResumeHeartbeatRequest : MSG_HEAD
{
	enum { IDD = RESUME_HEARTBEAT_REQUEST, };
	uint32_t	uin;

	protoResumeHeartbeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// �ͻ�����������
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

// �ͻ�������Ӧ��
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
