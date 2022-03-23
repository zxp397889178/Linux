/*
* =====================================================================================
*
*       Filename:  protoSharpTimeGift.h
*
*    Description:  整点礼包协议
*
*        Version:  1.0
*        Created:  2016年5月19日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_SHARP_TIME_GIFT_H_
#define _PROTO_SHARP_TIME_GIFT_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//查询整点礼包请求
struct protoQuerySharpTimeGiftRequest : MSG_HEAD
{
	enum { IDD = QUERY_SHARPTIME_GIFT_REQUEST };

	uint32_t uin;

	protoQuerySharpTimeGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询整点礼包应答
struct protoQuerySharpTimeGiftRespone : MSG_HEAD
{
	enum { IDD = QUERY_SHARPTIME_GIFT_RESPONE };

	uint32_t act_key;
	int32_t status;
	
	protoQuerySharpTimeGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



//领取整点礼包请求
struct protoReceiveSharpTimeGiftRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_SHARPTIME_GIFT_REQUEST };

	uint32_t uin;

	protoReceiveSharpTimeGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//领取整点礼包应答
struct protoReceiveSharpTimeGiftRespone : MSG_HEAD
{
	enum { IDD = RECEIVE_SHARPTIME_GIFT_RESPONE };

	int32_t result;
	
	protoReceiveSharpTimeGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //  _PROTO_SHARP_TIME_GIFT_H_

