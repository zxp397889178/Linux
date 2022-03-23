/*
* =====================================================================================
*
*       Filename:  protoAlms.h
*
*    Description:  救济金协议
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

#ifndef _GFX_PROXY_ALMSPROTOCOL_H_
#define _GFX_PROXY_ALMSPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoAccountAlmsInfoRequest : MSG_HEAD
{
	enum { IDD = ACCOUNTALMSINFOREQUEST, };
	uint32_t		uin;

	protoAccountAlmsInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoAccountAlmsInfoRespone : MSG_HEAD
{
	enum { IDD = ACCOUNTALMSINFORESPONE, };
	uint32_t		alms;
	uint32_t		coin;

	protoAccountAlmsInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoAccountAlmsRequest : MSG_HEAD
{
	enum { IDD = ACCOUNTALMSREQUEST, };
	uint32_t		uin;

	protoAccountAlmsRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoAccountAlmsRespone : MSG_HEAD
{
	enum { IDD = ACCOUNTALMSRESPONE, };
	uint32_t		alms;
	uint32_t		coin;

	protoAccountAlmsRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


#endif	//_GFX_PROXY_SEVENGIFTPROTOCOL_H_
