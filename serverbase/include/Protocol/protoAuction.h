#ifndef _QW_DXYM_PROTO_AUCTION_H_
#define _QW_DXYM_PROTO_AUCTION_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"


using namespace std;

// 竞购物品信息
struct AuctionItemInfo
{
    uint32_t index; // 流水号
    uint32_t itemId; // 商品ID
    uint32_t periods; // 期数
    uint32_t totalShare; // 总份额
    uint32_t singleShare; // 单人限购份额
    uint32_t currencyType; // 竞购货币类型
    uint32_t price; // 竞购货币数额
    uint32_t currentShare; // 当前已购份额
    uint32_t userShare; // 该玩家购买的份额
    uint32_t status; // 商品状态:0-竞购中 1-揭晓中 2-已下架
    uint32_t countDown; // 揭晓状态倒计时

    AuctionItemInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 竞购物品份额信息同步
struct AuctionShareNotify
{
    uint32_t index; // 流水号
    uint32_t itemId; // 商品ID
    uint32_t periods; // 期数
    uint32_t currentShare; // 当前已购份额
    uint32_t userShare; // 该玩家购买的份额
    uint32_t status; // 商品状态:0-竞购中 1-揭晓中 2-已下架
    uint32_t countDown; // 揭晓状态倒计时

    AuctionShareNotify();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 开奖信息
struct AuctionPublishInfo
{
    uint32_t index; // 流水号
    uint32_t itemId; // 商品ID
    uint32_t periods; // 期数
    uint32_t totalShare; // 总份额
    uint32_t userShare; // 该玩家购买的份额
    uint32_t winnerShare; // 中奖玩家购买的份额
    char winnerName[64]; // 中奖玩家名称
    char publishTime[32]; // 揭晓时间
    uint32_t bWin; // 是否中奖
    uint32_t countDown; // 揭晓状态倒计时

    AuctionPublishInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 获取竞购商品信息请求
struct protoReqAuctionItemInfo : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONITEMINFO, };

    uint32_t uin;

    protoReqAuctionItemInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取竞购商品信息请求应答
struct protoRspAuctionItemInfo : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONITEMINFO, };

    std::vector<AuctionItemInfo> auctionItemInfo;

    protoRspAuctionItemInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 竞购商品请求
struct protoReqAuctionShare : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONSHARE, };

    uint32_t itemId; // 商品ID
    uint32_t periods; // 期数
    uint32_t shareCount; // 竞购份额

    protoReqAuctionShare();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 竞购商品请求应答
struct protoRspAuctionShare : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONSHARE, };

    uint32_t result; // 竞购结果
    std::vector<AuctionShareNotify> auctionShareNotify;

    protoRspAuctionShare();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 竞购商品份额信息通知
struct protoSynAuctionShareInfoNotify : MSG_HEAD
{
    enum { IDD = PROTO_SYN_AUCTIONSHAREINFONOTIFY, };

    std::vector<AuctionShareNotify> auctionShareNotify;

    protoSynAuctionShareInfoNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取竞购商品记录（进行中）请求
struct protoReqAuctionOnGoing : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONONGOING, };

    uint32_t uin;
    uint32_t indexBegin;

    protoReqAuctionOnGoing();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取竞购商品记录（进行中）请求应答
struct protoRspAuctionOnGoing : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONONGOING, };

    std::vector<AuctionShareNotify> auctionShareNotify;

    protoRspAuctionOnGoing();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取竞购商品记录（已开奖,中奖记录）请求
struct protoReqAuctionPublish : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONPUBLISH, };

    uint32_t uin;
    uint32_t indexBegin;
    uint32_t type;

    protoReqAuctionPublish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取竞购商品记录（已开奖,中奖记录）请求应答
struct protoRspAuctionPublish : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONPUBLISH, };

    std::vector<AuctionPublishInfo> auctionPublishInfo;
    uint32_t type;

    protoRspAuctionPublish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取最新揭晓请求
struct protoReqAuctionLastPublish : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONLASTPUBLISH, };

    uint32_t uin;
    uint32_t indexBegin;

    protoReqAuctionLastPublish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取最新揭晓请求应答
struct protoRspAuctionLastPublish : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONLASTPUBLISH, };

    std::vector<AuctionPublishInfo> auctionPublishInfo;

    protoRspAuctionLastPublish();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取往期揭晓请求
struct protoReqAuctionHistory : MSG_HEAD
{
    enum { IDD = PROTO_REQ_AUCTIONHISTORY, };

    uint32_t itemId;
    uint32_t indexBegin;

    protoReqAuctionHistory();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取往期揭晓请求应答
struct protoRspAuctionHistory : MSG_HEAD
{
    enum { IDD = PROTO_RSP_AUCTIONHISTORY, };

    std::vector<AuctionPublishInfo> auctionPublishInfo;

    protoRspAuctionHistory();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 设置收货地址请求
struct protoReqSetDeliveryAddress : MSG_HEAD
{
    enum { IDD = PROTO_REQ_SETDELIVERYADDRESS, };

    char phoneNum[32]; // 电话号码
    char postCode[128]; // 地址
    char address[128]; // 地址
    char name[32]; // 姓名

    protoReqSetDeliveryAddress();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 设置收货地址请求应答
struct protoRspSetDeliveryAddress : MSG_HEAD
{
    enum { IDD = PROTO_RSP_SETDELIVERYADDRESS, };

    uint32_t result;

    protoRspSetDeliveryAddress();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 用户退出一元购通知
struct protoSynUserQuitAuctionNotify : MSG_HEAD
{
    enum { IDD = PROTO_SYN_USERQUITAUCTIONNOTIFY, };

    uint32_t uin;

    protoSynUserQuitAuctionNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 开奖信息通知
struct protoSynAuctionPublishInfoNotify : MSG_HEAD
{
    enum { IDD = PROTO_SYN_AUCTIONPUBLISHINFONOTIFY, };

    std::vector<AuctionPublishInfo> auctionPublishInfo;

    protoSynAuctionPublishInfoNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取收货地址请求
struct protoReqGetDeliveryAddress : MSG_HEAD
{
    enum { IDD = PROTO_REQ_GETDELIVERYADDRESS, };

    uint32_t uin;

    protoReqGetDeliveryAddress();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取收货地址请求应答
struct protoRspGetDeliveryAddress : MSG_HEAD
{
    enum { IDD = PROTO_RSP_GETDELIVERYADDRESS, };

    char phoneNum[32]; // 电话号码
    char postCode[128]; // 地址
    char address[128]; // 地址
    char name[32]; // 姓名

    protoRspGetDeliveryAddress();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};
// 获取一元购规则请求
struct protoReqGetAuctionRule : MSG_HEAD
{
    enum { IDD = PROTO_REQ_GETAUCTIONRULE, };

    uint32_t uin;

    protoReqGetAuctionRule();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 获取一元购规则请求应答
struct protoRspGetAuctionRule : MSG_HEAD
{
    enum { IDD = PROTO_RSP_GETAUCTIONRULE, };

    char rule[1024];

    protoRspGetAuctionRule();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};
#endif // end of _QW_DXYM_PROTO_AUCTION_H_
