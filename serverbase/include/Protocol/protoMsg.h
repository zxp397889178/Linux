/*
* =====================================================================================
*
*       Filename:  protoMsg.h
*
*    Description:  msg protocol
*
*        Version:  1.0
*        Created:  2015年09月25日 14时25分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_MSG_H_
#define _PROTO_MSG_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//发送短信请求
struct protoSendSmsRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SEND_SMS_REQUEST, };

	char			msisdn[32];
	uint32_t		verifyCode;
	uint32_t source;

	protoSendSmsRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送短信响应
struct protoSendSmsRespone : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SEND_SMS_RESPONE, };

	int32_t		result;			// 执行结果 0-成功， 其他值失败
	char errMsg[64];			
	protoSendSmsRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送邮件请求
struct protoSendEmailRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SEND_EMAIL_REQUEST, };

	char			emailAddress[64];
	uint32_t		verifyCode;
	uint32_t source;
	
	protoSendEmailRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送邮件响应
struct protoSendEmailRespone : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SEND_EMAIL_RESPONE, };

	enum Result_e
	{
		SUCCESS = 0,
		FAIL = 1
	};

	int32_t		result;			// 执行结果 0-成功， 其他值失败
	char errMsg[64];
	protoSendEmailRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//手机号码校验请求
struct protoPhoneNumberVerifyRequest : MSG_HEAD
{
	enum { IDD = PROTO_PHONE_NUMBER_VERIFY_REQUEST};
	char phoneNumber[16]; //eg: +8618259579305, 18259579305
	
	protoPhoneNumberVerifyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//手机号码校验响应
struct protoPhoneNumberVerifyRespone : MSG_HEAD
{
	enum { IDD = PROTO_PHONE_NUMBER_VERIFY_RESPONE};

	char phoneNumber[16]; //original number
	
	enum Result_e 
	{
		VALID_NUMBER = 0,
		INVALID_NUMBER
	};

	int32_t result;	
	char msisdn[16];		// eg:  18259579305
	char region[4];			// eg:CN
	int32_t countryCode; 	// eg:86
	
	protoPhoneNumberVerifyRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//发送告警短信
struct protoSendAlarmSmsRequest : MSG_HEAD
{
	enum { IDD = PROTO_SEND_ALARM_SMS_REQUEST };
	char sms[1024];

	protoSendAlarmSmsRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


//发送语音电话
struct protoSendVoiceRequest : MSG_HEAD
{
	enum { IDD = PROTO_SEND_VOICE_REQUEST };
	char	 msisdn[16];

	protoSendVoiceRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送语音电话响应
struct protoSendVoiceRespone : MSG_HEAD
{
	enum { IDD = PROTO_SEND_VOICE_RESPONE};

	enum Result_e
	{
		RESULT_SUCCESS = 0,//成功
		RESULT_FAIL = 1//失败
	};

	uint32_t result;

	protoSendVoiceRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送么么网页消息
struct protoSendWebMsgRequest : MSG_HEAD
{
	enum { IDD = PROTO_SEND_WEB_MSG_REQUEST };

	uint32_t msgType;
	char title[64];
	char content[1024];
	uint32_t uin;
	uint32_t userId;//么么平台用户id
	uint32_t coin;//捕鱼获得的金币

	protoSendWebMsgRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//发送第三方广播消息
struct protoSend3rdMsgNotify : MSG_HEAD
{
	enum { IDD = PROTO_SEND_3RD_MSG_NOTIFY };

	uint32_t uin;
	char account[64];
	char nick[64];
	uint32_t source;
	uint32_t coin;	//捕鱼获得的金币

	protoSend3rdMsgNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


#endif //  _PROTO_MSG_H_

