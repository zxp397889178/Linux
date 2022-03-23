/*
* =====================================================================================
*
*       Filename:  protoDataReload.h
*
*    Description:  data reload protocol
*
*        Version:  1.0
*        Created:  2016年5月23日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_DATA_RELOAD_H_
#define _PROTO_DATA_RELOAD_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//数据重新装载请求
struct protoDataReloadRequest : MSG_HEAD
{
	enum { IDD = DATARELOADREQUEST };

	char dictname[1024];

	protoDataReloadRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//停服通知
struct protoStopServerNotice : MSG_HEAD
{
	enum { IDD = STOP_SERVER_NOTICE };

	enum eType
	{
		StopNotify = 0,			// 停服通知
		CountDownNotify = 1,	// 倒计时通知
	};
	uint32_t type;
	char stopTime[64];
	uint32_t leftTime;

	protoStopServerNotice();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //  _PROTO_DATA_RELOAD_H_

