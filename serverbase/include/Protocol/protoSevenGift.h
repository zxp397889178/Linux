/*
* =====================================================================================
*
*       Filename:  protoSevenGift.h
*
*    Description:  七日礼包协议
*
*        Version:  1.0
*        Created:  2015年10月9日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_SEVENGIFTPROTOCOL_H_
#define _GFX_PROXY_SEVENGIFTPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoSevenGiftRequest : MSG_HEAD
{
	enum { IDD = SEVENGIFTREQUEST, };
	uint32_t		uin;

	protoSevenGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoSevenGiftRespone : MSG_HEAD
{
	enum { IDD = SEVENGIFTRESPONE, };
	uint32_t		day;
	uint32_t		flag;

	protoSevenGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoGetSevenGiftRequest : MSG_HEAD
{
	enum { IDD = GETSEVENGIFTREQUEST, };
	uint32_t		uin;

	protoGetSevenGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};



#endif	//_GFX_PROXY_SEVENGIFTPROTOCOL_H_
