/*
* =====================================================================================
*
*       Filename:  protoIdInfo.h
*
*    Description:  实名认证协议
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

#ifndef _PROTO_ID_INFO_H_
#define _PROTO_ID_INFO_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoQueryIdInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_IDINFO_REQUEST };

	uint32_t uin;

	protoQueryIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoQueryIdInfoRespone : MSG_HEAD
{
	enum { IDD = QUERY_IDINFO_RESPONE };

	char idNo[20];
	char name[32];
	uint32_t status;

	protoQueryIdInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/////////////////////////////////////////////////////////////////

struct protoSubmitIdInfoRequest : MSG_HEAD
{
	enum { IDD = SUBMIT_IDINFO_REQUEST };

	uint32_t uin;
	char idNo[20];
	char name[32];

	protoSubmitIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoSubmitIdInfoRespone : MSG_HEAD
{
	enum { IDD = SUBMIT_IDINFO_RESPONE };
	
	uint32_t result;
	uint32_t status;

	protoSubmitIdInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


////////////////////////////////////////////////////////////////////

struct protoLimitIdInfoRequest : MSG_HEAD
{
	enum { IDD = LIMIT_IDINFO_REQUEST };

	uint32_t uin;
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoLimitIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoLimitIdInfoRespone : MSG_HEAD
{
	enum { IDD = LIMIT_IDINFO_RESPONE };
	
	int32_t result;

	protoLimitIdInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


////////////////////////////////////////////////////////////////////

struct protoHallQueryIdInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_QUERY_IDINFO_REQUEST };

	uint32_t source;

	protoHallQueryIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoHallQueryIdInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_QUERY_IDINFO_RESPONSE };

	char idNo[20];
	char name[32];
	int32_t status;

	protoHallQueryIdInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/////////////////////////////////////////////////////////////////

struct protoHallSubmitIdInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_IDINFO_REQUEST };

	uint32_t uin;
	char idNo[20];
	char name[32];

	protoHallSubmitIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoHallSubmitIdInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_IDINFO_RESPONSE };

	uint32_t result;
	uint32_t status;

	protoHallSubmitIdInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////

struct protoHallLimitIdInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_LIMIT_IDINFO_REQUEST };

	uint32_t uin;
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoHallLimitIdInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoHallLimitIdInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_LIMIT_IDINFO_RESPONSE };

	int32_t result;

	protoHallLimitIdInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////

struct protoUserTotalOnlineRequest : MSG_HEAD
{
	enum { IDD = USER_TOTAL_ONLINE_REQUEST };

	uint32_t uin;

	protoUserTotalOnlineRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////

struct protoUserTotalOnlineNotify : MSG_HEAD
{
	enum { IDD = USER_TOTAL_ONLINE_NOTIFY };

	enum eStatus
	{
		OneHour = 1,			// 在线1小时
		TwoHour = 2,			// 在线2小时
		ThreeHour = 3,			// 在线3~5小时
		FiveHour = 4,			// 在线大于5小时
	};

	int32_t status;
	uint8_t take_off;			//0:不扣除， 1：扣除
	char reserve1[64];

	protoUserTotalOnlineNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////

struct protoSvrGainHalveNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_GAIN_HALVE_NOTIFY };

	enum eStatus
	{
		Halve = 1,			// 减半
		None = 2,			// 无收益
	};
	uint32_t uin;
	int32_t status;
	char reserve1[128];

	protoSvrGainHalveNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/////////////////////////////////////////////////////////////////////
struct protoSubmitIdInfoNotify : MSG_HEAD
{
    enum { IDD = PROTO_HALL_SUBMIT_IDINFO_NOTIFY };

    enum eStatus
    {
        eStatus_remind = 1,			// 提醒
        eStatus_timeover = 2,       // 超时
        eStatus_logout = 3,         // 登出
    };
    uint32_t status;

    protoSubmitIdInfoNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

struct protoRealNameSubmitReq : MSG_HEAD
{
	enum { IDD = PROTO_REALNAME_SUBMIT_REQ };

	uint32_t uin;
	char idNo[20];
	char name[32];
	uint32_t type;
	uint32_t source;
	
	protoRealNameSubmitReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoRealNameSubmitResp : MSG_HEAD
{
	enum { IDD = PROTO_REALNAME_SUBMIT_RESP };
	
	int32_t result;
	
	protoRealNameSubmitResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoRealNameLimitNotify : MSG_HEAD
{
	enum { IDD = PROTO_REALNAME_LIMIT_NOTIFY };
	
	uint32_t type;
	uint32_t value;
	
	protoRealNameLimitNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};
#endif //_PROTO_ID_INFO_H_

