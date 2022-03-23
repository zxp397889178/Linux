#ifndef _QW_DXYM_PROTO_FISHING_H_
#define _QW_DXYM_PROTO_FISHING_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"


using namespace std;

// 
struct LimitData
{
    uint32_t type;
    uint32_t itemId;
    uint32_t minNum;
    uint32_t maxNum;
    uint32_t consumeNum;

    LimitData();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 
struct FishingRoomInfo
{
    uint32_t roomId;
    char roomName[64];
    std::vector<LimitData> limit;

    FishingRoomInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 
struct FisherData
{
    uint32_t uin;
    char name[64];
    uint32_t seatid;
    uint32_t fishingRod;

    FisherData();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 
struct FishingRodData
{
    uint32_t fishingRodId;
    uint32_t durability;
    uint32_t price;

    FishingRodData();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 钓鱼人数同步
struct protoFishingPlayerCountNotify : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_PLAYER_COUNT_NOTIFY, };

    uint32_t count;

    protoFishingPlayerCountNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 钓鱼房间列表请求
struct protoFishingRoomlistRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ROOMLIST_REQUEST, };

    uint32_t uin;

    protoFishingRoomlistRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 钓鱼房间列表请求响应
struct protoFishingRoomlistResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ROOMLIST_RESPONSE, };

    std::vector<FishingRoomInfo> roomList;

    protoFishingRoomlistResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 进入钓鱼房间请求
struct protoFishingEnterRoomRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ENTER_ROOM_REQUEST, };

    uint32_t uin;
    uint32_t roomId;

    protoFishingEnterRoomRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 进入钓鱼房间请求响应
struct protoFishingEnterRoomResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ENTER_ROOM_RESPONSE, };

    uint32_t result;
    uint32_t roomId;
    uint32_t fishingTime;
    uint32_t durability;
    std::vector<uint32_t> fishingBait;
    std::vector<FisherData> userData;

    protoFishingEnterRoomResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 进入钓鱼房间消息同步
struct protoFishingEnterRoomNotify : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ENTER_ROOM_NOTIFY, };

    uint32_t uin;
    char name[64];
    uint32_t seatid;
    uint32_t fishingRod;

    protoFishingEnterRoomNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 甩杆请求
struct protoFishingAngleRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ANGLE_REQUEST, };

    uint32_t uin;
    uint32_t fishingBait;

    protoFishingAngleRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 甩杆请求响应
struct protoFishingAngleResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ANGLE_RESPONSE, };

    uint32_t result;
    uint32_t getCoin;
    uint32_t fishId;
    uint32_t fishValue;
    uint32_t durability;

    protoFishingAngleResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 甩杆消息同步
struct protoFishingAngleNotify : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ANGLE_NOTIFY, };

    uint32_t uin;
    uint32_t fishId;

    protoFishingAngleNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 鱼竿列表请求
struct protoFishingRodListRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ROD_LIST_REQUEST, };

    uint32_t uin;

    protoFishingRodListRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 鱼竿列表请求响应
struct protoFishingRodListResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_ROD_LIST_RESPONSE, };

    std::vector<FishingRodData> ownList;
    std::vector<FishingRodData> sellList;

    protoFishingRodListResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 更换鱼竿请求
struct protoFishingChangeRodRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_CHANGE_ROD_REQUEST, };

    uint32_t uin;
    uint32_t fishingRodId;

    protoFishingChangeRodRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 更换鱼竿请求响应
struct protoFishingChangeRodResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_CHANGE_ROD_RESPONSE, };

    uint32_t result;
    uint32_t fishingRodId;
    uint32_t durability;

    protoFishingChangeRodResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 更换鱼竿消息同步
struct protoFishingChangeRodNotify : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_CHANGE_ROD_NOTIFY, };

    uint32_t uin;
    uint32_t fishingRodId;

    protoFishingChangeRodNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 购买鱼竿请求
struct protoFishingBuyRodRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_BUY_ROD_REQUEST, };

    uint32_t uin;
    uint32_t fishingRodId;
    uint32_t num;

    protoFishingBuyRodRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 购买鱼竿请求请求响应
struct protoFishingBuyRodResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_BUY_ROD_RESPONSE, };

    uint32_t result;
    uint32_t fishingRodId;
    uint32_t num;

    protoFishingBuyRodResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 离开钓鱼房间请求
struct protoFishingLeaveRoomRequest : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_LEAVE_ROOM_REQUEST, };

    uint32_t uin;
    uint32_t roomId;

    protoFishingLeaveRoomRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 离开钓鱼房间请求响应
struct protoFishingLeaveRoomResponse : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_LEAVE_ROOM_RESPONSE, };

    uint32_t result;

    protoFishingLeaveRoomResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 离开钓鱼房间消息同步
struct protoFishingLeaveRoomNotify : MSG_HEAD
{
    enum { IDD = PROTO_FISHING_LEAVE_ROOM_NOTIFY, };

    uint32_t uin;

    protoFishingLeaveRoomNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif // end of _QW_DXYM_PROTO_FISHING_H_
