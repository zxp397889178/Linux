/*
* =====================================================================================
*
*       Filename:  protoAccount.h
*
*    Description:  �û���Ϣ���Э��
*
*        Version:  1.0
*        Created:  2015��10��15�� 16ʱ08��0��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_ACCOUNTPROTOCOL_H_
#define _GFX_PROXY_ACCOUNTPROTOCOL_H_

#include <string.h>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//�޸��˺���Ϣ����
struct protoUpdateAcctInfoRequest : MSG_HEAD
{
	enum { IDD = UPDATEACCTINFOREQUEST, };
	uint32_t	uin;
	uint32_t	type;
	char		data[128];
	char		value[128];

	protoUpdateAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�޸��˺���ϢӦ��
struct protoUpdateAcctInfoRespone : MSG_HEAD
{
	enum { IDD = UPDATEACCTINFORESPONE, };
	uint32_t	flag;
	uint32_t	type;

	protoUpdateAcctInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡͷ����������
struct protoGetPlayerPicRequest : MSG_HEAD
{
	enum { IDD = GETPLAYERPICREQUEST, };
	uint32_t	type;
	char		data[128];

	protoGetPlayerPicRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡͷ������Ӧ��
struct protoGetPlayerPicRespone : MSG_HEAD
{
	enum { IDD = GETPLAYERPICRESPONE, };
	enum FlagType
	{
		GetPictureFailure = 0,					// ��ȡͷ��ʧ��
		Success = 1,							// �ɹ�
		GetPicreqTypeError = 2,					// �������typeֵ����
		PicVerify = 3							// ��ͷ�������
	};

	uint32_t	flag;
	char		data[128];

	protoGetPlayerPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ�ǳ��Ƿ��޸�����
struct protoGetNickIfUpdateRequest : MSG_HEAD
{
	enum { IDD = GETNICKIFUPDATEREQUEST, };
	uint32_t	uin;

	protoGetNickIfUpdateRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ�ǳ��Ƿ��޸�Ӧ��
struct protoGetNickIfUpdateRespone : MSG_HEAD
{
	enum { IDD = GETNICKIFUPDATERESPONE, };
	uint32_t	number;

	protoGetNickIfUpdateRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//AccountRespone
struct protoAccountRespone : MSG_HEAD
{
	enum { IDD = ACCOUNTRESPONE, };
	uint32_t	uin;
	uint32_t	type;
	uint32_t	number;

	protoAccountRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���԰汾����
struct protoLanguageVersionRequest : MSG_HEAD
{
	enum { IDD = LANGUAGE_VERSION_REQUEST, };
	uint32_t uin;
	int32_t language;
	int32_t reserve1;
	int32_t reserve2;
	int32_t reserve3;
	int32_t reserve4;

	protoLanguageVersionRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoFindPasswdVerifyCodeRequest: MSG_HEAD
{
	enum { IDD = FINDPASSWD_VERIFYCODE_NEW_REQUEST, };
	char msisdn[32];
	char channel[32];
	uint32_t source;

	protoFindPasswdVerifyCodeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoFindPasswdVerifyCodeRespone: MSG_HEAD
{
	enum { IDD = FINDPASSWD_VERIFYCODE_NEW_RESPONE, };
	int32_t result;

	protoFindPasswdVerifyCodeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};



struct protoResetPasswdRequest: MSG_HEAD
{
	enum { IDD = RESET_PASSWD_NEW_REQUEST, };
	char msisdn[16];
	char channel[32];
	char verify_code[10];
	char new_passwd[33];
	uint32_t source;

	protoResetPasswdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoResetPasswdRespone: MSG_HEAD
{
	enum { IDD = RESET_PASSWD_NEW_RESPONE, };
	int32_t result;

	protoResetPasswdRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


//����ǳ�����
struct protoUserNickRequest : MSG_HEAD
{
	enum { IDD = USER_NICK_REQUEST, };
	uint32_t	uin;

	protoUserNickRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//����ǳ�Ӧ��
struct protoUserNickResponse : MSG_HEAD
{
	enum { IDD = USER_NICK_RESPONSE, };
	int32_t	result;
	char nick[128];

	protoUserNickResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ������
struct protoGiveCoinRequest : MSG_HEAD
{
	enum { IDD = GIVE_COIN_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t id;

	protoGiveCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ��Ӧ��
struct protoGiveCoinResponse: MSG_HEAD
{
	enum { IDD = GIVE_COIN_RESPONSE, };
	int32_t	result;

	protoGiveCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//ͷ��ͬ��
struct protoUserPicSync : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PIC_SYNC, };
	uint32_t uin;
	char pic[256];

	protoUserPicSync();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//���ͽ������
struct protoPresentCoinRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	char passwd[64];
	uint32_t coin;

	protoPresentCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ��Ӧ��
struct protoPresentCoinResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RESPONSE, };
	int32_t	result;
	uint32_t value;

	protoPresentCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//������������
struct protoPresentCondRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_COND_REQUEST, };
	uint32_t type;

	protoPresentCondRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��������Ӧ��
struct protoPresentCondResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_COND_RESPONSE, };
	uint32_t vip;
	uint32_t presentLimitEveryTime;
	uint32_t type;
	uint32_t reverse2;
	uint32_t level;
	uint32_t totalRecharge;

	protoPresentCondResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct PresentRecordInfo
{
	uint32_t uin;
	char nick[64];
	uint32_t time;
	int32_t num;

	PresentRecordInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//���ͽ�Ҽ�¼����
struct protoPresentRecordRequest : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RECORD_REQUEST, };
	uint32_t uin;
	uint32_t type;

	enum eType
	{
		ReceiveCoin = 0,			// �յ��Ľ��
		SendOutCoin = 1,			// �ͳ��Ľ��
		ReceiveCrystal = 2,			// �յ���ˮ��
		SendOutCrystal = 3,			// �ͳ���ˮ��
		AllCrystal = 4,				// ȫ��ˮ��
		ReceiveGun = 5,				// �յ�������
		SendOutGun = 6,				// �ͳ�������
	};

	protoPresentRecordRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ�Ҽ�¼Ӧ��
struct protoPresentRecordResponse : MSG_HEAD
{
	enum { IDD = PRESENT_COIN_RECORD_RESPONSE, };

	uint32_t type;
	std::vector<PresentRecordInfo> infoList;

	protoPresentRecordResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�޸��˺���Ϣ����(hallsvr)
struct protoHallUpdateAcctInfoRequest : MSG_HEAD
{
	enum { IDD = HALL_UPDATE_ACCT_INFO_REQUEST, };
	uint32_t	uin;
	uint32_t	type;
	char		data[512];
	char		value[512];

	enum OP_TYPE
	{
		UPDATE_NICK = 1,						// �޸��ǳ�
		UPDATE_PASSWD = 2, 						// �޸�����
		BIND_MSISDN = 3,						// ���ֻ���
		BIND_EMAIL = 4,							// ������
		SET_FACE = 5,							// ����ͷ������
		GET_MSISDN_VERIFYCODE = 6,				// ��ȡ�ֻ���֤��
		GET_EMAIL_VERIFYCODE = 7,				// ��ȡ������֤��
		UPDATE_NICK_NEW = 8,					// �޸��ǳ�(��) �۽��
		UPDATE_SIGNATURE = 9,					// �޸�ǩ��
	};

	protoHallUpdateAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�޸��˺���ϢӦ��(hallsvr)
struct protoHallUpdateAcctInfoResponse : MSG_HEAD
{
	enum { IDD = HALL_UPDATE_ACCT_INFO_RESPONSE, };
	int32_t	result;
	uint32_t type;
	char data[512];
	char value[512];

	protoHallUpdateAcctInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ѯ�˺���Ϣ����(hallsvr)
struct protoQueryAcctInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_ACCT_INFO_REQUEST, };
	uint32_t	uin;
	uint32_t	type;

	enum eType
	{
		Signature = 1,					// ǩ��
		Nick = 2,						// �ǳ�
		ClanMemberNick = 3,				// �����Ա�ǳ�
	};

	protoQueryAcctInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ѯ�˺���ϢӦ��(hallsvr)
struct protoQueryAcctInfoResponse : MSG_HEAD
{
	enum { IDD = QUERY_ACCT_INFO_RESPONSE, };
	uint32_t type;
	char data[256];

	protoQueryAcctInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ������(hallsvr->gamesvr)
struct protoSvrPresentCoinRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_COIN_REQ, };
	uint32_t uin;
	uint32_t targetUin;
	char passwd[64];
	uint32_t coin;
	uint32_t tax;

	protoSvrPresentCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ��Ӧ��(gamesvr->hallsvr)
struct protoSvrPresentCoinResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_COIN_RESP, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t coin;
	int32_t	result;
	uint32_t tax;

	protoSvrPresentCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct PresentInfo
{
	uint32_t id;
	uint32_t num;

	PresentInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//��������
struct protoPresentRequest : MSG_HEAD
{
	enum { IDD = PRESENT_REQUEST, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	char passwd[64];

	enum eType
	{
		Coin = 1,			// ���
		Crystal = 2,		// ˮ��
		Gun = 3,
	};

	protoPresentRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//����Ӧ��
struct protoPresentResponse : MSG_HEAD
{
	enum { IDD = PRESENT_RESPONSE, };
	int32_t	result;

	protoPresentResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ������(hallsvr->gamesvr)
struct protoSvrPresentRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_REQ, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	char passwd[64];
	uint32_t tax;

	protoSvrPresentRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ��Ӧ��(gamesvr->hallsvr)
struct protoSvrPresentResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_PRESENT_RESP, };
	uint32_t uin;
	uint32_t targetUin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;
	uint32_t tax;
	int32_t	result;

	protoSvrPresentResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�������ӵ���(hallsvr->gamesvr)
struct protoSvrAddItemForPresentNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_ADD_ITEM_PRESENT_NOTIFY, };
	uint32_t uin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;

	protoSvrAddItemForPresentNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct PresentRecordInfoV1
{
	uint32_t uin;
	char nick[64];
	uint32_t time;
	int32_t num;
	uint32_t itemTypeId;
	char reserve1[64];

	PresentRecordInfoV1();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//���ͽ�Ҽ�¼����
struct protoPresentRecordRequestV1 : MSG_HEAD
{
	enum { IDD = PRESENT_RECORD_REQUEST_V1, };
	uint32_t uin;
	uint32_t type;

	enum eType
	{
		ReceiveCoin = 0,			// �յ��Ľ��
		SendOutCoin = 1,			// �ͳ��Ľ��
		ReceiveCrystal = 2,			// �յ���ˮ��
		SendOutCrystal = 3,			// �ͳ���ˮ��
		AllCrystal = 4,				// ȫ��ˮ��
		ReceiveGun = 5,				// �յ�������
		SendOutGun = 6,				// �ͳ�������
	};

	protoPresentRecordRequestV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//���ͽ�Ҽ�¼Ӧ��
struct protoPresentRecordResponseV1 : MSG_HEAD
{
	enum { IDD = PRESENT_RECORD_RESPONSE_V1, };

	uint32_t type;
	std::vector<PresentRecordInfoV1> infoList;

	protoPresentRecordResponseV1();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��������������������
struct protoPresentGunPointRequest : MSG_HEAD
{
	enum { IDD = PRESENT_GUN_POINT_REQUEST, };
	uint32_t uin;
	
	protoPresentGunPointRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//����������������Ӧ��
struct protoPresentGunPointResponse : MSG_HEAD
{
	enum { IDD = PRESENT_GUN_POINT_RESPONSE, };

	uint32_t value;
	uint32_t point;
	uint32_t recharge;

	protoPresentGunPointResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//�û���ǰ���û��ֲ�ѯ
struct protoAccountPointRequest : MSG_HEAD
{
	enum { IDD = HALL_ACCOUNT_POINT_REQUEST, };
	uint32_t uin;

	protoAccountPointRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// �û���ǰ���û��ֲ�ѯӦ��
struct protoAccountPointResponse : MSG_HEAD
{
	enum { IDD = HALL_ACCOUNT_POINT_RESPONSE, };
	uint32_t canUsePointToday;
	uint32_t userPoint;
	
	protoAccountPointResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct MonthCardRewardInfo
{
	uint32_t type;
	uint32_t num;

	enum eType
	{
#if 0
		Coin = 0,					// ���	
		Item = 1,					// ����
		Probability = 2,			// ����
		SignIn = 3,					// ǩ���ӳ�
		Gun	= 4,					// ����
#endif
        
        MONEY = 1,
        COIN = 2,
        TICKET = 3
	};

	MonthCardRewardInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//�û��¿���Ϣ��ѯ����
struct protoUserMonthCardRequest : MSG_HEAD
{
	enum { IDD = USER_MONTH_CARD_REQUEST, };
	uint32_t uin;

	protoUserMonthCardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// �û��¿���Ϣ��ѯӦ��
struct protoUserMonthCardResponse : MSG_HEAD
{
	enum { IDD = USER_MONTH_CARD_RESPONSE, };
	int32_t leftDay;
	int32_t isReceive;
	std::vector<MonthCardRewardInfo> infoList;

	protoUserMonthCardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ�¿���������
struct protoReceiveMonthCardRewardRequest : MSG_HEAD
{
	enum { IDD = RECEIVE_MONTH_CARD_REWARD_REQUEST, };
	uint32_t uin;

	protoReceiveMonthCardRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//��ȡ�¿�����Ӧ��
struct protoReceiveMonthCardRewardResponse : MSG_HEAD
{
	enum { IDD = RECEIVE_MONTH_CARD_REWARD_RESPONSE, };
	int32_t result;

	protoReceiveMonthCardRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//ǩ������֪ͨ
struct protoSvrSigninBonusNotify : MSG_HEAD
{
	enum { IDD = PROTO_HALL_SIGNIN_BONUS_NOTIFY, };

	uint32_t uin;
	uint32_t bonus;

	protoSvrSigninBonusNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//����У������
struct protoPresentCheckRequest : MSG_HEAD
{
	enum { IDD = PRESENT_CHECK_REQUEST, };
	uint32_t uin;
	uint32_t type;
	std::vector<PresentInfo> vecPresent;

	protoPresentCheckRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//����У��Ӧ��
struct protoPresentCheckResponse : MSG_HEAD
{
	enum { IDD = PRESENT_CHECK_RESPONSE, };
	uint32_t result;
	uint32_t value;

	protoPresentCheckResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�û�������Ϣ����
struct protoUserPresentInfoRequest : MSG_HEAD
{
	enum { IDD = USER_PRESENT_INFO_REQUEST, };
	uint32_t uin;

	protoUserPresentInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�û�������ϢӦ��
struct protoUserPresentInfoResponse : MSG_HEAD
{
	enum { IDD = USER_PRESENT_INFO_RESPONSE, };
	uint32_t leftTotalTimes;
	uint32_t leftTodayTimes;
	uint32_t presentDailyMaxTimes;
	uint32_t presentSingleMaxPoint;
	uint32_t curLevelMaxPoint;
	uint32_t costPoint;
	uint32_t leftPresentPriceToday;			// ����ʣ�����ͼ�ֵ
	uint32_t leftPointToday;				// ����ʣ����û���

	protoUserPresentInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// ��ȡ�ϴ��Զ���ͷ��token����
struct protoReqGetUploadToken : MSG_HEAD
{
    enum { IDD = PROTO_REQ_GETUPLOADTOKEN, };

    uint32_t uin;

    protoReqGetUploadToken();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// ��ȡ�ϴ��Զ���ͷ��token����Ӧ��
struct protoRspGetUploadToken : MSG_HEAD
{
    enum { IDD = PROTO_RSP_GETUPLOADTOKEN, };

    uint32_t result;
    char token[16];
    char desc[64];

    protoRspGetUploadToken();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// �ϴ������ռ����û���֤ģ������
struct protoClientAliAuthReq : MSG_HEAD
{
	enum { IDD = PROTO_CLI_ALIAUTH_INFO, };

	uint32_t uin;
	char szRawData[64];
	char szSign[1024];

	protoClientAliAuthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �ϴ������ռ����û���֤ģ������
struct protoClientAliAuthResp : MSG_HEAD
{
	enum { IDD = PROTO_CLI_ALIAUTH_INFO_RESP, };

	uint32_t uin;
	int32_t result;
	// 0: ���� 1: ����� 2: ģ���� 3: IDC IP 4: �ֻ��Ƿ�

	protoClientAliAuthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//�û���Ϣ��ѯ����
struct protoQueryUserInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_USER_INFO_REQUEST, };
	uint32_t uin;

	protoQueryUserInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�û���Ϣ��ѯӦ��
struct protoQueryUserInfoResponse : MSG_HEAD
{
	enum { IDD = QUERY_USER_INFO_RESPONSE, };
	uint32_t uin;
	uint32_t vip;
	char nick[64];
	char pic[256];
	char signature[256];

	protoQueryUserInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//��ȡͷ����������
struct protoHallGetPlayerPicRequest : MSG_HEAD
{
    enum { IDD = HALL_GET_PLAYER_PIC_REQUEST, };
    uint32_t	type;
    char		data[128];

    protoHallGetPlayerPicRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//��ȡͷ������Ӧ��
struct protoHallGetPlayerPicRespone : MSG_HEAD
{
    enum { IDD = HALL_GET_PLAYER_PIC_RESPONSE, };
    enum FlagType
    {
        GetPictureFailure = 0,			// ��ȡͷ��ʧ��
        Success = 1,							// �ɹ�
        GetPicreqTypeError = 2,		// �������typeֵ����
        PicVerify = 3							// ��ͷ�������
    };

    uint32_t	flag;
    char		data[512];

    protoHallGetPlayerPicRespone();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//�ύ�Զ���ͷ������
struct protoHallSubmitCustomPicRequest : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_CUSTOM_PIC_REQUEST, };
	uint32_t	type;
	char		data[512];

	protoHallSubmitCustomPicRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//�ύ�Զ���ͷ��Ӧ��
struct protoHallSubmitCustomPicRespone : MSG_HEAD
{
	enum { IDD = HALL_SUBMIT_CUSTOM_PIC_RESPONE, };
	enum FlagType
	{
		SubmitCustomPicFailure = 0,			// �ύͷ��ʧ��
		Success = 1,						// �ɹ�
		PicVerify = 2						// ��ͷ�������
	};

	uint32_t	flag;
	char		data[512];

	protoHallSubmitCustomPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//�Զ���ͷ����˽��֪ͨ
struct protoHallAuditingCustomPicRespone : MSG_HEAD
{
	enum { IDD = HALL_AUDITING_CUSTOM_PIC_RESPONE, };
	enum FlagType
	{
		SubmitCustomPicFailure = 0,			// ���ͨ��
		Success = 1,						// �����
		PicVerify = 2						// ���δͨ��
	};

	uint32_t	flag;

	protoHallAuditingCustomPicRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoSvrSyncVip : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SYNC_VIP, };
	uint32_t uin;
	uint32_t vip;

	protoSvrSyncVip();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoVerifyCodeRequestV2 : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_REQUEST_V2, };
	char msisdn[32];
	char channel[32];
	uint32_t source;
	uint32_t type;

	protoVerifyCodeRequestV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};

struct protoVerifyCodeResponseV2 : MSG_HEAD
{
	enum { IDD = VERIFY_CODE_RESPONSE_V2, };
	int32_t result;
	uint32_t type;

	protoVerifyCodeResponseV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);

};


struct protoDriftBottleConditionRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CONDITION_REQUEST, };
    uint32_t uin;
    
    protoDriftBottleConditionRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleConditionResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CONDITION_RESPONSE, };
    uint32_t vip;
    uint32_t totalRecharge;
    int32_t maxBottleCount;
    int32_t	curBottleCount;
    int32_t maxErrCount;
    int32_t curErrCount;
    int32_t duration;
	int32_t maxChangePwdCount;	

    protoDriftBottleConditionResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct BottleItemInfo
{
    enum eType
    {
        Money = 1,				//Ȥ���
        Coin = 2,				//���
        Ticket = 3,				//��ȯ
        Trumpet = 4,			//����
        Item = 5,				//����
        BankCoin = 6,			//���н��
        GoldDiamon = 7,         //����
    };

    uint32_t type;			// 1-Ȥ��� 2-��� 3-��ȯ 4-���� 5-����
    uint32_t id;	        // ����ID
    int64_t num;

    BottleItemInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct protoDriftBottleMakeRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_MAKE_REQUEST, };
    std::vector<BottleItemInfo> vecBottleItem;
    char pwd[8];

    protoDriftBottleMakeRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleMakeResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_MAKE_RESPONSE, };
    int32_t result;
    char id[24];

    protoDriftBottleMakeResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleOpenRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_OPEN_REQUEST, };
    char id[24];
    char pwd[8];

    protoDriftBottleOpenRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleOpenResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_OPEN_RESPONSE, };
    int32_t result;
    int32_t leftErrCount;
    std::vector<BottleItemInfo> vecBottleItem;

    protoDriftBottleOpenResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleTakebackRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_TAKEBACK_REQUEST, };
    char id[24];    

    protoDriftBottleTakebackRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleTakebackResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_TAKEBACK_RESPONSE, };
    int32_t result;
    
    protoDriftBottleTakebackResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct BottleRecordInfo
{
    uint32_t uin;			
    char  nick[32];	        
    int64_t time;
    char id[24];
    std::vector<BottleItemInfo> vecBottleItem;
    char pwd[8];
    uint32_t status;
    int32_t curChangePwdCount;

    BottleRecordInfo();
    uint32_t getLength(void);
    int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
    int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


struct protoDriftBottleRecordRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_RECORD_REQUEST, };
    uint32_t type;

    protoDriftBottleRecordRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoDriftBottleRecordResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_RECORD_RESPONSE, };
    std::vector<BottleRecordInfo> vecBottleRecord;
    uint32_t type;

    protoDriftBottleRecordResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


struct protoSvrDriftBottleMakeRequest : MSG_HEAD
{
    enum { IDD = PROTO_HALL_DRIFTBOTTLE_REQ, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;
    char passwd[8];
    uint32_t tax;

    protoSvrDriftBottleMakeRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoSvrDriftBottleMakeResponse : MSG_HEAD
{
    enum { IDD = PROTO_HALL_DRIFTBOTTLE_RESP, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;
    char passwd[8];
    int32_t	result;
    uint32_t tax;

    protoSvrDriftBottleMakeResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoSvrAddBottleItemNotify : MSG_HEAD
{
    enum { IDD = PROTO_HALL_ADD_BOTTLEITEM_NOTIFY, };
    uint32_t uin;
    std::vector<BottleItemInfo> vecBottleItem;

    protoSvrAddBottleItemNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleChangePwdRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CHANGEPWD_REQUEST, };
    char id[24];
    char pwd[8];

    protoDriftBottleChangePwdRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleChangePwdResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_CHANGEPWD_RESPONSE, };
    int32_t result;
	int32_t leftChangePwdCount;

    protoDriftBottleChangePwdResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleQueryRequest : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_QUERY_REQUEST, };
    char id[24];

    protoDriftBottleQueryRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoDriftBottleQueryResponse : MSG_HEAD
{
    enum { IDD = PROTO_DRIFTBOTTLE_QUERY_RESPONSE, };
    int32_t result;

    protoDriftBottleQueryResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};
#endif // _GFX_PROXY_ACCOUNTPROTOCOL_H_
