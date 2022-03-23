/*
 * =====================================================================================
 *
 *       Filename:  protoServer.cpp
 *
 *    Description:  between server comminute
 *
 *        Version:  1.0
 *        Created:  2015年09月12日 16时56分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  liuwx
 *   Organization:  none
 *
 * =====================================================================================
 */

#include "protoSystem.h"
#include "protoBase.h"

#include "zlib.h"

protoSysAuthorize::protoSysAuthorize(uint32_t _actor, uint32_t _serverId, uint32_t _channelId)
{
	protoId = protoSysAuthorize::IDD;
	protoLen 	= 0;
	actor 		= _actor;
	serverId 	= _serverId;
	channelId 	= _channelId;
	port 		= 0;
	memset(&key[0], 0, sizeof(key));
	memset(&szIp[0], 0, sizeof(szIp));
}

int32_t protoSysAuthorize::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(actor, 			-4);
	SERIALIZE_DATA(serverId, 		-5);
	SERIALIZE_DATA(channelId, 		-6);
	SERIALIZE_BUF(key, sizeof(key), -7);	
	SERIALIZE_CHAR(szIp, 			-8);
	SERIALIZE_DATA(port, 			-10);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysAuthorize::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(actor, 			-4);
	DESERIALIZE_DATA(serverId, 			-5);
	DESERIALIZE_DATA(channelId, 		-6);
	DESERIALIZE_BUF(key, sizeof(key), 	-7);	
	DESERIALIZE_CHAR(szIp, 				-8);
	DESERIALIZE_DATA(port, 				-10);

	return (int32_t)offset;
}

int32_t protoSysAuthorize::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+ sizeof(uint32_t)
		+ sizeof(uint32_t)
		+ sizeof(uint32_t)
		+ sizeof(key)
		+ LENGTH_STR(szIp)
		+ sizeof(int32_t);
}

///////////////////////////////////////////////////////////////////////////////////////////
protoSysAuthorizeResp::protoSysAuthorizeResp(uint32_t _actor, uint32_t _serverId, uint32_t _channelId)
	: actor(_actor)
	, serverId(_serverId)
	, channelId(_channelId)
	, result(0) 
{
	protoId = protoSysAuthorizeResp::IDD;
	protoLen = 0;
}

int32_t protoSysAuthorizeResp::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(actor, 		-4);
	SERIALIZE_DATA(serverId, 	-5);
	SERIALIZE_DATA(channelId,	-6);
	SERIALIZE_DATA(result, 		-7);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysAuthorizeResp::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(actor, 	-4);
	DESERIALIZE_DATA(serverId, 	-5);
	DESERIALIZE_DATA(channelId, -6);
	DESERIALIZE_DATA(result, 	-7);

	return (int32_t)offset;
}

int32_t protoSysAuthorizeResp::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+ sizeof(uint32_t)*3
		+ sizeof(int32_t);
}


///////////////////////////////////////////////////////////////////////////////////////////
protoSysHeartBeat::protoSysHeartBeat(uint32_t _actor)
{
	protoId = protoSysHeartBeat::IDD;
	protoLen = 0;
	EMPTY_STRING(extra_code);

}

int32_t protoSysHeartBeat::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_CHAR(extra_code, -1);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysHeartBeat::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;
	DESERIALIZE_CHAR(extra_code, -2);

	return (int32_t)offset;
}

int32_t protoSysHeartBeat::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+ LENGTH_STR(extra_code);
}

protoSysClientShutdown::protoSysClientShutdown(uint32_t _uin, uint32_t _reason)
{
	protoId = protoSysClientShutdown::IDD;
	protoLen = 0;
	uin = _uin;
	reason = _reason;
}

int32_t protoSysClientShutdown::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(uin, -1);
	SERIALIZE_DATA(reason, -2);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysClientShutdown::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(uin, -1);
	DESERIALIZE_DATA(reason, -2);
	return (int32_t)offset;
}

int32_t protoSysClientShutdown::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+ sizeof(uin)
		+ sizeof(reason);
}


protoSysSessionShutdown::protoSysSessionShutdown()
{
	protoId = IDD;
	protoLen = 0;
	actor = 0;
	svrId = 0;
	channelId = 0;
}

int32_t protoSysSessionShutdown::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(actor, -4);
	SERIALIZE_DATA(svrId, -5);
	SERIALIZE_DATA(channelId, -6);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysSessionShutdown::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(actor, -4);
	DESERIALIZE_DATA(svrId, -5);
	DESERIALIZE_DATA(channelId, -6);

	return (int32_t)offset;
}

int32_t protoSysSessionShutdown::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(actor)
		+sizeof(svrId)
		+sizeof(channelId);
}

//////////////////////////////////////////////////////////////////////////
protoSysRepostMsg::protoSysRepostMsg()
{
	protoId = IDD;
	protoLen = 0;
	
	msgId = 0;
	cbData = 0;
	pData = 0;
}

protoSysRepostMsg::~protoSysRepostMsg()
{
	if (NULL != pData)
	{
		free(pData);
		pData = NULL;
	}
}

int32_t protoSysRepostMsg::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(msgId, -4);
	SERIALIZE_DATA(cbData, -9);
	SERIALIZE_BUF(pData, cbData, -10);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoSysRepostMsg::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(msgId, -4);
	DESERIALIZE_DATA(cbData, -9);
	pData = calloc(1, cbData);
	DESERIALIZE_BUF(pData, cbData, -10);

	return (int32_t)offset;
}

int32_t protoSysRepostMsg::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(msgId)
		+sizeof(cbData)
		+cbData;
}

