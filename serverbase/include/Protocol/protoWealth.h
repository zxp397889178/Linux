/*
* =====================================================================================
*
*       Filename:  protoWealth.h
*
*    Description:  ��ҲƸ����Э��
*
*        Version:  1.0
*        Created:  2018��03��09��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTO_WEALTH_H_
#define _QW_PROTO_WEALTH_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "BaseCode.h"
#include "protoDef.h"
#include <vector>
using namespace std;

struct ProtoItemInfo
{
	uint32_t itemTypeId;
	uint32_t num;

	ProtoItemInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// �۲Ƹ�����
struct protoSvrDeductWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEDUCT_WEALTH_REQUEST, };

	uint32_t uin;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	uint32_t trumpet;
	uint32_t indexId;
	uint32_t type;
	int64_t opIndex;
	
	vector<ProtoItemInfo> vecItemInfo;

	protoSvrDeductWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDeductWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DEDUCT_WEALTH_RESPONSE, };

	enum eResult
	{
		Success = 0,		// �ɹ�
		Failure = 1,		// ʧ��
	};

	int32_t result;
	uint32_t uin;
	uint32_t indexId;
	int64_t opIndex;
	
	protoSvrDeductWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//֪ͨgameSvr���ӽ��----------------------
struct protoSvrAddCoinNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_CLAN_ADDCOIN_NOTIFY };

	uint32_t	uin;
	uint32_t	coin;
	uint32_t	type;
	uint32_t	ticket;
	uint32_t	money;

	vector<ProtoItemInfo> vecItemInfo;

	enum eType
	{
		TicketExchangeCoin = 1,				//��ȯ�һ����
		TicketExchangeItem = 2,				//��ȯ�һ�����
		Recharge = 3,						//��ֵ
		SevenGift = 4,						//�������
		OnlineGift = 5,						//�������
		Mail = 6,							//�ʼ�
		MoneyExchangeCoin = 7,				//��ʯ�����
		MoidfyNick = 8,						//�޸��ǳ�
		SharpTimeGift = 9,					//�������
		Talent = 10,						//������ս
		Compet = 11,						//������
		VipGift = 12,						//VIP���
		SignIn = 13,						//ÿ��ǩ��
		GiftKey = 14,						//����һ���
		OperateActivity = 15,				//��Ӫ�
		Alms = 16,							//�ȼý�
		BindMsisdnReward = 17,				//���ֻ�
		Cornucopia = 18,					//�۱���
		OfferAReward = 19,					//��������
		LevelUp = 20,						//����
		CreateClan = 21,					//��������
		ExchangeFishCoin = 22,				//�һ��㱦
		GetoutPerFund = 23,					//��ȡ���˻���
		GetoutClanFund = 24,				//��ȡ�������
		DepositClanCoin = 25,				//���������
		PresentCoinCost = 26,				//���ͽ������
		PresentCoinGain = 27,				//���ͽ�һ��
		Broadcast = 28,						//�ɸ�㲥
		BuyItem = 29,						//�������
		UseItem = 30,						// ʹ�õ���
		PresentCrystalCost = 31,			//����ˮ������
		Treasure = 32,						//����
		MonthCard = 50,					// �¿�
		HappyTreasure = 60,		    //���ֶᱦ���
		HappyTreasureItem = 61,		    //���ֶᱦ����
		LivenessReward = 62,              //��Ծ������
		H5LoginGift = 63,					// H5��¼���
		ExchangeOrderRefund = 64,					// ȡ���һ������󷵻�
		TreasureChestReward = 70,           //����齱�н�����
	};

	protoSvrAddCoinNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//���ӲƸ�����----------------------
struct protoSvrAddWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_WEALTH_REQUEST };

	uint32_t	uin;
	uint32_t	coin;
	uint32_t	type;
	uint32_t	ticket;
	uint32_t	money;
	uint32_t 	trumpet;
	uint32_t 	indexId;
	int64_t 	opIndex;
	
	vector<ProtoItemInfo> vecItemInfo;

	enum eType
	{
		TicketExchangeCoin = 1,				//��ȯ�һ����
		TicketExchangeItem = 2,				//��ȯ�һ�����
		Recharge = 3,						//��ֵ
		SevenGift = 4,						//�������
		OnlineGift = 5,						//�������
		Mail = 6,							//�ʼ�
		MoneyExchangeCoin = 7,				//��ʯ�����
		MoidfyNick = 8,						//�޸��ǳ�
		SharpTimeGift = 9,					//�������
		Talent = 10,						//������ս
		Compet = 11,						//������
		VipGift = 12,						//VIP���
		SignIn = 13,						//ÿ��ǩ��
		GiftKey = 14,						//����һ���
		OperateActivity = 15,				//��Ӫ�
		Alms = 16,							//�ȼý�
		BindMsisdnReward = 17,				//���ֻ�
		Cornucopia = 18,					//�۱���
		OfferAReward = 19,					//��������
		LevelUp = 20,						//����
		CreateClan = 21,					//��������
		ExchangeFishCoin = 22,				//�һ��㱦
		GetoutPerFund = 23,					//��ȡ���˻���
		GetoutClanFund = 24,				//��ȡ�������
		DepositClanCoin = 25,				//���������
		PresentCoinCost = 26,				//���ͽ������
		PresentCoinGain = 27,				//���ͽ�һ��
		Broadcast = 28,						//�ɸ�㲥
		BuyItem = 29,						//�������
		UseItem = 30,						// ʹ�õ���
		PresentCrystalCost = 31,			//����ˮ������
		Treasure = 32,						//����
		MonthCard = 50,						// �¿�
		HappyTreasure = 60,		    		//���ֶᱦ���
		HappyTreasureItem = 61,		    	//���ֶᱦ����
		LivenessReward = 62,              	//��Ծ������
		H5LoginGift = 63,					// H5��¼���
		ExchangeOrderRefund = 64,			// ȡ���һ������󷵻�
		TreasureChestReward = 70,           //����齱�н�����
	};

	protoSvrAddWealthRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrAddWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ADD_WEALTH_RESPONSE, };

	enum eResult
	{
		Success = 0,		// �ɹ�
		Failure = 1,		// ʧ��
	};

	int32_t result;
	uint32_t uin;
	uint32_t indexId;
	int64_t opIndex;
	
	protoSvrAddWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// ��ѯ�Ƹ�����hallsvr->gamesvr
struct protoSvrQueryWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_WEALTH_REQUEST, };

	uint32_t uin;
	uint32_t actor;
	uint32_t svrId;
	uint32_t channelId;

	protoSvrQueryWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrQueryWealthResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_WEALTH_RESPONSE, };

	uint32_t uin;
	uint32_t actor;
	uint32_t svrId;
	uint32_t channelId;

	uint32_t coin;
	uint32_t ticket;
	uint32_t money;

	vector<ProtoItemInfo> vecItemInfo;

	protoSvrQueryWealthResponse();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDrawWealthRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DRAW_WEALTH_REQUEST, };

	uint32_t uin;

	protoSvrDrawWealthRequest();

	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �ⲿ����·�����(�纣�������������ƴ��������룬Ҫ��ӵ��·�)
struct protoExternalAddCurScoreReq : MSG_HEAD
{
	enum { IDD = PROTO_EXTERNAL_ADDCURSCORE, };

	uint32_t 		uin;
	int64_t			coin;

	protoExternalAddCurScoreReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif	// _QW_PROTO_WEALTH_H_
