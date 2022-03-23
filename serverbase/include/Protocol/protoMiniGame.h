/*
* =====================================================================================
*
*       Filename:  protoMiniGame.h
*
*    Description:  
*
*        Version:  1.0
*        Created:  2017年4月17日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_MINIGAME_H_
#define _QW_PROTOCOL_MINIGAME_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
#include "Serialize.hpp"

struct MiniGameData
{
    uint32_t id;			
    uint32_t type;	        
    uint32_t name;
	uint32_t pos;
	char icon[64];
	char extend[64];
	
    MiniGameData();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoMiniGameListReq : MSG_HEAD
{
	enum { IDD = PROTO_MINIGAME_LIST_REQ, };

	uint32_t version;

	protoMiniGameListReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoMiniGameListResp : MSG_HEAD
{
	enum { IDD = PROTO_MINIGAME_LIST_RESP, };
	std::vector<MiniGameData> gameData;

	protoMiniGameListResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoEnterMiniGameReq : MSG_HEAD
{
	enum { IDD = PROTO_ENTER_MINIGAME_REQ, };
	uint32_t id;

	protoEnterMiniGameReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoEnterMiniGameResp : MSG_HEAD
{
	enum { IDD = PROTO_ENTER_MINIGAME_RESP, };
	int32_t result;

	protoEnterMiniGameResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct TorpedoCompeteBoxData
{
    uint32_t type;			
    uint32_t pointTotal;	        
    uint32_t pointLeft;
	uint32_t result;
		
    TorpedoCompeteBoxData();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct TorpedoCompeteWinnerInfo
{
    uint32_t rank;			
    uint32_t uin;	        
    char nick[64];
	uint64_t damage;
	uint32_t reward;	
	
    TorpedoCompeteWinnerInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct TorpedoCompeteWinnningInfo
{
    uint32_t period;			
    uint32_t type;	        
    std::vector<TorpedoCompeteWinnerInfo> winner;
	
    TorpedoCompeteWinnningInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoTorpedoCompeteRoomInfoNotify : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_ROOMINFO_NOTIFY, };
	uint32_t period;	
	std::vector<TorpedoCompeteBoxData> boxData;
	std::vector<TorpedoCompeteWinnningInfo> winningInfo;

	protoTorpedoCompeteRoomInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoAttackTorpedoCompeteBoxReq : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_ATTACK_REQ, };
	uint32_t type;

	protoAttackTorpedoCompeteBoxReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoAttackTorpedoCompeteBoxResp : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_ATTACK_RESP, };
	int32_t result;
	uint32_t damage;

	protoAttackTorpedoCompeteBoxResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoTorpedoCompeteWinningInfoReq : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_WIN_INFO_REQ, };
	uint32_t page;
	uint32_t count;
	
	protoTorpedoCompeteWinningInfoReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoTorpedoCompeteWinningInfoResp : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_WIN_INFO_REQ, };
	uint32_t total;
	std::vector<TorpedoCompeteWinnningInfo> winningInfo;
	
	protoTorpedoCompeteWinningInfoResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct TorpedoCompetePlayerRecord
{
    uint32_t type;			
    uint32_t period;	        
	uint32_t result;
	uint32_t damage;	
	uint32_t torpedo;	
	uint32_t rank;	
	uint32_t reward;	
	
    TorpedoCompetePlayerRecord();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoTorpedoCompeteRecordInfoReq : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_REC_INFO_REQ, };
	uint32_t page;
	uint32_t count;
	
	protoTorpedoCompeteRecordInfoReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoTorpedoCompeteRecordInfoResp : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_REC_INFO_RESP, };
	uint32_t total;
	std::vector<TorpedoCompetePlayerRecord> recordInfo;
	
	protoTorpedoCompeteRecordInfoResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoTorpedoCompeteRankInfoReq : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_RANK_INFO_REQ, };
	uint32_t type;
	
	protoTorpedoCompeteRankInfoReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoTorpedoCompeteRankInfoResp : MSG_HEAD
{
	enum { IDD = PROTO_TROPEDO_CMP_RANK_INFO_RESP, };
	uint32_t selfRank;
	uint64_t selfDamage;
	uint32_t period;
	uint32_t totalPeriod;
	std::vector<TorpedoCompeteWinnerInfo> rankInfo;
	
	protoTorpedoCompeteRankInfoResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};
#endif	// _QW_PROTOCOL_MINIGAME_H_

