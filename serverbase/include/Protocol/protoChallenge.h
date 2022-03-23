/*
* =====================================================================================
*
*       Filename:  protoChallenge.h
*
*    Description:  �¾��������Э��
*
*        Version:  1.0
*        Created:  2016��12��1�� 13ʱ30��0��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_CHALLENGE_H_
#define _QW_PROTOCOL_CHALLENGE_H_

#include "protoDef.h"
#include "protoBase.h"
#include "protoRank.h"
#include <vector>
using namespace std;

// �¾�������������
struct protoChallengeSignUpRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_SIGNUP_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeSignUpRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾���������Ӧ��
struct protoChallengeSignUpResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_SIGNUP_RESPONSE, };

	int32_t result;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t cost;
	uint32_t costType;
	uint32_t number;

	protoChallengeSignUpResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾�����״̬֪ͨ
struct protoChallengeStatusNotify : MSG_HEAD
{
	enum  { IDD = CHALLENGE_STATUS_NOTIFY, };

	uint32_t roomId;
	uint32_t scheduleId;
	int32_t status;
	char beginTime[32];
	uint32_t duration;
	uint32_t number;
	uint32_t roomType;

	protoChallengeStatusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾����������б�����
struct protoChallengeListRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_LIST_REQUEST, };

	uint32_t uin;
	uint32_t roomFlag;

	protoChallengeListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct ChallengeRoomData
{
	uint32_t roomId;
	uint32_t scheduleId;
	int32_t status;
	uint32_t cost;
	uint32_t costType;
	char beginTime[32];
	uint32_t duration;
	uint32_t number;
	uint32_t roomType;
	uint32_t minNumber;
	uint32_t maxNumber;
	char icon[128];
	char roomName[64];

	ChallengeRoomData();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);

};

// �¾����������б�Ӧ��
struct protoChallengeListResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_LIST_RESPONSE, };

	uint32_t roomFlag;
	vector<ChallengeRoomData> roomList;

	protoChallengeListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �¾��������а�����
struct protoChallengeRankRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾��������а�Ӧ��
struct protoChallengeRankResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_RANK_RESPONSE, };

	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;

	protoChallengeRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �¾��������а�����
struct protoChallengeDynamicRankRequest : MSG_HEAD
{
	enum  { IDD = CHALLENGE_DYNAMIC_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;

	protoChallengeDynamicRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾��������а�Ӧ��
struct protoChallengeDynamicRankResponse : MSG_HEAD
{
	enum  { IDD = CHALLENGE_DYNAMIC_RANK_RESPONSE, };

	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;

	protoChallengeDynamicRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*-------------------------------------------------------------------------------------------------------------*/

struct SignUpUserInfo
{
	uint32_t uin;
	uint32_t tableId;
	uint8_t seatId;

	SignUpUserInfo()
	{
		uin = 0;
		tableId = 0;
		seatId = 0;
	}

	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};
// competsvr -> gamesvr �¾�����������Ϣͬ��
struct protoSvrChallengeInfoSync : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_INFO_SYNC, };

	uint32_t roomId;
	uint32_t scheduleId;
	time_t signUpBeginTime;
	time_t signUpEndTime;
	time_t gameBeginTime;
	time_t gameEndTime;
	uint32_t roomFlag;
	uint32_t minNum;
	uint32_t maxNum;
	uint32_t status;
	uint32_t gamesvrId;	
	uint32_t signUpNum;
	vector<SignUpUserInfo> vecSignUpUser;		// �������û�
	char twRoomName[64];				// ��������
	char engRoomName[64];				// Ӣ������
	char chnRoomName[64];				// ��������
	char icon[256];						// ����ͼ��URL

	protoSvrChallengeInfoSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr �¾�������������
struct protoSvrChallengeSignUpRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SIGNUP_REQ, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t vip;
	char nick[64];

	protoSvrChallengeSignUpRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr �¾���������Ӧ��
struct protoSvrChallengeSignUpResponse : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SIGNUP_RESP, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	int32_t result;

	enum eResult
	{
		Success = 0,
		Full,					// ����
		SignUped,				// �Ѿ�����
		ScheduleNotFound,		// ����û�ҵ�
		StatusError,			// ���Ǳ���״̬
		CurrencyNotEnough,		// ���Ҳ���
	};

	protoSvrChallengeSignUpResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr �¾��������а�����
struct protoSvrChallengeRankReq : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RANK_REQ, };

	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;

	protoSvrChallengeRankReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr �¾��������а�Ӧ��
struct protoSvrChallengeRankResp : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RANK_RESP, };

	vector<NewRankInfoVip> vecRank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;
	uint32_t rank;
	uint32_t score;

	protoSvrChallengeRankResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// gamesvr -> competsvr �¾��������������Ǽ�
struct protoSvrChallengeScoreReg : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_SCORE_REG, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t score;
	uint32_t vip;
	char nick[64];

	protoSvrChallengeScoreReg();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> competsvr �¾�������̬���а�����
struct protoSvrChallengeDynamicRankReq : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_DYNAMIC_RANK_REQ, };

	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;

	protoSvrChallengeDynamicRankReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// competsvr -> gamesvr �¾�������̬���а�Ӧ��
struct protoSvrChallengeDynamicRankResp : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_DYNAMIC_RANK_RESP, };

	vector<NewRankInfoVip> vecRank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t uin;
	uint32_t rank;
	uint32_t score;

	protoSvrChallengeDynamicRankResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾������ҵı�������
struct protoSvrChallengeMyselfRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_MYSELF_REQ, };

	uint32_t uin;
	uint32_t roomFlag;
	uint32_t language;

	protoSvrChallengeMyselfRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾������ҵı���Ӧ��
struct protoSvrChallengeMyselfResponse : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_MYSELF_RESP, };

	uint32_t uin;
	uint32_t roomFlag;
	vector<ChallengeRoomData> roomList;

	protoSvrChallengeMyselfResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �¾��������ñ���֪ͨ
struct protoSvrChallengeResetNotify : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_CHALLENGE_RESET_NOTIFY, };

	protoSvrChallengeResetNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*�������Ϣ*/
struct protoHallCompeteInfoRequest : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_INFO_REQUEST, };

	uint32_t uin;

	protoHallCompeteInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoHallCompeteInfoResponse : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_INFO_RESPONSE, };

	uint32_t uin;
	uint32_t beginTime;
	uint32_t svrTime;
	uint32_t roomId;
	vector<uint32_t> enterRoomIds;
	uint32_t curRewardRoomId;
	vector<uint32_t> rewardRoomIds;

	protoHallCompeteInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

/*��������а�*/
struct protoHallCompeteRankRequest : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_RANK_REQUEST, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t scheduleId;
	uint32_t type;

	enum eType
	{
		TopSix = 0,			// ǰ6��
		TopTen = 1,			// ǰ10��
	};

	protoHallCompeteRankRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoHallCompeteRankResponse : MSG_HEAD
{
	enum  { IDD = HALL_COMPETE_RANK_RESPONSE, };

	uint32_t uin;
	vector<NewRankInfoVip> vecRank;
	uint32_t rank;
	uint32_t roomId;
	uint32_t scheduleId;
	uint64_t score;
	uint32_t beginTime;
	uint32_t endTime;
	uint32_t type;

	protoHallCompeteRankResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// gamesvr -> center ��ú�����ͳ��
struct protoSvrGainNotify : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_GAIN_NOTIFY, };

	uint32_t uin;
	uint32_t roomId;
	uint32_t gain;
	uint32_t cost;
	uint32_t vip;
	char nick[64];

	protoSvrGainNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoPonitCompeteStatusNotify : MSG_HEAD
{
	enum  { IDD = POINT_COMPETE_STATUS_NOTIFY, };

	uint32_t status;
	uint32_t time;
	uint32_t svrTime;
	uint32_t roomId;
	vector<uint32_t> enterRoomIds;
	uint32_t scheduleId;

	protoPonitCompeteStatusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif	// _QW_PROTOCOL_CHALLENGE_H_
