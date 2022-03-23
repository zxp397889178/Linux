/*
* =====================================================================================
*
*       Filename:  protoTreasure.h
*
*    Description:  欢乐夺宝协议
*
*        Version:  1.0
*        Created:  2015年10月06日 10时40分31秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
*   Organization:  www.skyunion.net(IGG)
*
* =====================================================================================
*/

#ifndef _QW_CRAZYFISH_PROTO_TREASURE_H_
#define _QW_CRAZYFISH_PROTO_TREASURE_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoDef.h"

// 查询夺宝详情
struct protoTreasureDetailRequest : MSG_HEAD
{
	enum { IDD = TREASURE_DETAIL_REQUEST, };

	uint32_t		nUin;

	protoTreasureDetailRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct  ChestItem
{
	uint32_t nIndex;
	uint32_t nId;
	uint32_t nNumber;
	uint32_t nStatus;

	ChestItem();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct ChestData
{
	uint32_t nChestId;
	uint32_t nChestType;
	char strName[256];
	uint32_t nItemTypeId;
	uint32_t nNumber;
	uint32_t nProbability;
	uint32_t nLimit;
	uint32_t nMustProduct;
	uint32_t nItemPreview;
	uint32_t nStatus;
	uint32_t nGlobalProduct;
	uint32_t nNoticeLog;
	uint32_t nBroadcastLog;
};
// 夺宝详情应答
struct protoTreasureDetailResponse : MSG_HEAD
{
	enum  { IDD = TREASURE_DETAIL_RESPONSE, };

	int32_t	  nResult;									//查询结果，0为成功，-1活动不存在
	uint32_t nTreasureId;								//寻宝活动ID
	uint32_t nScore;									//用户积分
	uint32_t nOpenAllScore;						//打开所有宝箱需要的积分
	uint32_t nResetScore;							//打开所有宝箱需要的积分
	uint32_t nCostScore;								//打开一个宝箱需要的积分
	char szIconUrl[256];								// 宝箱图片辍
	char szUrl[256];										// 活动详情地址

	vector<ChestItem> vecChestList;			// 已开宝箱列表
	vector<ChestItem> vecPreChestList;		// 宝箱预览列表

	protoTreasureDetailResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询夺宝公告信息
struct protoTreasureNoticeRequest : MSG_HEAD
{
	enum { IDD = TREASURE_NOTICE_REQUEST, };

	uint32_t	nUin;
	uint32_t	nTreasureId;								//寻宝活动ID
	uint32_t	nLastIndex;				//0为第一批，其它为用户当前取得记录的最小值

	protoTreasureNoticeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct TreasureNotice
{
	uint32_t nIndex;						//查询的记录索引
	uint32_t nTime;							//时间
	char szAccount[64];
	char szContent[128];

	TreasureNotice();
	int32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 夺宝公告应答
struct protoTreasureNoticeResponse : MSG_HEAD
{
	enum  { IDD = TREASURE_NOTICE_RESPONSE, };
	uint32_t nTreasureId;								//寻宝活动ID
	vector<TreasureNotice> vecNoticeList;

	protoTreasureNoticeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 打开宝箱
struct protoTreasureOpenRequest : MSG_HEAD
{
	enum { IDD = TREASURE_OPEN_REQUEST, };

	uint32_t		nUin;
	uint32_t		nTreasureId;								//寻宝活动ID
	uint8_t			nOpenType;						//打开类型，1：单个宝箱，2：所有宝箱
	uint32_t		nIndex;								//格子ID,打开单个宝箱时有效,从0开始



	protoTreasureOpenRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 打开宝箱应答
struct protoTreasureOpenResponse : MSG_HEAD
{
	enum  { IDD = TREASURE_OPEN_RESPONSE, };

	int32_t			nResult;
	uint32_t		nTreasureId;								//寻宝活动ID
	uint8_t			nOpenType;			//打开类型，1：单个宝箱，2：所有宝箱
	uint32_t		nIndex;				//宝箱ID,打开单个宝箱时有效
	vector<ChestItem> vecChestList;			// 已开宝箱列表   

	protoTreasureOpenResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 重置所有宝箱
struct protoTreasureResetRequest : MSG_HEAD
{
	enum { IDD = TREASURE_RESET_REQUEST, };

	uint32_t		nUin;
	uint32_t		nTreasureId;								//寻宝活动ID
	
	protoTreasureResetRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 重置所有宝箱应答
struct protoTreasureResetResponse : MSG_HEAD
{
	enum  { IDD = TREASURE_RESET_RESPONSE, };
	int32_t nResult;
	uint32_t nTreasureId;								//寻宝活动ID

	protoTreasureResetResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif // _QW_CRAZYFISH_PROTO_TREASURE_H_


