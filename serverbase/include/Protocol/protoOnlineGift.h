/*
* =====================================================================================
*
*       Filename:  protoOnlineGift.h
*
*    Description:  在线礼包协议
*
*        Version:  1.0
*        Created:  2015年10月13日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_ONLINEGIFTPROTOCOL_H_
#define _GFX_PROXY_ONLINEGIFTPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoOnlineTimeRequest : MSG_HEAD
{
	enum { IDD = ONLINETIMEREQUEST, };
	uint32_t		uin;

	protoOnlineTimeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoOnlineTimeRespone : MSG_HEAD
{
	enum { IDD = ONLINETIMERESPONE, };
	uint32_t		act_key;
	uint32_t		online_time;

	protoOnlineTimeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoGetOnlineGiftRequest : MSG_HEAD
{
	enum { IDD = GETONLINEGIFTREQUEST, };
	uint32_t		uin;
	uint32_t		act_key;

	protoGetOnlineGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

#endif	//_GFX_PROXY_ONLINEGIFTPROTOCOL_H_
