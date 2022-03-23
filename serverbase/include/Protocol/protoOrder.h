/*
* =====================================================================================
*
*       Filename:  protoOrder.h
*
*    Description:  订单协议
*
*        Version:  1.0
*        Created:  2016年5月26日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_ORDER_H_
#define _PROTO_ORDER_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//订单信息请求
struct protoOrderInfoRequest : MSG_HEAD
{
	enum { IDD = ORDER_INFO_REQUEST };

	uint32_t uin;

	protoOrderInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct OrderInfo
{
	char item_name[256];
	char order_time[16];
	char status[32];
	uint32_t order_id;

	OrderInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//订单信息响应
struct protoOrderInfoRespone : MSG_HEAD
{
	enum { IDD = ORDER_INFO_RESPONE };

	vector<OrderInfo> vecOrderInfo;
	
	protoOrderInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


#endif //_PROTO_ORDER_H_

