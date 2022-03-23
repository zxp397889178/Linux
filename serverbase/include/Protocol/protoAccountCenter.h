/*
* =====================================================================================
*
*       Filename:  protoAccountCenter.h
*
*    Description:  账号中心协议
*
*        Version:  1.0
*        Created:  2016年10月28日 9时0分0秒
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

// 账号中心认证请求
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

// 账号中心认证应答
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
		Success = 0,		// 成功
		AccountNotExist,	// 账号不存在
		AccountLocked,		// 账号被锁定
		PasswdError,		// 密码错误
	};

	protoAccountAuthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 账号中心账号注册请求
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

// 账号中心账号注册应答
struct protoAccountRegisterResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_REGISTER_RESP, };
	int32_t result;
	char account[64];
	uint32_t uin;
	enum ErrorCode
	{
		Success = 0,		// 成功
		AccountExists = 1,		// 账号已存在
		PasswdInvalid = 2,		// 密码非法
		VerifyCodeInvalid = 3,		// 验证码非法
	};
	protoAccountRegisterResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 账号中心绑定手机请求
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

// 账号中心绑定手机应答
struct protoAccountBindMobileResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_BINDMOBILE_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
		MobileInvalid = 2,			// 手机号非法
		VerifyCodeInvalid = 3,		// 验证码非法
		MobileBinded = 4,			// 手机号已经绑定
	};
	protoAccountBindMobileResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 账号中心修改密码请求
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

// 账号中心修改密码应答
struct protoAccountModifyPwdResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPWD_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
		OldPasswdError = 2,			// 旧密码错误
		NewPasswdInvalid = 3,		// 新密码非法
	};
	protoAccountModifyPwdResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 账号中心修改昵称请求
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

// 账号中心修改昵称应答
struct protoAccountModifyNickResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYNICK_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
		ExistNick = 2,				// 昵称存在
	};
	protoAccountModifyNickResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 账号中心修改昵称请求
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

// 账号中心修改昵称应答
struct protoAccountModifyPicResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_MODIFYPIC_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
	};
	protoAccountModifyPicResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 账号中心重置密码请求
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

// 账号中心重置密码应答
struct protoAccountFindPwdResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_RESETPWD_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
		VerifyCodeInvalid = 2,		// 验证码非法

	};
	protoAccountFindPwdResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 账号中心验证码请求
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

// 账号中心验证码应答
struct protoAccountVerifyCodeResp : MSG_HEAD
{
	enum { IDD = PROTO_ACCOUNT_VERIFYCODE_RESP, };
	int32_t result;

	enum ErrorCode
	{
		Success = 0,				// 成功
		TokenInvalid = 1,			// token非法
		MobileInvalid = 2,			// 手机号非法
		VerifyCodeExhaust = 3,		// 验证码耗尽（今日验证码发送数量已达到最大值）
		VerifyCodeFrequently = 4,	// 验证码请求太频繁
	};
	protoAccountVerifyCodeResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif	// _QW_PROTO_ACCOUNTCENTER_H_
