/*
* =====================================================================================
*
*       Filename:  protoCornucopia.h
*
*    Description:  聚宝盆 protocol
*
*        Version:  1.0
*        Created:  2015年12月1日 14时25分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_CORNUCOPIA_H_
#define _PROTO_CORNUCOPIA_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//聚宝盆奖励明细
struct CornucopiaReward
{
	uint32_t dataId;
	uint32_t coin;
	uint32_t status;

	CornucopiaReward();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//聚宝盆等级明细
struct CornucopiaLevel
{
	uint32_t level;
	uint32_t amount;

	CornucopiaLevel();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//查询聚宝盆请求
struct protoQueryCornucopiaRequest : MSG_HEAD
{
	enum { IDD = QUERY_CORNUCOPIA_REQUEST, };

	uint32_t uin;

	protoQueryCornucopiaRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//查询聚宝盆响应
struct protoQueryCornucopiaResponse : MSG_HEAD
{
	enum { IDD = QUERY_CORNUCOPIA_RESPONSE, };

	uint32_t id;
	char activityBeginTime[32];
	char activityEndTime[32];
	uint32_t amount;
	uint32_t lastLevel;
	char url[256];
	vector<CornucopiaReward> vecReward;
	vector<CornucopiaLevel> vecLevel;
	uint32_t reverse1;
	char reverse2[128];
	
	protoQueryCornucopiaResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////

//领取聚宝盆请求
struct protoReceiveCornucopiaRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_CORNUCOPIA_REQUEST, };

	uint32_t uin;
	uint32_t id;
	uint32_t dataId;

	protoReceiveCornucopiaRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//领取聚宝盆响应
struct protoReceiveCornucopiaResponse : MSG_HEAD
{
	enum { IDD = RECEIVE_CORNUCOPIA_RESPONSE, };

	uint32_t result;
	uint32_t coin;
	uint32_t reverse1;
	char reverse2[128];
	
	protoReceiveCornucopiaResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



#endif //  _PROTO_CORNUCOPIA_H_

