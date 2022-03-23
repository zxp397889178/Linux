/*
 * =====================================================================================
 *
 *       Filename:  protoServer.h
 *
 *    Description:  between server comminute
 *
 *        Version:  1.0
 *        Created:  2015年09月04日 16时56分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_SYSTEM_PROTOCOL111_H_
#define _QW_SYSTEM_PROTOCOL111_H_

#include "protoSerialize.h"
#include "BaseCode.h"

#include "protoBase.h"
#include "protoSysDef.h"

struct protoSysAuthorize : MSG_HEAD
{
	enum { IDD = PROTO_SYS_AUTHORIZE, };

	uint32_t	actor;
	uint32_t	serverId;		// 服务器ID
	uint32_t	channelId;		// 频道ID, 一个服务器连接有多个频道
	uint8_t		key[64];
	char			szIp[32];
	int32_t		port;

	protoSysAuthorize(uint32_t _actor = 0, uint32_t _serverId = 0, uint32_t _channelId = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSysAuthorizeResp : MSG_HEAD
{
	enum { IDD =  PROTO_SYS_AUTHORIZE_RESP, };	

	uint32_t	actor;
	uint32_t	serverId;
	uint32_t	channelId;
	int32_t		result;		// 1 - OK, 0 - fail

	protoSysAuthorizeResp(uint32_t _actor = 0, uint32_t _serverId = 0, uint32_t _channelId = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSysHeartBeat : MSG_HEAD
{
	enum { IDD = PROTO_SYS_HEARTBEAT_RESP, };

	char	extra_code[16];

	protoSysHeartBeat(uint32_t _actor = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoSysClientShutdown : MSG_HEAD
{
	enum { IDD = SYS_CLIENT_CLOSE_REQUEST, };

	uint32_t		uin;
	uint32_t		reason;

	protoSysClientShutdown(uint32_t _uin = 0, uint32_t _reason = 0);

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

// 服务器之间客户端断开
struct protoSysSessionShutdown : MSG_HEAD
{
	enum { IDD = PROTO_SYS_SESSION_SHUTDOWN, };

	uint32_t actor;
	uint32_t svrId;
	uint32_t channelId;

	protoSysSessionShutdown();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 转发消息
struct protoSysRepostMsg : MSG_HEAD
{
	enum { IDD = PROTO_SYS_REPOST_MSG, };

	uint32_t msgId;
	uint32_t cbData;
	void* pData;

	protoSysRepostMsg();
	~protoSysRepostMsg();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_RECHARGE_SERVER_PROTO_SERVER_H_


