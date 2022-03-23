/*
* =====================================================================================
*
*       Filename:  protoTableServer.h
*
*    Description:  桌子服务器协议
*
*        Version:  1.0
*        Created:  2016年3月25日 15时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_TABLESERVER_H_
#define _QW_PROTOCOL_TABLESERVER_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
#include <map>


struct SeatInfo
{
	uint8_t seatId;
	uint32_t uin;
	uint32_t coin;
	char nick[32];
	SeatInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};
typedef SeatInfo ProtoSeatInfo;
typedef std::map<uint32_t, std::vector<SeatInfo> > TABLESEAT_MAP;		// 桌子id->座位信息

// 开启桌子请求
struct protoTableOpenRequest : MSG_HEAD
{
	enum { IDD = PROTO_TABLEOPEN_REQUEST, };
	std::vector<uint32_t> vecTableId;
	uint32_t roomId;

	protoTableOpenRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 开启桌子应答
struct protoTableOpenRespone : MSG_HEAD
{
	enum { IDD = PROTO_TABLEOPEN_RESPONE, };

	TABLESEAT_MAP mapTableSeat;
	uint32_t roomId;

	protoTableOpenRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 桌子列表请求
struct protoTableListRequest : MSG_HEAD
{
	enum { IDD = PROTO_TABLELIST_REQUEST, };
	uint32_t uin;
	uint32_t roomId;

	protoTableListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 桌子列表应答
struct protoTableListRespone : MSG_HEAD
{
	enum { IDD = PROTO_TABLELIST_RESPONE, };
	uint32_t uin;
	TABLESEAT_MAP mapTableSeat;
	uint32_t roomId;

	protoTableListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 随机座位坐下请求
struct protoSeatDownRandomRequest : MSG_HEAD
{
	enum { IDD = PROTO_SEATDOWNRANDOM_REQUEST, };
	uint32_t uin;
	char nick[32];
	uint32_t coin;
	uint32_t roomId;

	protoSeatDownRandomRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 指定座位坐下请求
struct protoSeatDownSpecifyRequest : MSG_HEAD
{
	enum { IDD = PROTO_SEATDOWNSPECIFY_REQUEST, };
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;
	char nick[32];
	uint32_t coin;
	uint32_t roomId;

	protoSeatDownSpecifyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 坐下应答
struct protoSeatDownRespone : MSG_HEAD
{
	enum { IDD = PROTO_SEATDOWN_RESPONE, };
	enum Result
	{
		Success = 0,			// 成功
		TableDisabled = -1,		// 桌子不可用
		SeatDisabled = -2,		// 座位不可用
	};

	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;
	uint32_t gamesvrId;
	int32_t result;
	uint32_t roomId;

	protoSeatDownRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 离开桌子请求
struct protoLeaveTableNotify : MSG_HEAD
{
	enum { IDD = PROTO_LEAVETABLE_NOTIFY, };
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;
	uint32_t roomId;

	protoLeaveTableNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 同步座位信息请求
struct protoSyncSeatInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_SYNCSEATINFO_REQUEST, };
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;
	uint32_t roomId;

	protoSyncSeatInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 同步座位信息应答
struct protoSyncSeatInfoRespone : MSG_HEAD
{
	enum { IDD = PROTO_SYNCSEATINFO_RESPONE, };
	enum Result
	{
		Success = 0,		// 成功
		Failure = -1,		// 失败
	};
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;
	int32_t result;
	uint32_t coin;
	uint32_t roomId;

	protoSyncSeatInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 改变座位请求
struct protoSvrChangeSeatRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_CHANGESEAT_REQUEST, };
	uint32_t uin;
	uint32_t roomId;
	uint32_t tableId;
	uint8_t seatId;

	protoSvrChangeSeatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 改变座位应答
struct protoSvrChangeSeatRespone : MSG_HEAD
{
	enum { IDD = PROTO_SVR_CHANGESEAT_RESPONE, };
	enum Result
	{
		Success = 0,		// 成功
		Failure = -1,		// 失败 
	};
	uint32_t uin;
	uint32_t roomId;
	uint32_t tableId;
	uint8_t seatId;
	int32_t result;

	protoSvrChangeSeatRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif	// _QW_PROTOCOL_TABLESERVER_H_
