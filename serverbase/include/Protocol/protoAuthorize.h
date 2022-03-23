/*
 * =====================================================================================
 *
 *       Filename:  protoAuthorize.h
 *
 *    Description:  authorize protocol
 *
 *        Version:  1.0
 *        Created:  2015年09月09日 14时25分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _GFX_PROXY_AUTHORIZEPROTOCOL_H_
#define _GFX_PROXY_AUTHORIZEPROTOCOL_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>

#include "BaseCode/ByteOrder.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"
using namespace std;


// 认证请求
struct protoAuthorizeReq : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_AUTHORIZE, };

	uint32_t	uin;
#if 0
	char 		device_id[64];
	char 		account[32];
	char 		passwd[32];
	uint32_t 	new_flag;
	uint32_t 	source;
	uint32_t 	chg_account_flag;
	uint32_t 	version;
	char 		channel[32];
#else
	char 		device_id[64];
	char 		account[256];
	char 		passwd[128];
	uint32_t 	new_flag;
	uint32_t 	source;
	uint32_t	chg_account_flag;
	uint32_t 	version;
	char 		channel[32];
#endif

	protoAuthorizeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 丰游麻将登陆认证协议
struct protoMahjongAuthorizeReq : MSG_HEAD
{
	enum { IDD = PROTO_3RD_MAHJONG_AUTHORIZE, };

	int32_t		userId;			// 用户ID
	int32_t		sex;			// 性别
	char		gameTicket[64]; // 通话KEY
	char		nickName[64];	// 呢称
	char		headImage[64];  // 头像

	//--------- 以下数据为扩展,兼容官方之用 ---------------
	uint32_t	source;
	uint32_t	version;
	char		channel[32];
	char		device_id[64];	// 设备号
	char		extra[256];		// 扩展

	protoMahjongAuthorizeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 么么直播登陆认证协议
struct protoMemeyuleAuthorizeReq : MSG_HEAD
{
	enum { IDD = PROTO_3RD_MEMEYULE_AUTHORIZE, };

	int64_t		userId;				// 么么平台唯一账号
	char		account[64];		// 么么平台id标识 
	char		reserve[256];		// 预留之用

	//--------- 以下数据为扩展,兼容官方之用 ---------------
	uint32_t	source;
	uint32_t	version;
	char		channel[32];
	char		device_id[64];		// 设备号
	char		extra[1024];			// 扩展

	protoMemeyuleAuthorizeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 微信账号登陆认证协议
struct protoWechatAuthorizeReq : MSG_HEAD
{
	enum { IDD = PROTO_3RD_WECHAT_AUTHORIZE, };

	char		openId[64];		// 微信ID
	char		token[64];		// 微信API返回token	
	int32_t		sex;			// 性别
	char		nickName[64];	// 呢称
	char		headImg[64];	// 头像

	//--------- 以下数据为扩展,兼容官方之用 ---------------
	uint32_t	source;
	uint32_t	version;
	char		channel[32];
	char		deviceId[64];	// 设备号
	char		imei[64];		// imei

	protoWechatAuthorizeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 微信绑定账号认证结果应答
struct protoWxAuthorizeResp : MSG_HEAD
{
	enum { IDD = PROTO_WXAUTHRESP_AUTHRESULT, };
	enum ErrorCode
	{
		errSucceed = 0,
		errPendingNew,			//1 新用户注册
		errProtoFault,			//2 协议错误
		errIpForbid,			//3 IP被禁
		errDevNull,				//4 设备号为空
		errDevForbid,			//5 设备被禁
		errVersion,				//6 协议版本号不兼容,或来源非法
		errAccountFault,		//7 账号密码错
		errDupLogin,			//8 重复登陆
		errGameOutService,		//9 游服未启动
		errSysForbid,			//10 系统封号
		errSysFault,			//11 系统错误
		errAccountBinded,		//12 账号已绑定,请输入账号密码
		errAccountNotExist,		//13 账号不存在
		errFetchCoinFailure,	//14 获取金币失败
		errLoginTooFrequency,	//15 登陆太频繁
		err3rdAuthFailure,		//16 第三方认证服务器错误
		errAuthTimeout,			//17 认证超时，请重试
		errAuthKeyError,		//18 auth_key不匹配 360平台
	};

	int32_t		errcode;
	uint32_t	uin;
	char		openId[64];		// 微信ID
	char		nick[64];
	char		token[64];		// 备注，可能未用
	int32_t		newuser;		// 1 - 新用户, 0 - 旧用户
	char		channel[32];	// 渠道
	uint32_t	source;			// 来源
	uint32_t	version;		// 版本号

	protoWxAuthorizeResp(int32_t _err = 0, uint32_t _uin = 0, const char *pszNick = "", const char *pszNote="");
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 认证结果应答
struct protoAuthorizeResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTHRESP_AUTHRESULT, };
	enum ErrorCode
	{
		errSucceed = 0,
		errPendingNew,			//1 新用户注册
		errProtoFault,			//2 协议错误
		errIpForbid,			//3 IP被禁
		errDevNull,				//4 设备号为空
		errDevForbid,			//5 设备被禁
		errVersion,				//6 协议版本号不兼容,或来源非法
		errAccountFault,		//7 账号密码错
		errDupLogin,			//8 重复登陆
		errGameOutService,		//9 游服未启动
		errSysForbid,			//10 系统封号
		errSysFault,			//11 系统错误
		errAccountBinded,		//12 账号已绑定,请输入账号密码
		errAccountNotExist,		//13 账号不存在
		errFetchCoinFailure,	//14 获取金币失败
		errLoginTooFrequency,	//15 登陆太频繁
		err3rdAuthFailure,		//16 第三方认证服务器错误
		errAuthTimeout,			//17 认证超时，请重试
		errAuthKeyError,		//18 auth_key不匹配 360平台
		errRemoteLogin,			//19 异地登录
        errClientUpdate,        //20 客户端需要更新
		errClientCreateIP,      //21 创建账号失败(IP限制)
		errClientCreateChannel, //22 创建账号失败(渠道限制)
		errClientCreateDevice   //23 创建账号失败(设备限制)
	};
		
	int32_t		errcode;
	uint32_t	uin;
	char		nick[32];
	char		note[64];		// 备注，可能未用
	int32_t		newuser;		// 1 - 新用户, 0 - 旧用户
	char		channel[64];	// 渠道
	uint32_t	source;			// 来源
	uint32_t	version;		// 版本号

	protoAuthorizeResp(int32_t _err = 0, uint32_t _uin = 0, const char *pszNick = "", const char *pszNote="");
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 服务器准备好连接
struct protoGameSvrReadyResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTHRESP_GAMEREADY, };

	uint32_t	gameId;
	uint32_t	uin;
	uint32_t	flag;		// 1-内部使用; 2-指定游服ID
	char		szDevice[64];	// 设备号
	uint32_t	source;			// 来源
	char		szChannel[32];  // 频道号
	int32_t		newuser;		// 1 - new user, 2 - old user
	uint32_t	version;		// client version


	protoGameSvrReadyResp(uint32_t _gameId = 0, uint32_t _uin=0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求进入游戏
struct protoEnterGameReq : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_ENTERGAME, };

	uint32_t	gameId;
	uint32_t	uin;
	char		szDevice[64];	// 设备号
	uint32_t	source;			// 来源
	char		szChannel[32];  // 频道号
	int32_t		newuser;		// 1 - new user, 2 - old user
	uint32_t	version;		// client version

	protoEnterGameReq(uint32_t _gameId=0, uint32_t _uin=0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoClientCtrlRespone : public MSG_HEAD
{
	enum { IDD = CLIENT_CONTROL_RESPONE };

	uint8_t updateControl;
	uint8_t shopControl;
	uint8_t control1;
	uint8_t control2;

	protoClientCtrlRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 断开客户端连接
struct protoCloseClientResp : MSG_HEAD
{
	enum { IDD = CLOSE_CLIENT_ERROR_RESPONE, };

	enum KickoutReason
	{
		accountBinded = 1001,		// 账号已绑定
		accountFault = 1002,		// 账号名密码错
		accountNotFound = 1003,		// 账号不存在
		duplicateLogin = 1004,		// 重复登陆
		versionInvalid = 1005,		// 版本不匹配
		sourceInvalid = 1006,		// 来源不合法
		protocolInvalid = 1007,		// 协议不合法
		gamesvrShutdown = 1008,		// 游服断开连接
		clientShutdown = 1009,		// 客户端主动断开
	};

	uint32_t error_code;

	protoCloseClientResp(uint32_t _error_code = 0);

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

// 下发用户数据
struct protoUserDataResp : MSG_HEAD
{
	enum { IDD = PROTO_GAMERESP_USERDATA, };

	struct AreaInfo
	{
		uint32_t game_id;
		char game_name[64];
		uint32_t area_id;
		char area_name[64];
		uint32_t area_flags;
		char area_note[256];
		int32_t length()
		{
			return sizeof(uint32_t)
				+ sizeof(uint16_t) + strlen(game_name)
				+ sizeof(uint32_t)
				+ sizeof(uint16_t) + strlen(area_name)
				+ sizeof(uint32_t)
				+ sizeof(uint16_t) + strlen(area_note);
		}
	};
	struct FriendInfo
	{
		int32_t f_type;
		uint32_t f_uin;
		char f_name[256];
		int32_t today_gift;
		char f_icon[128];
		int32_t length()
		{
			return sizeof(int32_t)
				+ sizeof(uint32_t)
				+ sizeof(uint16_t) + strlen(f_name)
				+ sizeof(int32_t)
				+ sizeof(uint16_t) + strlen(f_icon);
		}
	};
	struct PkgInfo
	{
		uint32_t item_id;
		uint32_t item_typeid;
		uint32_t num;
		int32_t length()
		{
			return sizeof(uint32_t)
				+ sizeof(uint32_t)
				+ sizeof(uint32_t);
		}
	};
	struct RechargeInfo
	{
		uint32_t recharge_id;
		uint32_t buy_num;
		uint32_t limit_num;
		uint32_t buy_flag;
		int32_t length()
		{
			return sizeof(uint32_t)
				+ sizeof(uint32_t)
				+ sizeof(uint32_t)
				+ sizeof(uint32_t);
		}
	};


	char 					device_id[64];
	char 					account[32];
	uint32_t 				uin;
	uint32_t 				source;
	char 					nick[128];
	uint32_t 				vip;
	char 					mail[64];
	char 					mobile[32];
	uint32_t 				money;
	uint32_t 				coin;
	uint32_t 				ticket;
	uint32_t 				pkgsize;
	uint32_t 				signin;
	uint32_t 				bank_coin;
	uint32_t 				total_recharge;
	uint32_t 				newbie_flag;
	uint32_t 				trumpet;
	vector<AreaInfo> 		area_list;
	vector<FriendInfo> 		friend_list;
	vector<PkgInfo> 		pkg_list;
	vector<RechargeInfo> 	recharge_list;
	char 					channel[32];
	
	protoUserDataResp();

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

struct protoLoadUserDataReq : MSG_HEAD
{
	enum  { IDD = PROTO_AUTH_UINAUTHOK, };
	uint32_t	gamesvrId;
	uint32_t	uin;
	int32_t		new_user;		// 1 - new user, 0 - old user
	int32_t		dup_flag; 		// 1-duplicate login, 0-new user

	char		szDevice[64];
	char		szChannel[64];
	uint32_t	source;
	uint32_t	version;
	uint32_t	sessionId;
	uint32_t	clientIp;
	uint32_t	proxyId;
	uint32_t	channelId;


	protoLoadUserDataReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoLoadUserDataResp : MSG_HEAD
{
	enum { IDD = PROTO_GAME_UINACCEPT, };
	uint32_t	gamesvrId;
	uint32_t	uin;
	int32_t		new_user;

	char		szDevice[64];
	char		szChannel[64];
	uint32_t	source;
	uint32_t	version;
	uint32_t	sessionId;
	uint32_t	clientIp;
	uint32_t	proxyId;
	uint32_t	channelId;

	protoLoadUserDataResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoUserTickNotify : MSG_HEAD
{
	enum { IDD = PROTO_GAME_UINTICK, };	
	enum TickType
	{
		tickEnterGame,
		tickHeartbeat,
		tickLeaveGame,
	};

	uint32_t	gamesvrId;
	uint32_t	uin;
	int32_t		tickType;

	protoUserTickNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoGameSyncUserReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_SYNCUSERREQ, };

	struct UserSyncInfo
	{
		uint32_t	uin;
		uint32_t	clientIp;		// 可先不填
		uint32_t	sessionId;
		uint32_t	proxyId;	 	// 预留，先不填
		time_t		create_time;
		uint32_t	source;
		char		channel[32];
		uint32_t	version;
		char		deviceId[64];

		UserSyncInfo()
			: uin(0), clientIp(0), sessionId(0), proxyId(0), create_time(0), source(0), version(0)
		{
			memset(&channel[0], 0, sizeof(channel));
			memset(&deviceId[0], 0, sizeof(deviceId));
		}
		uint32_t getLength(void)
		{
			return sizeof(uint32_t)* 6 + sizeof(time_t)+LENGTH_STR(channel) + LENGTH_STR(deviceId);
		}
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	
	uint32_t					gamesvrId;
	std::vector<UserSyncInfo>	vecSyncInfo;

	protoGameSyncUserReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrSyncUserReq : MSG_HEAD
{
	enum { IDD = PROTO_SVR_SYNCUSER_REQ, };

	uint32_t svrId;

	protoSvrSyncUserReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};



struct protoGameCarryWealthReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_CARRY_WEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int32_t			ticket;
	int32_t			money;
	int32_t			reserve[6];

	protoGameCarryWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoGameSyncNick : MSG_HEAD
{
	enum { IDD = PROTO_GAME_SYNCNICK, };

	uint32_t 		uin;
	char			oldNick[64];
	char			newNick[64];

	protoGameSyncNick();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询丰游金币、奖券信息
struct protoQueryWealthReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_QUERYWEALTH, };

	uint32_t 		uin;

	protoQueryWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoQueryWealthResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTH_QUERYWEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int64_t			ticket;
	int32_t 		result;

	protoQueryWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 代出丰游金币、奖券
struct protoDrawWealthReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_DRAWWEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int64_t			ticket;
	int64_t			fishCoin;
	int64_t			fishTicket;

	protoDrawWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoDrawWealthResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTH_DRAWWEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int64_t			ticket;
	int64_t			leftCoin;
	int64_t			leftTicket;
	int32_t			result;

	protoDrawWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 代入丰游金币、奖券
struct protoDepositWealthReq : MSG_HEAD
{
	enum { IDD = PROTO_GAME_DEPOSITWEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int64_t			ticket;
	int64_t			fishCoin;
	int64_t			fishTicket;

	protoDepositWealthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoDepositWealthResp : MSG_HEAD
{
	enum { IDD = PROTO_AUTH_DEPOSITWEALTH, };

	uint32_t 		uin;
	int64_t			coin;
	int64_t			ticket;
	int64_t			leftCoin;
	int64_t			leftTicket;
	int32_t			result;

	protoDepositWealthResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 认证请求(新)
struct protoNewAuthorizeReq : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_AUTHORIZE_NEW, };

	char 		device_id[64];
	char 		account[32];
	char 		passwd[64];
	uint32_t 	new_flag;
	uint32_t 	source;
	uint32_t	chg_account_flag;
	uint32_t 	version;
	char 		channel[32];
	uint32_t 	ip;
	uint32_t	reserve1;
	uint32_t	reserve2;
	char		reserve3[512];

	protoNewAuthorizeReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 登出请求
struct protoLogoutRequest : MSG_HEAD
{
	enum { IDD = LOGOUT_REQUEST, };
	uint32_t 	uin;

	protoLogoutRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户分组信息
struct UserGroupInfo
{
	uint32_t level;
	char group[128];
	char key[128];
	uint32_t reverse1;
	char reverse2[128];

	UserGroupInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


// 用户分组请求
struct protoUserGroupRequest : MSG_HEAD
{
	enum { IDD = USER_GROUP_REQUEST, };
	uint32_t uin;
	uint32_t version;
	uint32_t source;
	char channel[64];
	char deviceId[64];
	int32_t isSimulator;
	uint32_t reverse1;
	char reverse2[128];

	protoUserGroupRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户分组应答
struct protoUserGroupResponse : MSG_HEAD
{
	enum { IDD = USER_GROUP_RESPONSE, };
	vector<UserGroupInfo> userGroupList;

	protoUserGroupResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户分组请求 gamesvr->competsvr
struct protoSvrUserGroupRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USER_GROUP_REQ, };
	uint32_t uin;
	uint32_t version;
	uint32_t source;
	char channel[64];
	char deviceId[64];
	int32_t isSimulator;
	uint32_t reverse1;
	char reverse2[128];

	protoSvrUserGroupRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户分组应答 competsvr->gamesvr
struct protoSvrUserGroupResponse : MSG_HEAD
{
	enum { IDD = PROTO_SVR_USER_GROUP_RESP, };
	uint32_t uin;
	vector<UserGroupInfo> userGroupList;

	protoSvrUserGroupResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 注册请求
struct protoRegisterRequest : MSG_HEAD
{
	enum { IDD = ACCOUTN_REGISTER_REQUEST, };
	
	char msisdn[16];
	uint32_t verifyCode;
	char password[32];
	char deviceId[64];
	uint32_t source;
	char channel[64];
	uint32_t	version;	
	uint32_t language;
	uint32_t reverse1;
	char reverse2[128];

	protoRegisterRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 注册应答
struct protoRegisterResponse : MSG_HEAD
{
	enum { IDD = ACCOUTN_REGISTER_RESPONSE, };

	enum ErrorCode
	{
		errSucceed = 0,	
		errIpForbid,			// 1 IP被禁
		errDevNull,				// 2 设备号为空
		errDevForbid,			// 3 设备被禁
		errVersion,				// 4 协议版本号不兼容,或来源非法
		errSysFault,			// 5 系统错误
		errVerifyCode,			// 6 验证码错误
		errMsisdnInUse,			//7 手机号已被使用
	};
	

	uint32_t result;
	char note[64];
	uint32_t	uin;
	char		nick[32];

	protoRegisterResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 注册校验请求
struct protoRegisterVerifyRequest : MSG_HEAD
{
	enum { IDD = ACCOUNT_REGISTER_VERIFY_REQUEST, };

	uint32_t countryCode;
	char msisdn[32];
	uint32_t source;
	char channel[64];	
	uint32_t reverse1;
	char reverse2[128];

	protoRegisterVerifyRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 注册校验应答
struct protoRegisterVerifyResponse : MSG_HEAD
{
	enum { IDD = ACCOUNT_REGISTER_VERIFY_RESPONE, };

	enum ErrorCode
	{
		errSucceed = 0,				//成功
		errMsisdnErr = 1,			//手机号码错误
		errMsisdnInUse = 2,			// 手机号已被使用
	};
	
	uint32_t result;

	protoRegisterVerifyResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 认证请求(加密)
struct protoAuthorizeReqV2 : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_AUTHORIZE_V2, };

	char 		account[64];						//账号信息
	char 		passwd[64];						//用户密码
	char 		device_id[64];					//设备类型
	char 		channel[64];						//渠道号
	uint32_t 	new_flag;						//是否新账号
	uint32_t 	account_type;				//账号类型 1：游客号， 2：手机注册账号， 3：第三方平台账号
	uint32_t 	source;							//来源号 
	uint32_t 	version;							//客户端版本
	uint32_t 	clientip;							//客户端IP
	uint8_t	    simulator;						//是否有模拟器
	uint8_t	    language;						//客户端语言版本
	uint32_t 	logintime;						//登陆时间
	uint32_t	reserve1;						//备用字段
	char		reserve2[1024];					//备用字段2
	char		sign[64];							//数据签名 数据签名密钥与来源号，版本号相关
														// MD5(account+passwd+device_id+channel+new_flag+account_type+source+version+clientip+simulator+language+logintime+reserve1+reserve2+key)

	protoAuthorizeReqV2();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 客户端校验失败通知
struct protoClientCheckFailureNotify : MSG_HEAD
{
	enum { IDD = HALL_CLIENT_CHECK_FAILURE_NOTIFY, };

	uint32_t uin;
	uint32_t times;
	char reserve1[256];

	protoClientCheckFailureNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 刷号标识通知
struct protoSvrBrushNumberFlagNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_BRUSH_NUMBER_FLAG_NOTIFY, };

	uint32_t uin;
	uint32_t flag;		// 0：正常 1：刷号

	protoSvrBrushNumberFlagNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 认证请求(加密)
struct protoAuthorizeReqV3 : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_AUTHORIZE_V3, };

	char data[1024];

	protoAuthorizeReqV3();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 认证请求(加密)
struct protoEncryptKeyNotify : MSG_HEAD
{
	enum { IDD = PROTO_ENCRYPT_KEY_NOTIFY, };

	char key[256];

	protoEncryptKeyNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 认证请求(加密)
struct protoAuthorizeReqV4 : MSG_HEAD
{
	enum { IDD = PROTO_CLIREQ_AUTHORIZE_V4, };

	char data[1024];
	char extend[2048];
	
	protoAuthorizeReqV4();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //  _GFX_PROXY_AUTHORIZEPROTOCOL_H_



