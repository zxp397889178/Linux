/*
* =====================================================================================
*
*       Filename:  protoAccountCenter.h
*
*    Description:  �˺�����Э��
*
*        Version:  1.0
*        Created:  2016��10��28�� 9ʱ0��0��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/
#ifndef _QW_PROTO_ACCOUNTCENTER_H_
#define _QW_PROTO_ACCOUNTCENTER_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

// �˺�������֤����
struct protoAccountAuthReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_AUTH_REQ, };
	char deviceId[64];
	char account[64];
	char passwd[64];
	uint32_t version;
	char channel[64];
	uint32_t source;
	char sign[64];

	protoAccountAuthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺�������֤Ӧ��
struct protoAccountAuthResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_AUTH_RESP, };
	int32_t code;
	char account[64];
	uint32_t uin;
	char mobile[32];
	char nick[32];
	char pic[256];
	uint32_t money;
	uint32_t level;
	uint32_t totalRecharge;
	char token[64];

	enum ErrorCode
	{
		Success = 0,		// �ɹ�
		AccountNotExist,	// �˺Ų�����
		AccountLocked,		// �˺ű�����
		PasswdError,		// �������
	};

	protoAccountAuthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������˺�ע������
struct protoAccountRegisterReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_REGISTER_REQ, };
	char deviceId[64];
	char account[64];
	char passwd[64];
	uint32_t version;
	char channel[64];
	uint32_t source;
	char nick[64];
	char sign[64];
	uint32_t verifycode;

	protoAccountRegisterReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������˺�ע��Ӧ��
struct protoAccountRegisterResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_REGISTER_RESP, };
	int32_t result;
	char account[64];
	uint32_t uin;
	enum ErrorCode
	{
		Success = 0,		// �ɹ�
		AccountExists = 1,		// �˺��Ѵ���
		PasswdInvalid = 2,		// ����Ƿ�
		VerifyCodeInvalid = 3,		// ��֤��Ƿ�
	};
	protoAccountRegisterResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �˺����İ��ֻ�����
struct protoAccountBindMobileReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_BINDMOBILE_REQ, };
	uint32_t uin;
	char mobile[32];
	uint32_t verifycode;
	char token[64];
	char sign[64];

	protoAccountBindMobileReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺����İ��ֻ�Ӧ��
struct protoAccountBindMobileResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_BINDMOBILE_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
		MobileInvalid = 2,			// �ֻ��ŷǷ�
		VerifyCodeInvalid = 3,		// ��֤��Ƿ�
		MobileBinded = 4,			// �ֻ����Ѿ���
	};
	protoAccountBindMobileResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �˺������޸���������
struct protoAccountModifyPwdReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPWD_REQ, };
	uint32_t uin;
	char oldPasswd[32];
	char newPasswd[32];
	char token[64];
	char sign[64];

	protoAccountModifyPwdReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������޸�����Ӧ��
struct protoAccountModifyPwdResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPWD_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
		OldPasswdError = 2,			// ���������
		NewPasswdInvalid = 3,		// ������Ƿ�
	};
	protoAccountModifyPwdResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �˺������޸��ǳ�����
struct protoAccountModifyNickReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYNICK_REQ, };
	uint32_t uin;
	char nick[32];
	char token[64];
	char sign[64];

	protoAccountModifyNickReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������޸��ǳ�Ӧ��
struct protoAccountModifyNickResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYNICK_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
		ExistNick = 2,				// �ǳƴ���
	};
	protoAccountModifyNickResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������޸��ǳ�����
struct protoAccountModifyPicReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPIC_REQ, };
	uint32_t uin;
	char pic[265];
	char token[64];
	char sign[64];

	protoAccountModifyPicReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺������޸��ǳ�Ӧ��
struct protoAccountModifyPicResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPIC_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
	};
	protoAccountModifyPicResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// �˺�����������������
struct protoAccountFindPwdReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_RESETPWD_REQ, };
	uint32_t uin;
	char account[32];
	uint32_t verifycode;
	char passwd[32];
	char token[64];
	char sign[64];

	protoAccountFindPwdReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺�������������Ӧ��
struct protoAccountFindPwdResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_RESETPWD_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
		VerifyCodeInvalid = 2,		// ��֤��Ƿ�

	};
	protoAccountFindPwdResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺�������֤������
struct protoAccountVerifyCodeReq : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_VERIFYCODE_REQ, };
	uint32_t uin;
	char mobile[32];
	int32_t type;
	char token[64];
	char sign[64];

	protoAccountVerifyCodeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// �˺�������֤��Ӧ��
struct protoAccountVerifyCodeResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_VERIFYCODE_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// �ɹ�
		TokenInvalid = 1,			// token�Ƿ�
		MobileInvalid = 2,			// �ֻ��ŷǷ�
		VerifyCodeExhaust = 3,		// ��֤��ľ���������֤�뷢�������Ѵﵽ���ֵ��
		VerifyCodeFrequently = 4,	// ��֤������̫Ƶ��
	};
	protoAccountVerifyCodeResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif	// _QW_PROTO_ACCOUNTCENTER_H_
