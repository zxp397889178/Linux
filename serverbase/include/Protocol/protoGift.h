/*
* =====================================================================================
*
*       Filename:  protoGift.h
*
*    Description:  礼包兑换码协议
*
*        Version:  1.0
*        Created:  2016年7月14日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_GIFT_H_
#define _PROTO_GIFT_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct RewardInfo
{
	uint32_t itemType;
	uint32_t itemId;
	uint32_t quantity;
	uint32_t extra;

	RewardInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoQueryGiftRequest : MSG_HEAD
{
	enum { IDD = QUERY_GIFT_REQUEST };

	uint32_t uin;
	char key[64];

	protoQueryGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoQueryGiftRespone : MSG_HEAD
{
	enum { IDD = QUERY_GIFT_RESPONE };

	int32_t status;
	vector<RewardInfo> vecRewardInfo;

	protoQueryGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/////////////////////////////////////////////////////////////////

struct protoUseGiftRequest : MSG_HEAD
{
	enum { IDD = USE_GIFT_REQUEST };

	uint32_t uin;
	char key[64];

	protoUseGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoUseGiftRespone : MSG_HEAD
{
	enum { IDD = USE_GIFT_RESPONE };
	
	uint32_t result;
	vector<RewardInfo> vecRewardInfo;

	protoUseGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct CurrencyData
{
    uint32_t type;
    uint32_t itemId;
    uint32_t num;

    CurrencyData();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 
struct protoGetNewUserGiftStatusRequest : MSG_HEAD
{
    enum { IDD = PROTO_NEWUSER_GIFT_STATUS_REQUEST, };

    uint32_t uin;

    protoGetNewUserGiftStatusRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoGetNewUserGiftStatusResponse : MSG_HEAD
{
    enum { IDD = PROTO_NEWUSER_GIFT_STATUS_RESPONSE, };

    int32_t result;
    std::vector<CurrencyData> giftInfo;

    protoGetNewUserGiftStatusResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoGetNewUserGiftRequest : MSG_HEAD
{
    enum { IDD = PROTO_GET_NEWUSER_GIFT_REQUEST, };

    uint32_t uin;

    protoGetNewUserGiftRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoGetNewUserGiftResponse : MSG_HEAD
{
    enum { IDD = PROTO_GET_NEWUSER_GIFT_RESPONSE, };

    int32_t result;

    protoGetNewUserGiftResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif //_PROTO_GIFT_H_

