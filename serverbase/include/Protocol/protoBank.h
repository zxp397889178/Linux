/*
* =====================================================================================
*
*       Filename:  protoBank.h
*
*    Description:  银行协议
*
*        Version:  1.0
*        Created:  2015年10月9日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_BANKPROTOCOL_H_
#define _GFX_PROXY_BANKPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

//存取银行金币请求
struct protoAccessBankCoinRequest : MSG_HEAD
{
	enum { IDD = ACCESS_BANK_COIN_REQUEST, };
	uint32_t uin;
	uint32_t coin_num;
	uint32_t flag;
	char passwd[128];

	protoAccessBankCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//存取银行金币应答
struct protoAccessBankCoinRespone : MSG_HEAD
{
	enum { IDD = ACCESS_BANK_COIN_RESPONE, };
	uint32_t uin;
	uint32_t coin;
	uint32_t bank_coin;

	protoAccessBankCoinRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//修改银行密码请求
struct protoModifyBankPasswdRequest : MSG_HEAD
{
	enum { IDD = MODIFY_BANK_PASSWD_REQUEST, };
	uint32_t uin;
	char old_passwd[128];
	char new_passwd[128];

	protoModifyBankPasswdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//修改银行密码应答
struct protoModifyBankPasswdRespone : MSG_HEAD
{
	enum { IDD = MODIFY_BANK_PASSWD_RESPONE, };
	uint32_t uin;

	protoModifyBankPasswdRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

/////////////////////Bank V2/////////////////////////

//存取银行金币请求
struct protoAccessBankCoinRequestV2 : MSG_HEAD
{
	enum { IDD = PROTO_ACC_BANK_COIN_REQ_V2, };
	uint32_t uin;
	uint32_t coin_num;
	uint32_t flag;

	protoAccessBankCoinRequestV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//存取银行金币应答
struct protoAccessBankCoinResponeV2 : MSG_HEAD
{
	enum { IDD = PROTO_ACC_BANK_COIN_RESP_V2, };
	enum RESULT
	{
		SUCCESS,
		BANK_FULL,		//保险箱已满
		TODAY_MAX,		//今日已不可存入更多金币
		MISS_CONFIG,	//找不到配置
		COIN_ERR,		//金币数量错误
		SYS_ERR,		//Redis错误
		NEED_VIP,		//VIP等级不足
		WAIT_ASYNC,		//异步处理等待
	};

	uint32_t result;
	uint32_t flag;
	uint32_t coin;
	uint32_t bank_coin;
	uint32_t coin_today;

	protoAccessBankCoinResponeV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//检查银行密码请求
struct protoChkBankPasswdRequest : MSG_HEAD
{
	enum { IDD = PROTO_CHK_BANK_PASSWD_REQ, };
	enum FLAG
	{
		SET_PASSWD, //设置密码
		CHK_PASSWD, //输入密码
	};
	uint32_t uin;
	uint32_t flag;
	char passwd[128];

	protoChkBankPasswdRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//检查银行密码应答
struct protoChkBankPasswdRespone : MSG_HEAD
{
	enum { IDD = PROTO_CHK_BANK_PASSWD_RESP, };
	enum RESULT
	{
		SUCCESS,
		WRONG_PASSWD, //密码错误
		HAS_PASSWD,	  //已存在密码
		WRONG_FLAG,	  //操作码flag错误
		PWD_LENTH_ERR,  //密码长度错误
		PWD_FORMAT_ERR, //密码格式错误
	};

	uint32_t result;	

	protoChkBankPasswdRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//银行信息请求
struct protoGetBankInfoRequest : MSG_HEAD
{
	enum { IDD = PROTO_GET_USER_BANK_INFO_REQ, };

	uint32_t uin;

	protoGetBankInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//银行信息应答
struct protoGetBankInfoRespone : MSG_HEAD
{
	enum { IDD = PROTO_GET_USER_BANK_INFO_RESP, };
	enum HAS_PASSWD
	{
		NO_PASSWD,
		HAS_PASSWD,
	};

	uint32_t coin_today;
	uint32_t has_passwd;
	uint32_t need_vip;

	char extend[256];

	protoGetBankInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



#endif	//_GFX_PROXY_BANKPROTOCOL_H_

