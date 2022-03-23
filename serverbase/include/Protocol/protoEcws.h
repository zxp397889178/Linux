/*
* =====================================================================================
*
*       Filename:  protoEcws.h
*
*    Description:  以太坊协议
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


#ifndef _QW_PROTOCOL_ECWS_H_
#define _QW_PROTOCOL_ECWS_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

/*以太坊地址请求*/
struct protoQueryUserEcwsAddressRequest : MSG_HEAD
{
	enum { IDD = QUERY_USER_ECWS_ADDRESS_REQUEST, };
	uint32_t		uin;

	protoQueryUserEcwsAddressRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoQueryUserEcwsAddressResponse : MSG_HEAD
{
	enum { IDD = QUERY_USER_ECWS_ADDRESS_RESPONSE, };
	char address[64];

	protoQueryUserEcwsAddressResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


/*提现请求*/
struct protoEcwsWithDrawRequest : MSG_HEAD
{
	enum { IDD = ECWS_WITHDRAW_REQUEST, };
	uint32_t uin;
	uint32_t coin;
	char address[128];

	protoEcwsWithDrawRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoEcwsWithDrawResponse : MSG_HEAD
{
	enum { IDD = ECWS_WITHDRAW_RESPONSE, };
	int32_t result;

	protoEcwsWithDrawResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


/*兑换配置请求*/
struct protoDictEosRequest : MSG_HEAD
{
	enum { IDD = DICT_EOS_REQUEST, };
	uint32_t uin;

	protoDictEosRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct DictEosInfo
{
	uint32_t type;
	char param1[32];
	char param2[32];

	DictEosInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoDictEosResponse : MSG_HEAD
{
	enum { IDD = DICT_EOS_RESPONSE, };
	std::vector<DictEosInfo> infoList;

	protoDictEosResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


/*以太坊每日交易限制请求*/
struct protoEcwsLimitDailyRequest : MSG_HEAD
{
	enum { IDD = ECWS_LIMIT_DAILY_REQUEST, };
	uint32_t		uin;

	protoEcwsLimitDailyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoEcwsLimitDailyResponse : MSG_HEAD
{
	enum { IDD = ECWS_LIMIT_DAILY_RESPONSE, };
	uint32_t canWithDrawCoinToday;

	protoEcwsLimitDailyResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


/*以太坊交易记录查询请求*/
struct protoQueryEcwsRecordRequest : MSG_HEAD
{
	enum { IDD = QUERY_ECWS_RECORD_REQUEST, };
	uint32_t uin;
	uint32_t type;

	protoQueryEcwsRecordRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct EcwsRecordInfo
{
	uint32_t coin;
	double eth;
	uint64_t time;

	EcwsRecordInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoQueryEcwsRecordResponse : MSG_HEAD
{
	enum { IDD = QUERY_ECWS_RECORD_RESPONSE, };
	uint32_t type;
	std::vector<EcwsRecordInfo> infoList;

	protoQueryEcwsRecordResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};



#endif	// _QW_PROTOCOL_ECWS_H_
