/*
* =====================================================================================
*
*       Filename:  protoRecharge.h
*
*    Description:  充值协议
*
*        Version:  1.0
*        Created:  2015年09月21日 12时48分21秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _QW_RECHARGESVR_PROTO_H_
#define _QW_RECHARGESVR_PROTO_H_

#include "protoWealth.h"
#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "Serialize.hpp"
#include "protoSerialize.h"

//充值通知
struct protoRechargeNotifyRequest : MSG_HEAD
{
	enum { IDD = RECHARGE_NOTIFY_REQUEST, };
	uint32_t	uin;

	protoRechargeNotifyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值通知(RMB直接兑换钻石)
struct protoRechargeNotifyExRequest : MSG_HEAD
{
	enum { IDD = RECHARGE_NOTIFYEX_REQUEST, };
	uint32_t	uin;

	protoRechargeNotifyExRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRechargeMallNotifyReq : MSG_HEAD
{
	enum { IDD = RECHARGE_MALL_NOTIFY_REQ, };
	uint32_t	uin;
	uint32_t	money;
	uint32_t	rechargeId;
	uint32_t	svrId;			// 大区ID
	char		orderId[64];	// 订单号
	uint32_t	worthType;		//商品类型
	uint32_t	worth;			//商品数量
	uint32_t	payFrom;		//支付来源

	protoRechargeMallNotifyReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoRechargeMallNotifyResp : MSG_HEAD
{
	enum { IDD = RECHARGE_MALL_NOTIFY_RESP, };
	uint32_t	uin;
	uint32_t	money;
	uint32_t	rechargeId;
	uint32_t	svrId;			// 大区ID
	char		orderId[64];	// 订单号
	int32_t		result;

	protoRechargeMallNotifyResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值请求
struct protoRechargeRequest : MSG_HEAD
{
	enum { IDD = RECHARGE_REQUEST, };
	uint32_t	uin;
	uint32_t	recharge_id;

	protoRechargeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值应答
struct protoRechargeRespone : MSG_HEAD
{
	enum { IDD = RECHARGE_RESPONE, };
	uint32_t	uin;
	uint32_t	total_recharge;
	uint32_t	vip;
	uint32_t	money;
	uint32_t	coin;
	uint32_t	ticket;
	uint32_t	recharge_money;
	uint32_t	recharge_rmb;

	protoRechargeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值应答(新)
struct protoRechargeResponeEx : MSG_HEAD
{
	enum { IDD = RECHARGE_RESPONE_EX, };
	uint32_t	uin;
	uint32_t	total_recharge;
	uint32_t	vip;
	uint32_t	money;
	uint32_t	coin;
	uint32_t	ticket;
	uint32_t	recharge_money;
	uint32_t	recharge_rmb;
	uint32_t	recharge_id;

	protoRechargeResponeEx();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值校验请求
struct protoRechargeCheckRequest : MSG_HEAD
{
	enum { IDD = RECHARGE_CHECK_REQUEST, };
	uint32_t	uin;
	uint32_t	money;

	protoRechargeCheckRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值校验应答
struct protoRechargeCheckRespone : MSG_HEAD
{
	enum { IDD = RECHARGE_CHECK_RESPONE, };
	uint32_t	uin;
	uint32_t	money;

	protoRechargeCheckRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//RechgeIdfyCodeRequest
struct protoRechgeIdfyCodeRequest : MSG_HEAD
{
	enum { IDD = RECHGEIDFYCODEREQUEST, };
	char		orderid[20];
	char		code[20];

	protoRechgeIdfyCodeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//RechgeIdfyCodeRespone
struct protoRechgeIdfyCodeRespone : MSG_HEAD
{
	enum { IDD = RECHGEIDFYCODERESPONE, };
	uint32_t	result;
	char		data[128];

	protoRechgeIdfyCodeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//RechgeMessageRequest
struct protoRechgeMessageRequest : MSG_HEAD
{
	enum { IDD = RECHGEMESSAGEREQUEST, };
	char	orderid[20];

	protoRechgeMessageRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//RechgeMessageRespone
struct protoRechgeMessageRespone : MSG_HEAD
{
	enum { IDD = RECHGEMESSAGERESPONE, };
	uint32_t	result;
	char		data[128];

	protoRechgeMessageRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//PayResultRequest
struct protoPayResultRequest : MSG_HEAD
{
	enum { IDD = PAYRESULTREQUEST, };
	char		orderid[20];
	uint32_t	mz;
	char		md5[128];
	char		oid[128];
	char		spid[20];
	char		zdy[128];
	char		spuid[20];

	protoPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//PayResultRespone
struct protoPayResultRespone : MSG_HEAD
{
	enum { IDD = PAYRESULTRESPONE, };
	char		result[20];

	protoPayResultRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Ping客户端充值请求
struct protoPingClientRechargeRequest : MSG_HEAD
{
	enum { IDD = PINGCLIENTRECHARGEREQUEST, };
	uint32_t	money;
	char		uid[20];
	char		channel[20];
	char		clientIp[20];
	char		extra[512];

	protoPingClientRechargeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Ping客户端充值应答
struct protoPingClientRechargeRespone : MSG_HEAD
{
	enum { IDD = PINGCLIENTRECHARGERESPONE, };
	uint32_t	result;
	char		data[4096];

	protoPingClientRechargeRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Ping支付结果请求
struct protoPingPayResultRequest : MSG_HEAD
{
	enum { IDD = PINGPAYRESULTREQUEST, };
	char		data[2048];

	protoPingPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Ping支付结果应答
struct protoPingPayResultRespone : MSG_HEAD
{
	enum { IDD = PINGPAYRESULTRESPONE, };
	char		result[20];

	protoPingPayResultRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度客户端登入成功请求
struct protoBaiduClientLoginInRequest : MSG_HEAD
{
	enum { IDD = BAIDUCLIENTLOGININREQUEST, };
	char		uid[20];
	char		user_id[20];

	protoBaiduClientLoginInRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度客户端登入成功应答
struct protoBaiduClientLoginInRespone : MSG_HEAD
{
	enum { IDD = BAIDUCLIENTLOGININRESPONE, };
	uint32_t result;
	char data[100];

	protoBaiduClientLoginInRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度支付结果请求
struct protoBaiduPayResultRequest : MSG_HEAD
{
	enum { IDD = BAIDUPAYRESULTREQUEST, };
	char		api_key[50];
	char		user_id[20];
	char		server_id[20];
	char		timestamp[30];
	char		order_id[30];
	char		wanba_oid[30];
	uint32_t	amount;
	char		currency[20];
	int32_t		result;
	char		back_send[10];
	char		return_attach[20];
	char		sign[128];

	protoBaiduPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度支付结果应答
struct protoBaiduPayResultRespone : MSG_HEAD
{
	enum { IDD = BAIDUPAYRESULTRESPONE, };
	char		result[30];

	protoBaiduPayResultRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度移动游戏支付结果请求
struct protoBaiduMobGamePayResultRequest : MSG_HEAD
{
	enum { IDD = BAIDUMOBGAMEPAYRESULTREQUEST, };
	char		appid[50];
	char		orderid[64];
	uint32_t	amount;
	char		unit[20];
	char		jfd[30];
	char		status[10];
	char		paychanne[20];
	char		phone[20];
	char		channel[20];
	char		from[10];
	char		sign[128];
	char		extchannel[20];
	char		cpdefinepart[64];

	protoBaiduMobGamePayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//百度移动游戏支付结果应答
struct protoBaiduMobGamePayResultRespone : MSG_HEAD
{
	enum { IDD = BAIDUMOBGAMEPAYRESULTRESPONE, };
	char result[30];

	protoBaiduMobGamePayResultRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Apple充值请求
struct protoAppleClientRechargeRequest : MSG_HEAD
{
	enum { IDD = APPLECLIENTRECHARGEREQUEST, };
	uint32_t	money;
	char		uid[20];
	char		clientIp[20];

	protoAppleClientRechargeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Apple充值请求应答
struct protoAppleClientRechargeResponse : MSG_HEAD
{
	enum { IDD = APPLECLIENTRECHARGERESPONSE, };
	char		uid[20];
	char		orderid[64];
	uint32_t	result;
	char		errmsg[256];

	protoAppleClientRechargeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Apple充值应答校验
struct protoApplePayResultRequest : MSG_HEAD
{
	enum { IDD = APPLEPAYRESULTREQUEST, };
	char		uid[20];
	char		orderid[64];
	char		receipt_data[16384];	// 16KB

	protoApplePayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//Apple充值应答校验回复
struct protoApplePayResultResponse : MSG_HEAD
{
	enum { IDD = APPLEPAYRESULTRESPONSE, };
	char		uid[20];
	char		orderid[64];
	uint32_t	result;
	char		errmsg[256];

	protoApplePayResultResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360获取用户信息请求
struct protoQiHooUserInfoRequest : MSG_HEAD
{
	enum { IDD = QIHOOUSERINFOREQUEST, };
	char access_token[64];

	protoQiHooUserInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360获取用户信息应答
struct protoQiHooUserInfoResponse : MSG_HEAD
{
	enum { IDD = QIHOOUSERINFORESPONSE, };
	char		result[30];
	char		userinfo[256];

	protoQiHooUserInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360用户充值请求
struct protoQiHooClientPayRequest : MSG_HEAD
{
	enum { IDD = QIHOOCLIENTPAYREQUEST,};
	char		uid[20];
	uint32_t	money;

	protoQiHooClientPayRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360用户充值请求应答
struct protoQiHooClientPayResponse : MSG_HEAD
{
	enum { IDD = QIHOOCLIENTPAYRESPONSE, };
	char		result[30];
	char		orderid[30];

	protoQiHooClientPayResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360用户充值结果校验
struct protoQiHooPayResultRequest : MSG_HEAD
{
	enum { IDD = QIHOOPAYRESULTREQUEST, };
	char		app_key[32];
	char		product_id[36];
	uint32_t	amount;
	char		app_uid[50];
	char		app_ext1[255];
	char		app_ext2[255];
	char		user_id[20];
	char		order_id[20];
	char		gateway_flag[16];
	char		sign_type[8];
	char		app_order_id[64];
	char		sign_return[32];
	char		sign[32];

	protoQiHooPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360用户充值结果校验应答
struct protoQiHooPayResultResponse : MSG_HEAD
{
	enum { IDD = QIHOOPAYRESULTRESPONSE, };
	char		result[30];

	protoQiHooPayResultResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户登录成功请求
struct protoXiaomiClientLoginInRequest : MSG_HEAD
{
	enum { IDD = XIAOMICLIENTLOGININREQUEST, };
	char		uid[20];
	char		session[32];

	protoXiaomiClientLoginInRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户登录成功请求应答
struct protoXiaomiClientLoginInResponse : MSG_HEAD
{
	enum { IDD = XIAOMICLIENTLOGININRESPONSE, };
	int32_t		result;

	protoXiaomiClientLoginInResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户充值请求
struct protoXiaomiClientPayRequest : MSG_HEAD
{
	enum { IDD = XIAOMICLIENTPAYREQUEST, };
	char		uid[20];
	uint32_t	money;

	protoXiaomiClientPayRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户充值请求应答
struct protoXiaomiClientPayResponse : MSG_HEAD
{
	enum { IDD = XIAOMICLIENTPAYRESPONSE, };
	int32_t		result;
	char		orderid[30];
	char		data[50];

	protoXiaomiClientPayResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户充值结果校验
struct protoXiaomiPayResultRequest : MSG_HEAD
{
	enum { IDD = XIAOMIPAYRESULTREQUEST, };
	char		appId[32];
	char		cpOrderId[50];
	char		cpUserInfo[50];
	char		uid[20];
	char		orderId[32];
	char		orderStatus[32];
	uint32_t	payFee;
	char		productCode[32];
	char		productName[128];
	uint32_t	productCount;
	char		payTime[30];
	char		orderConsumeType[10];
	char		partnerGiftConsume[20];
	char		signature[128];

	protoXiaomiPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//小米用户充值结果校验应答
struct protoXiaomiPayResultResponse : MSG_HEAD
{
	enum { IDD = XIAOMIPAYRESULTRESPONSE, };
	int32_t		errcode;
	char		errMsg[50];

	protoXiaomiPayResultResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//咪咕客户端充值请求
struct protoMiGuClientPayRequest : MSG_HEAD
{
	enum { IDD = MIGUCLIENTPAYREQUEST, };
	char		uid[20];
	uint32_t	money;

	protoMiGuClientPayRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//咪咕客户端充值请求应答
struct protoMiGuClientPayResponse : MSG_HEAD
{
	enum { IDD = MIGUCLIENTPAYRESPONSE, };
	uint32_t	result;
	char		orderid[64];
	char		errmsg[256];

	protoMiGuClientPayResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//咪咕充值应答校验
struct protoMiGuPayResultRequest : MSG_HEAD
{
	enum { IDD = MIGUPAYRESULTREQUEST, };
	char		userId[10];
	char		contentId[12];
	char		consumeCode[12];
	char		cpId[6];
	char		hRet[1];
	char		status[4];
	char		versionId[5];
	char		cpparam[16];
	char		packageID[12];

	protoMiGuPayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//咪咕充值应答校验回复
struct protoMiGuPayResultResponse : MSG_HEAD
{
	enum { IDD = MIGUPAYRESULTRESPONSE, };
	int32_t		hRet;
	char		result[12];

	protoMiGuPayResultResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台客户端登入成功请求
struct protoWan360ClientLoginRequest : MSG_HEAD
{
	enum { IDD = WAN360CLIENTLOGINREQUEST, };
	char		uid[32];
	char		platform[10];
	char		gkey[20];
	int32_t		skey;
	uint64_t	time;
	int32_t		is_adult;
	char		exts[50];
	char		type[10];
	char		sign[50];

	protoWan360ClientLoginRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台客户端登入成功应答
struct protoWan360ClientLoginRespone : MSG_HEAD
{
	enum { IDD = WAN360CLIENTLOGINRESPONE, };
	int32_t		result;
	char		data[100];

	protoWan360ClientLoginRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台支付结果请求
struct protoWan360PayResultRequest : MSG_HEAD
{
	enum { IDD = WAN360PAYRESULTREQUEST, };
	char		uid[32];
	char		role_id[32];
	char		role_name[32];
	char		platform[10];
	char		gkey[20];
	int32_t		skey;
	char		order_id[32];
	int32_t		coins;
	int32_t		moneys;
	uint64_t	time;
	char		sign[50];

	protoWan360PayResultRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台支付结果应答
struct protoWan360PayResultRespone : MSG_HEAD
{
	enum { IDD = WAN360PAYRESULTRESPONE, };
	char		uid[32];
	char		role_id[32];
	char		role_name[32];
	char		platform[10];
	char		gkey[20];
	int32_t		skey;
	char		order_id[32];
	int32_t		coins;
	int32_t		moneys;
	uint64_t	time;
	int32_t		result;
	char		errmsg[20];

	protoWan360PayResultRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台用户查询请求
struct protoWan360QueryRequest : MSG_HEAD
{
	enum { IDD = WAN360QUERYREQUEST, };
	char		uid[32];
	char		platform[10];
	char		gkey[20];
	int32_t		skey;
	uint64_t	time;
	char		sign[50];

	protoWan360QueryRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//360开放平台用户查询应答
struct protoWan360QueryRespone : MSG_HEAD
{
	enum { IDD = WAN360QUERYRESPONE, };
	char		uid[32];
	char		nickname[64];
	int32_t		result;
	char		errmsg[20];

	protoWan360QueryRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询特惠礼包请求
struct protoQueryDiscountGiftRequest : MSG_HEAD
{
	enum { IDD = QUERY_DISCOUNTGIFT_REQUEST, };
	
	uint32_t uin;

	protoQueryDiscountGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询特惠礼包请求
struct protoQueryDiscountGiftExRequest : MSG_HEAD
{
	enum { IDD = QUERY_DISCOUNTGIFTEX_REQUEST, };

	uint32_t uin;

	protoQueryDiscountGiftExRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//特惠礼包结构体
struct DiscountGift
{
	uint32_t recharge_id;
	uint32_t price;
	uint32_t num;
	char reward[64];
	uint32_t addreward;
	uint32_t pos;

	DiscountGift();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	
};

//查询特惠礼包响应
struct protoQueryDiscountGiftRespone : MSG_HEAD
{
	enum { IDD = QUERY_DISCOUNTGIFT_RESPONE, };
	
	vector<DiscountGift> vecDiscountGift;

	protoQueryDiscountGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询特惠礼包响应
struct protoQueryDiscountGiftExRespone : MSG_HEAD
{
	enum { IDD = QUERY_DISCOUNTGIFTEX_RESPONE, };
	enum {
		DISCOUNT_ACTIVITY_DISABLE,				//特惠礼包活动关闭
		DISCOUNT_ACTIVITY_ENABLE,				//特惠礼包活动开启
	};

	vector<DiscountGift> vecDiscountGift;
	vector<DiscountGift> vecNextGift;

	uint8_t activity;							//0: 没有活动， 1：活动

	protoQueryDiscountGiftExRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询充值商品

struct protoQueryRechargeItemRequest : MSG_HEAD
{
	enum { IDD = QUERY_RECHARGE_ITEM_REQUEST, };

	uint32_t uin;
	uint32_t item_type; //商品类型

	protoQueryRechargeItemRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct RECHARGE_ITEM
{
	uint32_t recharge_id;
	char name[64];
	int32_t name_key;
	int32_t pos;
	char icon[64];
	int32_t type;
	int32_t label;
	int32_t rmb;
	int32_t money;
	char gift_daily[64];
	char gift_extra[64];
	int32_t vip;
	int32_t discount;
	int32_t num;

	RECHARGE_ITEM();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//查询充值商品响应
struct protoQueryRechargeItemResponse : MSG_HEAD
{
	enum { IDD = QUERY_RECHARGE_ITEM_RESPONSE, };
	uint32_t item_type; //商品类型
	vector<RECHARGE_ITEM> recharge_item;

	protoQueryRechargeItemResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//特惠礼包购买结果请求
struct protoBuyRetDiscountGiftRequest : MSG_HEAD
{
	enum { IDD = BUYRET_DISCOUNTGIFT_REQUEST, };
	
	uint32_t uin;
	uint32_t recharge_id;

	protoBuyRetDiscountGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//特惠礼包购买结果响应
struct protoBuyRetDiscountGiftRespone : MSG_HEAD
{
	enum { IDD = BUYRET_DISCOUNTGIFT_RESPONE, };
	
	uint32_t recharge_id;
	uint32_t num;

	protoBuyRetDiscountGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//充值校验请求(新)
struct protoRechargeCheckExRequest : MSG_HEAD
{
	enum { IDD = RECHARGE_CHECKEX_REQUEST, };
	
	uint32_t uin;
	uint32_t recharge_id;

	protoRechargeCheckExRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//充值校验响应(新)
struct protoRechargeCheckExRespone : MSG_HEAD
{
	enum { IDD = RECHARGE_CHECKEX_RESPONE, };
	
	uint32_t uin;
	uint32_t recharge_id;

	protoRechargeCheckExRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//新手充值礼包
struct protoQueryNewUserGiftRequest : MSG_HEAD
{
	enum { IDD = QUERY_NEWUSER_GIFT_REQUEST, };

	uint32_t uin;

	protoQueryNewUserGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//新手充值礼包响应
struct protoQueryNewUserGiftResponse : MSG_HEAD
{
	enum { IDD = QUERY_NEWUSER_GIFT_RESPONSE, };

	uint32_t newUser;
	uint32_t restTime;

	protoQueryNewUserGiftResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


struct protoSvrRechargeNumNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_RECHARGE_NUM_NOTIFY, };

	uint32_t uin;
	uint32_t vipUp;					// vip提升
	uint32_t rechargeNum;			// 充值金额

	protoSvrRechargeNumNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

struct protoReqDiamondExCoin : MSG_HEAD
{
    enum { IDD = PROTO_REQ_DIAMONDEXCOIN, };

    uint32_t diamond;

    protoReqDiamondExCoin();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

// 钻石兑换金币请求应答
struct protoRspDiamondExCoin : MSG_HEAD
{
    enum { IDD = PROTO_RSP_DIAMONDEXCOIN, };

    uint32_t result;
    uint32_t coin;

    protoRspDiamondExCoin();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
    int32_t getProtoId(void);
};

//充值通知(rechargesvr->hallsvr)
struct protoRechargeNotify : MSG_HEAD
{
    enum { IDD = RECHARGE_NOTIFY, };
    uint32_t	uin;
    uint32_t    extendType; //0:不带数据 1：兑换商城数据
    char    szExtend[1024];

    enum eType
    {
        recharge, // 充值

        exchange, // 兑换
    };

    protoRechargeNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//兑换订单充值通知(hallsvr->gamesvr)
struct protoExchangeRechargeNotify : MSG_HEAD
{
    enum { IDD = EXCHANGE_RECHARGE_NOTIFY, };
    uint32_t	uin;
    uint32_t    exchangeId;
    uint32_t    itemId;
    uint32_t    rmb;
    uint32_t    rechargeId;

    protoExchangeRechargeNotify();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

//新手礼包状态(hallsvr)
struct protoNewUserGiftStatusRequest : MSG_HEAD
{
    enum { IDD = NEWUSER_GIFT_STATUS_REQUEST, };

    uint32_t uin;

    protoNewUserGiftStatusRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};



//新手礼包状态响应(hallsvr)
struct protoNewUserGiftStatusResponse : MSG_HEAD
{
    enum { IDD = NEWUSER_GIFT_STATUS_RESPONSE, };

    uint32_t newUser;

    protoNewUserGiftStatusResponse();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};


//充值信息通知 hallsvr->gamesvr
struct protoRechargeInfoNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_RECHARGE_INFO_NOTIFY, };

	uint32_t uin;
	uint32_t vip;
	uint32_t rmb;
	uint32_t totalRecharge;
	uint32_t rechargeId;
	char nick[64];
	uint32_t gainCoin;
	uint32_t gainMoney;
	uint32_t gainTicket;
	char gainItems[128];

	protoRechargeInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//充值通知(rechargesvr->centersvr)
struct protoRechargeInfoSync : MSG_HEAD
{
    enum { IDD = RECHARGE_SYNC, };
    uint32_t	uin;
	uint32_t	rechargeId;
    uint32_t    amount; 
	uint32_t    platform;
	uint32_t	source;
	char 		channel[64];
    char    	szExtend[1024];

    protoRechargeInfoSync();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoRechargeNotifyReq : MSG_HEAD
{
	enum { IDD = RECHARGE_NOTIFY_REQ };

	uint32_t	uin;
	uint32_t	coin;
	uint32_t	type;
	uint32_t	ticket;
	uint32_t	money;
	char 		orderid[64];
	
	vector<ProtoItemInfo> vecItemInfo;

	protoRechargeNotifyReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoRechargeNotifyResp : MSG_HEAD
{
    enum { IDD = RECHARGE_NOTIFY_RESP, };
    int32_t		result;
	char 		orderid[64];

    protoRechargeNotifyResp();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

#endif // _QW_RECHARGESVR_PROTO_H_
