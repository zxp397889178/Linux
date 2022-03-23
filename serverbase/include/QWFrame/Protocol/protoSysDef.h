/*
 * =====================================================================================
 *
 *       Filename:  protoDef.h
 *
 *    Description:  protocol define
 *
 *        Version:  1.0
 *        Created:  2015年09月07日 19时45分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_SYS_PROTOCOL_DEFINE_H_
#define _QW_CRAZYFISH_SYS_PROTOCOL_DEFINE_H_

#include "protoBase.h"
#include "MyNet.h"

/*
 *====================================================================================
 * 系统内部通讯消息
 *====================================================================================
 */

#define  PROTO_SYS_HEARTBEAT							3       //心跳协议
#define  PROTO_SYS_HEARTBEAT_RESP						5       //心跳协议

#define  PROTO_SYS_AUTHORIZE							5001	//内部服务器身份验证
#define  PROTO_SYS_AUTHORIZE_RESP						5002	//内部服务器身份证验确认
#define  PROTO_SYS_SESSION_SHUTDOWN						5000	//内部服务器客户端断开
#define  PROTO_SYS_REPOST_MSG							5003	//转发消息
#define  PROTO_SYS_DEFAULT_MSG							5004	//默认处理函数，如果注册了默认的回调函数，没有注册的数据都会走这个回调

#define  SYS_CLIENT_CLOSE_REQUEST						1002    //客户端与proxy断开后，proxy向gamesvr通知client断开

/*用户数据相关 bgsvr2 5300开始*/
#define	 PROTO_SVR_USERINFO_REQUEST			5300	// 用户信息查询请求
#define	 PROTO_SVR_USERINFO_RESPONSE		5301	// 用户信息查询应答
#define	 PROTO_SVR_USERLEAVEROOM_REQUEST	5302	// 用户离开房间信息同步请求
#define	 PROTO_SVR_USERLEAVEROOM_RESPONSE	5303	// 用户离开房间信息同步应答
#define	 PROTO_SVR_ADD_CURRENCY_REQUEST		5304	// 增加货币请求
#define	 PROTO_SVR_ADD_CURRENCY_RESPONSE	5305	// 增加货币应答
#define	 PROTO_SVR_DEC_CURRENCY_REQUEST		5306	// 扣除货币请求
#define	 PROTO_SVR_DEC_CURRENCY_RESPONSE	5307	// 扣除货币应答
#define	 PROTO_DCSVR_LOAD_USERDATA_RESPONSE	5308	// 数据中心加载数据应答
#define  PROTO_DCSVR_USERDATA_SYNC_REQUEST	5309	// 用户数据同步请求
#define  PROTO_DCSVR_USERDATA_SYNC_RESPONSE	5310	// 用户数据同步应答
#define  PROTO_SVR_REPOST_MSG_REQUEST		5311	// 转发数据请求 to player
#define  PROTO_SVR_REPOST_MSG_RESPONSE		5312	// 转发数据应答

#define  PROTO_CENTERSVR_SUBSCRIBE_REQUEST	5313	// 消息订阅请求 to centersvr

#define CURRENCY_OP_KEY "crazyfish#currency_op_index"

enum CloseReason
{
	closeUnknown = 0,						//0  未明确原因
	closePassive = 1,						//1  客户端主动断开
	closeZombie = 2,						//2  僵尸客户端,指连接上后未发任何消息
	closeNotAuth = 3,						//3  客户端验证超时,指客户端在指定时间内验证合法性未通过
	closeHeartBeat = 4,						//4  客户端心跳超时
	closeDataFault = 5,						//5  客户端数据错误
	closeServerReq = 6,						//6  服务端请求关闭连接
	closeMemoryFault = 7,					//7  系统内存错误导致异常(暂未用)
	closeSystemFault = 8,					//8  服务器系统错误导至客户端关闭
	closeBlackList = 9,						//9  黑名单
	closeDecrypt = 10,						//10 解密失败,加入黑名单
	closeEncrypt = 11,						//11 加密失败
	closeGameShutdown = 12,					//12 gameserver shutdown
	closeLogicFault = 13,					//13 逻辑错误
	closeHackJail = 14,						//14 黑客监狱
	closeDuplicateLogin = 15,				//15 重复登陆
	closeDuplicateLoginWithoutSaveData = 16,//16 重复登陆，且不保存数据
	closeNetFault = 17,						//17 网络错误
	closeAuthTimeout = 18,					//18 认证超时
	closeAuthFault = 19,					//19 认证失败
	closePending = 20,							//20 正常关闭等待
};

struct LogicReq
{
	uint32_t		actor;
	uint32_t 		svrId;		// proxy服务器ID
	uint32_t 		channelId;	// 服务器频道
	struct Record	record;		// 记录头
	void*			pData;		// 消息协议数据
	uint32_t		cbData;		// pData的长度
};
typedef std::vector<LogicReq>	LOGICREQ_VEC;

struct HttpRespData
{
	uint32_t code;
	CMyString strResp;
	CMyString strStatus;
	http::CHttpData::NodeVector vecHead;	// 自定义头
	HttpRespData()
	{
		code = 200;
		strStatus = "OK";
	}
};

struct HttpLogicReq
{
	uint32_t			nSessionId;
	http::CHttpRequest	httpData;			// 消息协议数据
	HttpRespData		respData;			// 应答数据
	bool				bSync;				// 是否同步

	HttpLogicReq()
	{
		nSessionId = 0;
		bSync = true;
	}

};
typedef std::vector<HttpLogicReq>	HTTPLOGICREQ_VEC;


struct WsLogicReq
{
	uint32_t		nSessionId;
	uint32_t		actor;
	uint32_t 		svrId;		// proxy服务器ID
	uint32_t 		channelId;	// 服务器频道
	uint32_t		msgId;		// 消息ID
	void*			pData;		// 消息协议数据
	uint32_t		cbData;		// pData的长度
};
typedef std::vector<WsLogicReq>	WSLOGICREQ_VEC;

#endif // _QW_CRAZYFISH_SYS_PROTOCOL_DEFINE_H_


