/*
* =====================================================================================
*
*       Filename:  protoPVPBattle.h
*
*    Description:  PVP对战协议
*
*        Version:  1.0
*        Created:  2018-03-23
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/
#ifndef _QW_PROTO_PVPBATTLE_H_
#define _QW_PROTO_PVPBATTLE_H_


#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"


struct protoPVPinfoRequest : MSG_HEAD
{
	enum { IDD = PVP_GET_INFO_REQUEST };

	uint32_t uin;

	protoPVPinfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};





struct protoPVPinfoRespone : MSG_HEAD
{
	enum { IDD = PVP_GET_INFO_RESPONSE };

	uint32_t uin;
    uint32_t limitVipMin;
    uint32_t limitLevelMin;
    uint32_t limitTotalRechargeMin;
    uint32_t coinPerIntegral;
    uint32_t minBetting;
    uint32_t maxBetting;
    uint32_t remainTotalBetting;
    uint32_t Fees;
    

	protoPVPinfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct PVPcompetitorInfo
{
    enum {PVP_ROLE_CHALLENGER , PVP_ROLE_MASTER};
    enum {PVP_COMMON_USER , PVP_BLACK_GOLD_USER};
    enum {PVP_READY_STATUS_NOT_READY , PVP_READY_STATUS_READY};
    
    uint32_t uin;
    uint32_t role;
    uint32_t level;
    uint32_t vip;
    uint32_t integral;
    uint32_t winCoin;
    uint32_t betting;
    uint32_t readyStatus;

    char nickname[128];
    char headPortrait[256];

    PVPcompetitorInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};



struct PVProomInfo
{
    enum {PVP_MODE_EQUAL,PVP_MODE_FREE};
    
    uint32_t roomId;
    uint32_t bettingMode;

    char reserved[128];

    std::vector<PVPcompetitorInfo> competitors;
    
    PVProomInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};



struct protoPVProomListRequest : MSG_HEAD
{
	enum { IDD = PVP_GET_ROOM_LIST_REQUEST };

	uint32_t uin;

	protoPVProomListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVProomListRespone : MSG_HEAD
{
	enum { IDD = PVP_GET_ROOM_LIST_RESPONSE };

	uint32_t isBlackGoldUser;
    uint32_t totalRooms;

    std::vector<PVProomInfo> rooms;

	protoPVProomListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVPcreateRoomRequest : MSG_HEAD
{
	enum { IDD = PVP_CREATE_ROOM_REQUEST };

	uint32_t uin;
    uint32_t isVisible;
    uint32_t betting;
    uint32_t bettingMode;

	protoPVPcreateRoomRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVPcreateRoomRespone : MSG_HEAD
{
	enum { IDD = PVP_CREATE_ROOM_RESPONSE };

	uint32_t uin;
    uint32_t errorCode;

    std::vector<PVProomInfo> roomInfo;
    
	protoPVPcreateRoomRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};





struct protoPVPjoinRoomRequest : MSG_HEAD
{
	enum { IDD = PVP_JOIN_ROOM_REQUEST };

	uint32_t uin;
    uint32_t roomId;
    
	protoPVPjoinRoomRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVPjoinRoomRespone : MSG_HEAD
{
	enum { IDD = PVP_JOIN_ROOM_RESPONSE };

	uint32_t uin;
    uint32_t errorCode;

    std::vector<PVProomInfo> roomInfo;
    
	protoPVPjoinRoomRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};






struct protoPVPeventRequest : MSG_HEAD
{
	enum { IDD = PVP_EVENT_REQUEST };


    enum { 
       PVP_EVENT_READY = 1,     // 准备
       PVP_EVENT_START = 2,     // 开始对战
       PVP_EVENT_KICKOUT = 3,   // 踢掉对手
       PVP_EVENT_QUIT = 4,      // 退出房间
       PVP_EVENT_INVITE = 5     // 邀请
    };

	uint32_t uin;
    uint32_t roomId;
    uint32_t eventType;
    
	protoPVPeventRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoPVPeventRoomRespone : MSG_HEAD
{
	enum { IDD = PVP_EVENT_RESPONSE };

    uint32_t uin;
    uint32_t roomId;
    uint32_t eventType;
    uint32_t errorCode;

	protoPVPeventRoomRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVPeventNotify : MSG_HEAD
{
	enum { IDD = PVP_EVENT_NOTIFY };

	uint32_t uin;
    uint32_t roomId;
    uint32_t eventType;
    
	protoPVPeventNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};







struct protoPVPinviteNotify : MSG_HEAD
{
	enum { IDD = PVP_INVITE_NOTIFY };

	uint32_t uin;
    uint32_t roomId;
    uint32_t betting;
    uint32_t bettingMode;
    uint32_t losePoint;

    char nickname[128];
    
	protoPVPinviteNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};










struct protoPVPmoraWhatRequest : MSG_HEAD
{
	enum { IDD = PVP_MORAW_WHAT_REQUEST };

    enum 
    {
        NOTHING = 0,
        ROCK = 1 , 
        SCISSORS = 2,
        PAPER = 3
    };

	uint32_t uin;
    uint32_t roomId;
    uint32_t moraWhat;
    
	protoPVPmoraWhatRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoPVPmoraWhatRespone : MSG_HEAD
{
	enum { IDD = PVP_MORAW_WHAT_RESPONSE };

	uint32_t errorCode;
    
	protoPVPmoraWhatRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct PVPresultInfo
{   
    uint32_t uin;
    uint32_t vip;
    uint32_t fees;
    uint32_t role;
    uint32_t moraWhat;
    uint32_t betting;
    uint32_t isWinner;
    uint32_t gainCoin;
    uint32_t loseCoin;
    uint32_t losePoint;

    char nickname[128];
    char headPortrait[256];

    PVPresultInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};



struct protoPVPresultNotify : MSG_HEAD
{
	enum { IDD = PVP_RESULT_NOTIFY };

    enum {DRAW,NOT_DRAW};

    uint32_t roomId;
	uint32_t result;
    uint32_t isGiveUp;

    std::vector<PVPresultInfo> resultInfo;
    
	protoPVPresultNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



#endif

