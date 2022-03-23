#ifndef _QW_PROTO_REDPACKAGEFISH_H_
#define _QW_PROTO_REDPACKAGEFISH_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

using namespace std;

// 同步生成红包通知
struct protoSynRedPackageFishNotify : MSG_HEAD
{
    enum { IDD = REDPACKAGEFISH_SYN_START_NOTIFY, };

    uint32_t fishId;
    std::vector<uint32_t> points;
    uint32_t countDown;
    uint32_t gainCoin;
    uint32_t fishIndex;

    protoSynRedPackageFishNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 点击红包请求
struct protoReqClickRedPackage : MSG_HEAD
{
    enum { IDD = REDPACKAGEFISH_CLICK_REQUEST, };

    uint32_t fishIndex;
    uint32_t point;

    protoReqClickRedPackage();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 点击红包应答
struct protoRspClickRedPackage : MSG_HEAD
{
    enum { IDD = REDPACKAGEFISH_CLICK_RESPONSE, };

    uint32_t result;
    uint32_t point;
    uint32_t gainCoin;

    protoRspClickRedPackage();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 同步红包领取通知
struct protoSynRedPackageClickedNotify : MSG_HEAD
{
    enum { IDD = REDPACKAGEFISH_SYN_CLICKED_NOTIFY, };

    uint32_t points;

    protoSynRedPackageClickedNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif // end of _QW_PROTO_REDPACKAGEFISH_H_
