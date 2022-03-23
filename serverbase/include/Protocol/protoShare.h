/*
* =====================================================================================
*
*       Filename:  protoShare.h
*
*    Description:  share protocol
*
*        Version:  1.0
*        Created:  2016年5月18日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_SHARE_H_
#define _PROTO_SHARE_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//用户分享请求
struct protoUserShareRequest : MSG_HEAD
{
	enum { IDD = USER_SHARE_REQUEST };

	uint32_t uin;
	char channel[32];

	protoUserShareRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//用户分享应答
struct protoUserShareRespone : MSG_HEAD
{
	enum { IDD = USER_SHARE_RESPONE };

	uint32_t uin;
	
	protoUserShareRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



//用户接受分享请求
struct protoUserAcceptShareRequest : MSG_HEAD
{
	enum { IDD = USER_ACCEPT_SHARE_REQUEST };

	uint32_t uin;
	uint32_t share_uin;
	char channel[32];

	protoUserAcceptShareRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//用户接受分享应答
struct protoUserAcceptShareRespone : MSG_HEAD
{
	enum { IDD = USER_ACCEPT_SHARE_RESPONE };

	uint32_t uin;
	
	protoUserAcceptShareRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询分享信息请求
struct protoQueryShareInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_SHAREINFO_REQUEST };

	uint32_t uin;

	protoQueryShareInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询分享信息响应
struct protoQueryShareInfoResponse : MSG_HEAD
{
	enum { IDD = QUERY_SHAREINFO_RESPONE };

	uint32_t shareNum;
	uint32_t leftCanSharNum;
	uint32_t receivedCoin;
	uint32_t leftCanReceiveCoin;
	uint32_t reverse1;
	char reverse2[64];

	protoQueryShareInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> competsvr 用户接受分享请求
struct protoSvrAcceptShareRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ACCEPTSHARE_REQ };

	uint32_t accepterUin;
	uint32_t shareUin;
	char channel[32];
	char deviceId[64];
	char accepterNick[64];

	protoSvrAcceptShareRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr 用户今日分享信息请求
struct protoSvrShareInfoDailyRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SHAREINFODAILY_REQ };

	uint32_t uin;

	protoSvrShareInfoDailyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr 用户今日分享信息应答
struct protoSvrShareInfoDailyRespone : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SHAREINFODAILY_RESP };

	uint32_t uin;
	uint32_t shareNum;					// 已经分享个数
	uint32_t leftCanSharNum;			// 剩余可分享个数
	uint32_t receivedCoin;				// 已领取金币数
	uint32_t leftCanReceiveCoin;		// 剩余可领取金币数

	protoSvrShareInfoDailyRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询H5分享请求
struct protoQueryH5ShareRequest : MSG_HEAD
{
	enum { IDD = HALL_QUERY_H5_SHARE_REQUEST };

	uint32_t uin;

	protoQueryH5ShareRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询H5分享应答
struct protoQueryH5ShareResponse : MSG_HEAD
{
	enum { IDD = HALL_QUERY_H5_SHARE_RESPONSE };

	int32_t result;

	protoQueryH5ShareResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5分享请求
struct protoH5ShareRequest : MSG_HEAD
{
	enum { IDD = HALL_H5_SHARE_REQUEST };

	uint32_t uin;

	protoH5ShareRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5分享应答
struct protoH5ShareResponse : MSG_HEAD
{
	enum { IDD = HALL_H5_SHARE_RESPONSE };

	int32_t result;

	protoH5ShareResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//查询H5添加桌面请求
struct protoQueryH5AddDesktopRequest : MSG_HEAD
{
	enum { IDD = HALL_QUERY_H5_ADDDESKTOP_REQUEST };

	uint32_t uin;

	protoQueryH5AddDesktopRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询H5添加桌面应答
struct protoQueryH5AddDesktopResponse : MSG_HEAD
{
	enum { IDD = HALL_QUERY_H5_ADDDESKTOP_RESPONSE };

	int32_t result;

	protoQueryH5AddDesktopResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5添加桌面请求
struct protoH5AddDesktopRequest : MSG_HEAD
{
	enum { IDD = HALL_H5_ADDDESKTOP_REQUEST };

	uint32_t uin;

	protoH5AddDesktopRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5添加桌面应答
struct protoH5AddDesktopResponse : MSG_HEAD
{
	enum { IDD = HALL_H5_ADDDESKTOP_RESPONSE };

	int32_t result;

	protoH5AddDesktopResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5添加桌面请求
struct protoH5LoginGiftRequest : MSG_HEAD
{
	enum { IDD = HALL_H5_LOGINGIFT_REQUEST };

	uint32_t uin;
	uint32_t giftId;

	protoH5LoginGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//H5添加桌面应答
struct protoH5LoginGiftResponse : MSG_HEAD
{
	enum { IDD = HALL_H5_LOGINGIFT_RESPONSE };

	int32_t result;
	uint32_t giftId;

	protoH5LoginGiftResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //  _PROTO_SHARE_H_

