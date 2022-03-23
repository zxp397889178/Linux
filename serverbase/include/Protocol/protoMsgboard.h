#ifndef _QW_PROTOCOL_MSGBOARD_H_
#define _QW_PROTOCOL_MSGBOARD_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

using namespace std;

// 留言板信息
struct MsgBoardInfo
{
    uint32_t id;
    uint32_t uin;
    char nick[64];
    char face[128];
    uint32_t vip;
    char content[128];
    uint32_t publishTime;

    MsgBoardInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 获取留言板信息请求
struct protoReqMsgBoardInfo : MSG_HEAD
{
    enum { IDD = PROTO_REQ_MSG_BOARD_INFO, };

    uint32_t uin;
    uint32_t index;
    uint32_t count;

    protoReqMsgBoardInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取留言板信息请求应答
struct protoRspMsgBoardInfo : MSG_HEAD
{
    enum { IDD = PROTO_RSP_MSG_BOARD_INFO, };

    std::vector<MsgBoardInfo> msgBoardInfo;

    protoRspMsgBoardInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 发布信息请求
struct protoReqPublishMsg : MSG_HEAD
{
    enum { IDD = PROTO_REQ_PUBLISH_MSG, };

    uint32_t uin;
    char nick[64];
    char face[128];
    uint32_t vip;
    char content[128];

    protoReqPublishMsg();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 发布信息请求应答
struct protoRspPublishMsg : MSG_HEAD
{
    enum { IDD = PROTO_RSP_PUBLISH_MSG, };

    uint32_t result;

    protoRspPublishMsg();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 新信息通知
struct protoSynNewMsgNotify : MSG_HEAD
{
    enum { IDD = PROTO_SYN_NEW_MSG_NOTIFY, };

    uint32_t count;

    protoSynNewMsgNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 用户退出留言板通知
struct protoSynUserQuitMsgBoardNotify : MSG_HEAD
{
    enum { IDD = PROTO_SYN_USER_QUIT_MSG_BOARD_NOTIFY, };

    uint32_t uin;

    protoSynUserQuitMsgBoardNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif