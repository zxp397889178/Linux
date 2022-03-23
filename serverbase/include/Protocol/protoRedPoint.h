/*
* =====================================================================================
*
*       Filename:  protoRedPoint.h
*
*    Description:  ������Э��
*
*        Version:  1.0
*        Created:  2018��01��12�� 16ʱ33��49��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  meilinfeng
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_PROTOCOL_REDPOINT_H_
#define _QW_PROTOCOL_REDPOINT_H_


#include <vector>
#include "protoBase.h"
#include "protoDef.h"

struct protoGetRedPointStatusRequest : MSG_HEAD
{
	enum { IDD = GET_RED_POINT_STATUS_REQUEST};
	uint32_t	uin;

	protoGetRedPointStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



struct RedPonitInfo
{
	enum eType
	{
		Mail = 1,			// �ʼ�
		Activity = 2,		// �
		FreeCoin = 3,		// ��ѽ��
		NewbieSignin = 4,	// ����ǩ��
		Jackpot = 5,		// ��Ϸ����
		RechargeFund = 6,	// ��ֵ����
	};

	enum eFreeCoinType		// ��Ӧ redPointId
	{
		Task = 2,				// ����
		Signin = 3,				// ǩ��
		SharpTimeGift = 4,		// �������
		BindMobile = 5,			// ���ֻ�
	};

	enum ePointId
	{
		CheckInGameGold = 15,			// ǩ����(���)
    	CheckInGameDiamond = 16,		// ǩ����(��ʯ)
    	CheckInGameLottery = 17,		// ǩ����(��ȯ)

    	JackpotWarPrimary = 18,			// ����ս��(����)
    	JackpotWarAdvanced = 19,		// ����ս��(�߼�)
		
		Turnplate2 = 66,				// ת��2

		ActivityButton = 101,
		FreeCoinButton = 102,
		MooncakeButton = 103,

		CheckInGameButton = 104,		// ����������

		JackpotWarButton = 105,			// ����ս��
		JackpotWarPrimaryWin = 106,		// ����ս��(����-�н�)
		JackpotWarPrimaryBox = 107,		// ����ս��(����-����)
		JackpotWarAdvancedWin = 108,	// ����ս��(�߼�-�н�)
		JackpotWarAdvancedBox = 109,	// ����ս��(�߼�-����)

		DailyTask2 = 110,				// ÿ������2
		DailyTask2_Box = 111,			// ÿ������2_����
		DailyTask2_Turnplate = 112,		// ÿ������2_ת��
		
		DailyRecharge = 114,			// ÿ�ճ�ֵ
		Auction = 115,					// һԪ�ᱦ
		Auction_money= 116,				// һԪ�ᱦ(��ʯ)
		Auction_coin= 117,				// һԪ�ᱦ(���)
		Auction_ticket= 118,			// һԪ�ᱦ(��ȯ)
		
		
		NewbieSigninButton = 200,		// ����ǩ��
		JackpotButton = 201,			// ��Ϸ����
		RechargeFundButton = 202,		// ��ֵ����
	};

	enum eStatus
	{
		Close = 0,
		Open = 1,
	};


	uint32_t  redPointType;	// ������ͣ� 1Ϊ�ʼ���2Ϊ���3 ��ѽ��
	// ���ID���ʼ�Ϊ1��2-����  3-ǩ��  4-������� 5-���ֻ�  �Ϊ�����   
	// ActivityButton = 101   FreeCoinButton = 102
	uint32_t  redPointId;	
	uint32_t  status;		// ״̬:0/1
	uint32_t  clearFlag;
	char reserve[256];		// �����ֶ�

	RedPonitInfo();
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	uint32_t getLength(void);
};



struct protoGetRedPointStatusResponse : MSG_HEAD
{
	enum { IDD = GET_RED_POINT_STATUS_RESPONSE };

	
	std::vector<RedPonitInfo> vecRedPointStatus;

	protoGetRedPointStatusResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};





struct protoSetRedPointStatusRequest : MSG_HEAD
{
	enum { IDD = SET_RED_POINT_STATUS_REQUEST};
	uint32_t	uin;
	uint32_t  redPointType;
	uint32_t  redPointId;
	uint32_t  status;
	uint32_t  clearFlag;
	char reserve[256];

	protoSetRedPointStatusRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};




struct protoSetRedPointStatusResponse : MSG_HEAD
{
	enum { IDD = SET_RED_POINT_STATUS_RESPONSE};
	uint32_t	result;


	protoSetRedPointStatusResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrRedPointNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_RED_POINT_NOTIFY, };

	uint32_t uin;
	uint32_t type;
	uint32_t id;
	uint32_t status;

	protoSvrRedPointNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

#endif


