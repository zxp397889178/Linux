/*
* =====================================================================================
*
*       Filename:  protoWealth.h
*
*    Description:  玩家财富相关协议
*
*        Version:  1.0
*        Created:  2018年03月09日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_FISHREPORT_H_
#define _QW_PROTO_FISHREPORT_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoDef.h"
#include <vector>
using namespace std;

struct FISH_MATCH
{
	char MatchId[32];
	uint32_t nCost;
	uint32_t nSocre;
	int32_t nGain;
	uint32_t nEnterRoom;
	uint32_t nOutRoom;

	FISH_MATCH();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct FISH_MATCH_V2
{
	char MatchId[32];
	uint64_t nCost;
	uint64_t nSocre;
	int64_t nGain;
	uint32_t nEnterRoom;
	uint32_t nOutRoom;

	FISH_MATCH_V2();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


//捕获鱼种信息
struct MATCH_FISH_INFO
{
	uint32_t nFishId;
	uint32_t nNumber;

	MATCH_FISH_INFO();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 捕鱼结果请求
struct protoFishMatchResultRequest : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_RESULT_REQUEST, };

	uint32_t nUin;
	uint32_t nPage;

	protoFishMatchResultRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 捕鱼结果响应
struct protoFishMatchResultResponse : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_RESULT_RESPONSE, };

	uint32_t nUin;
	uint32_t nPage;
	uint32_t nTotalPage;
	uint32_t nTotalCost;
	uint32_t nTotalSocre;

	vector<FISH_MATCH> vecMatchInfo;

	protoFishMatchResultResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 捕鱼结果请求V2
struct protoFishMatchResultRequestV2 : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_RESULT_REQUESTV2, };

	uint32_t nUin;
	uint32_t nPage;

	protoFishMatchResultRequestV2();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 捕鱼结果响应V2
struct protoFishMatchResultResponseV2 : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_RESULT_RESPONSEV2, };

	uint32_t nUin;
	uint32_t nPage;
	uint32_t nTotalPage;
	uint64_t nTotalCost;
	uint64_t nTotalSocre;

	vector<FISH_MATCH_V2> vecMatchInfo;

	int64_t nTotalGain;

	protoFishMatchResultResponseV2();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 捕鱼结果请求
struct protoFishMatchInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_INFO_REQUEST, };

	uint32_t nUin;
	char MatchId[32];

	protoFishMatchInfoRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 捕鱼结果响应
struct protoFishMatchInfoResponse : MSG_HEAD
{
	enum { IDD = PROTO_FISH_MATCH_INFO_RESPONSE, };

	uint32_t nUin;
	char MatchId[32];

	vector<MATCH_FISH_INFO> vecFishInfo;

	uint32_t taxRate;
	uint32_t tax;

	protoFishMatchInfoResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif	// _QW_PROTO_WEALTH_H_
