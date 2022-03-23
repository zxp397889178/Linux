#ifndef _QW_DXYM_PROTO_FORESTPARTY_H_
#define _QW_DXYM_PROTO_FORESTPARTY_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"


using namespace std;

// 入场限制信息
struct PartyCoinLimitInfo
{
    uint32_t roomId;
    uint32_t coinLimit;
    uint32_t roomNameKey;

    PartyCoinLimitInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 赔率信息
struct PartyOddsInfo
{
    uint32_t animal;
    uint32_t color;
    uint32_t odds;

    PartyOddsInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 房间配置信息
struct PartyRoomCfgInfo
{
    uint32_t roomId;
    char animal[64];
    std::vector<PartyOddsInfo> odds;
    std::vector<uint32_t> chips;
    uint32_t bettingTime;
    uint32_t lotteryTime;

    PartyRoomCfgInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 抽奖结果信息
struct PartyLotteryResInfo
{
    uint32_t animal;
    uint32_t color;

    PartyLotteryResInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 玩家投注信息
struct UserBettingInfo
{
    uint32_t animal;
    uint32_t color;
    uint32_t chip;

    UserBettingInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 玩家投注信息
struct PartyUserBettingInfo
{
    char face[256];
    char nick[64];
    uint32_t vip;
    uint32_t coin;
    uint32_t bettingCoin;

    PartyUserBettingInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 玩家中奖信息
struct PartyUserWinningInfo
{
    char face[256];
    char nick[64];
    uint32_t vip;
    uint32_t coin;
    uint32_t winningCoin;

    PartyUserWinningInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 投注信息
struct PartyBettingInfo
{
    uint32_t animal;
    uint32_t color;
    uint32_t odds;
    uint32_t userBetting;
    uint32_t totalBetting;

    PartyBettingInfo();
    int32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 
struct protoReqForestPartyLimitInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_LIMIT_REQUEST, };

    uint32_t uin;

    protoReqForestPartyLimitInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspForestPartyLimitInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_LIMIT_RESPONSE, };

    std::vector<PartyCoinLimitInfo> limitInfo;

    protoRspForestPartyLimitInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoReqJoinForestParty : MSG_HEAD
{
    enum { IDD = FORESTPARTY_JOIN_REQUEST, };

    uint32_t uin;
    uint32_t room;

    protoReqJoinForestParty();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspJoinForestParty : MSG_HEAD
{
    enum { IDD = FORESTPARTY_JOIN_RESPONSE, };

    uint32_t result;
    std::vector<PartyRoomCfgInfo> roomCfg;

    protoRspJoinForestParty();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoReqBetting : MSG_HEAD
{
    enum { IDD = FORESTPARTY_BETTING_REQUEST, };

    uint32_t room;
    std::vector<UserBettingInfo> bettingInfo;

    protoReqBetting();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspBetting : MSG_HEAD
{
    enum { IDD = FORESTPARTY_BETTING_RESPONSE, };

    uint32_t result;

    protoRspBetting();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoSynPartyStatusInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_SYN_STATUS, };

    uint32_t status;
    char color[64];
    std::vector<PartyOddsInfo> odds;
    std::vector<PartyBettingInfo> bettingInfo;
    std::vector<PartyLotteryResInfo> resInfo;
    uint32_t countdown;

    protoSynPartyStatusInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoSynPartyResInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_SYN_RESULT, };

    uint32_t animal;
    uint32_t color;
    uint32_t countDown;

    protoSynPartyResInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoReqLotteryHistory : MSG_HEAD
{
    enum { IDD = FORESTPARTY_HISTORY_REQUEST, };

    uint32_t uin;
    uint32_t room;

    protoReqLotteryHistory();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspLotteryHistory : MSG_HEAD
{
    enum { IDD = FORESTPARTY_HISTORY_RESPONSE, };

    std::vector<PartyLotteryResInfo> resInfo;

    protoRspLotteryHistory();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoSynUserQuitForestPartyNotify : MSG_HEAD
{
    enum { IDD = FORESTPARTY_USER_QUIT, };

    uint32_t room;

    protoSynUserQuitForestPartyNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoReqClearBetting : MSG_HEAD
{
    enum { IDD = FORESTPARTY_CLEAR_BETTING_REQUEST, };

    uint32_t room;

    protoReqClearBetting();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspClearBetting : MSG_HEAD
{
    enum { IDD = FORESTPARTY_CLEAR_BETTING_RESPONSE, };

    uint32_t result;

    protoRspClearBetting();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoReqPartyStatusInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_PARTY_STATUS_REQUEST, };

    uint32_t room;

    protoReqPartyStatusInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 
struct protoRspPartyStatusInfo : MSG_HEAD
{
    enum { IDD = FORESTPARTY_PARTY_STATUS_RESPONSE, };

    uint32_t status;
    char color[64];
    std::vector<PartyBettingInfo> bettingInfo;
    std::vector<PartyLotteryResInfo> resInfo;
    uint32_t countdown;

    protoRspPartyStatusInfo();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

#endif // end of _QW_DXYM_PROTO_FORESTPARTY_H_
