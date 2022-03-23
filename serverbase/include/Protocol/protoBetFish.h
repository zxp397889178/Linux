#ifndef _QW_DXYM_PROTO_BETFISH_H_
#define _QW_DXYM_PROTO_BETFISH_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

using namespace std;

// 同步生成竞猜通知
struct protoSynBetFishStartNotify : MSG_HEAD
{
    enum { IDD = BETFISH_SYN_START_NOTIFY, };

    uint32_t fishId;
    uint32_t gainCoin;
    uint32_t totalCountdown;
    uint32_t roundCountdown;
    uint32_t resultCountdown;
    uint32_t baseCost;

    protoSynBetFishStartNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 同步竞猜激活通知
struct protoSynBetFishActive : MSG_HEAD
{
    enum { IDD = BETFISH_SYN_ACTIVE, };

    uint32_t uin;

    protoSynBetFishActive();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 发起竞猜消息请求
struct protoReqBetFish : MSG_HEAD
{
    enum { IDD = BETFISH_BET_REQUEST, };

    uint32_t type;

    protoReqBetFish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 发起竞猜消息请求应答
struct protoRspBetFish : MSG_HEAD
{
    enum { IDD = BETFISH_BET_RESPONSE, };

    uint32_t errCode;
    uint32_t result;
    uint32_t gainCoin;
    uint32_t nextCost;

    protoRspBetFish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 同步放弃竞猜通知
struct protoSynGiveupBetFish : MSG_HEAD
{
    enum { IDD = BETFISH_SYN_GIVEUP, };

    uint32_t uin;

    protoSynGiveupBetFish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 退出竞猜通知
struct protoSynQuitBetFish : MSG_HEAD
{
    enum { IDD = BETFISH_SYN_QUIT_NOTIFY, };

    uint32_t uin;

    protoSynQuitBetFish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif // end of _QW_DXYM_PROTO_BETFISH_H_
