/*
 * =====================================================================================
 *
 *       Filename:  protoServer.h
 *
 *    Description:  between server comminute
 *
 *        Version:  1.0
 *        Created:  2015年09月04日 16时56分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_RECHARGE_SERVER_PROTO_SERVER_H_
#define _QW_RECHARGE_SERVER_PROTO_SERVER_H_

#include "protoBase.h"
#include "protoSerialize.h"
#include "protoRank.h"
#include "BaseCode.h"

struct protoAuthorize : MSG_HEAD
{
	enum { IDD = PROTO_SYS_AUTHORIZE, };

	uint32_t	actor;
	uint32_t	serverId;		// 服务器ID
	uint32_t	channelId;		// 频道ID, 一个服务器连接有多个频道
	uint8_t		key[64];
	char		szIp[32];
	int32_t		port;

	protoAuthorize(uint32_t _actor = 0, uint32_t _serverId = 0, uint32_t _channelId = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoServerAuthorizeResp : MSG_HEAD
{
	enum { IDD =  PROTO_SYS_AUTHORIZE_RESP, };	

	uint32_t	actor;
	uint32_t	serverId;
	uint32_t	channelId;
	int32_t		result;		// 1 - OK, 0 - fail

	protoServerAuthorizeResp(uint32_t _actor = 0, uint32_t _serverId = 0, uint32_t _channelId = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoHeartBeat : MSG_HEAD
{
	enum { IDD = PROTO_SYS_HEARTBEAT_EX, };

	uint8_t	extra_code[16];

	protoHeartBeat(uint32_t _actor = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//enum CloseReason
//{
//	closeUnknown = 0,						//0  未明确原因
//	closePassive = 1,						//1  客户端主动断开
//	closeZombie = 2,						//2  僵尸客户端,指连接上后未发任何消息
//	closeNotAuth = 3,						//3  客户端验证超时,指客户端在指定时间内验证合法性未通过
//	closeHeartBeat = 4,						//4  客户端心跳超时
//	closeDataFault = 5,						//5  客户端数据错误
//	closeServerReq = 6,						//6  服务端请求关闭连接
//	closeMemoryFault = 7,					//7  系统内存错误导致异常(暂未用)
//	closeSystemFault = 8,					//8  服务器系统错误导至客户端关闭
//	closeBlackList = 9,						//9  黑名单
//	closeDecrypt = 10,						//10 解密失败,加入黑名单
//	closeEncrypt = 11,						//11 加密失败
//	closeGameShutdown = 12,					//12 gameserver shutdown
//	closeLogicFault = 13,					//13 逻辑错误
//	closeHackJail = 14,						//14 黑客监狱
//	closeDuplicateLogin = 15,				//15 重复登陆
//	closeDuplicateLoginWithoutSaveData = 16,//16 重复登陆，且不保存数据
//	closeNetFault = 17,						//17 网络错误
//	closeAuthTimeout = 18,					//18 认证超时
//	closeAuthFault = 19,					//19 认证失败
//};


struct protoClientShutdown : MSG_HEAD
{
	enum {IDD = CLIENTCLOSEREQUEST , };

	uint32_t		uin;
	uint32_t		reason;

	protoClientShutdown(uint32_t _uin = 0, uint32_t _reason = 0);

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

struct protoClientShutdownResp : MSG_HEAD
{
	enum { IDD = CLIENT_CLOSE_RESPONSE, };

	uint32_t		uin;
	uint32_t		reason;

	protoClientShutdownResp();

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};


struct protoClientEnterHall : MSG_HEAD
{
	enum  { IDD = USER_ENTER_HALL_REQUEST, };

	uint32_t		uin;

	protoClientEnterHall(uint32_t _uin = 0);

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

//gamesvr发送用户数据
struct protoUserData : MSG_HEAD
{
	enum { IDD = PROTO_SYS_USERDATA_SEND, };

	uint32_t		uin;
	uint32_t		zipped;
	uint32_t		bufferLen;
	byte_t*			buffer;

	bool			bAllocated;

	protoUserData(uint32_t _uin = 0, uint32_t _zipped = 0, void *_buf = NULL, size_t _len = 0);

	~protoUserData();

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

//用户数据回写
struct protoUserDataWrite : MSG_HEAD
{
	enum { IDD = PROTO_SYS_USERDATA_WRITE, };

	uint32_t	uin;
	int32_t		level;
	int32_t		zip;
	uint32_t	cbData;
	void*		pData;

	bool		bAllocated;

	protoUserDataWrite(uint32_t _uin = 0, int32_t _level = 0, int32_t _zip = 0, uint32_t _cbData = 0, void *_pData = NULL);

	~protoUserDataWrite();

	int32_t Serialize(void *pBuf, size_t cbBuf);

	int32_t Deserialize(void *pBuf, size_t cbBuf);

	int32_t getProtoLen(void);
};

// 传到BgSvr写数据库日志
struct protoServerDbLog : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DBLOGGER, };

	int32_t		priority;		// 优先级: 1-9, 缺省为1级
	char		szTable[128];	// 表名
	char		szItem[5120];	// 项目数据

	protoServerDbLog();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 写log数据，传sql语句方式
struct protoServerDbLogEx : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DBLOGGER_EX, };

	uint32_t	uin;			// 账号ID
	char		sql[10240];		// SQL语句

	protoServerDbLogEx();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 用户数据传到BgSvr写数据库
struct protoBgUserDataRequest : MSG_HEAD
{
	enum { IDD = PROTO_BG_USERDATA_REQUEST, };

	int32_t		priority;		// 值定义 1-旧版gamesvr写数据   0-bgsvr写数据  2-bgsvr查用户数据
	uint32_t	index;			// 序号
	char		sql[65535];	// SQL语句

	protoBgUserDataRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoBgUserDataRespone : MSG_HEAD
{
	enum { IDD = PROTO_BG_USERDATA_RESPONE, };

	int32_t		result;			// 执行结果 0-成功， 其他值失败
	uint32_t	index;			// 序号
	protoBgUserDataRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求游服做每日重置
struct protoGameDailyResetReq : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DAILYRESET_REQ, };

	uint32_t	timeout;	// 最大允许的处理时间,ms,暂未用

	protoGameDailyResetReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 游服每日重置结果应答
struct protoGameDailyResetResp : MSG_HEAD
{
	enum { IDD = PROTO_SVR_DAILYRESET_RESP, };

	int32_t		result;		// 处理结果，暂未用

	protoGameDailyResetResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 奖券鱼命中判断请求
struct protoTicketHitReq : MSG_HEAD
{
	enum { IDD = PROTO_SVR_HITTICKET_REQ, };

	uint32_t	uin;
	uint32_t	roomId;
	uint32_t	tableId;
	uint32_t	fishTypeId;
	uint32_t	fishInstId;
	uint32_t	gunId;
	uint32_t	bulletScore;

	protoTicketHitReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 奖券鱼命中结果应答 
struct protoTicketHitResp : MSG_HEAD
{
	enum { IDD = PROTO_SVR_HITTICKET_RESP, };

	uint32_t	uin;
	uint32_t	roomId;
	uint32_t	tableId;
	uint32_t	fishTypeId;
	uint32_t	fishInstId;
	uint32_t	gunId;
	uint32_t	bulletScore;
	int32_t		result;		// -1 错误数据, 0 未命中, 1 命中

	protoTicketHitResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户进入比赛房的登记信息
struct protoEnterCompetRoomReq : MSG_HEAD
{
	enum { IDD = PROTO_SVR_ENTERCOMPET_REG ,};

	uint32_t	roomId;
	uint32_t	uin;
	char		szNick[32];

	protoEnterCompetRoomReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户比赛结果登记
struct protoCompetDataReg : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETDATA_REG, };

	uint32_t	roomId;
	uint32_t	uin;
	uint32_t	baseScore;
	uint32_t	plusScore;
	char		nick[32];
	uint32_t	vip;

	protoCompetDataReg();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户比赛结果返回
struct protoCompetDataResp : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETDATA_RESP, };

	uint32_t	uin;
	uint32_t	roomId;
	uint32_t	baseScore;
	uint32_t	plusScore;
	uint32_t	rank;
	uint32_t	leftTime;

	protoCompetDataResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 用户比赛房信息查询请求
struct protoQueryCompetDataReq : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_COMPETDATA_REQ, };

	uint32_t	uin;

	protoQueryCompetDataReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 用户比赛房信息结果返回
struct protoUserCompetDataResp : MSG_HEAD
{
	enum { IDD = PROTO_SVR_QUERY_COMPETDATA_RESP, };

	struct CompetData
	{
		uint32_t	roomId;			// 房间ID
		uint32_t	enterCount;		// 进入次数
		uint32_t	scoreMax;		// 用户最高分数
		uint32_t	scoreMaxInRoom;	// 房间最高分(仅指在查询时, 暂未用)

		CompetData();
		uint32_t getLength(void);
		int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
		int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
	};
	uint32_t					uin;
	std::vector<CompetData>		vecCompetData;

	protoUserCompetDataResp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 通知游服金币及奖券排行更新了
struct protoRankRenewNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_RANKRENEW_NOTIFY ,};

	protoRankRenewNotify(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 通知游服某个房间的排行榜更新了
struct protoCompetRenewNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETRENEW_NOTIFY, };

	uint32_t		roomId;

	protoCompetRenewNotify(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求获得金币及奖券排行数据
struct protoRankDataRequest : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_RANKDATA_REQUEST, };

	protoRankDataRequest(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 奖券及金币排行数据下发
struct protoRankDataRespone : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_RANKDATA_RESPONE, };

	typedef protoAllRankRespone::RankInfo	RankInfo;
	typedef std::vector<RankInfo>	RANKVEC;

	RANKVEC			vecRankCoin;
	RANKVEC			vecRankTicket;
	RANKVEC			vecRankOfferAReward;

	protoRankDataRespone(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 请求获得房间的排行榜
struct protoCompetRankRequest : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETRANK_REQUEST, };
	
	uint32_t		roomId;

	protoCompetRankRequest(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 房间排行榜数据下发
struct protoCompetRankRespone : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_COMPETRANK_RESPONE, };
	typedef std::vector<NewRankInfoVip> RANKVEC;

	uint32_t	roomId;
	RANKVEC		vecRankCompet;

	protoCompetRankRespone(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 清除指定房间的进入次数
struct protoCompetEnterDataReset : MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPETENTER_RESET,};

	uint32_t	roomId;

	protoCompetEnterDataReset(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 通知游戏服务器更新在线用户的奖励数据，并去获取mail
struct protoCompetRewardNotify: MSG_HEAD
{
	enum { IDD = PROTO_SVR_COMPET_REWARD_NOTIFY, };

	struct RewardData
	{
		uint32_t		uin;		// UIN
		uint32_t		rank;		// 排名
	};
	uint32_t			roomId;		// 房间ID
	time_t				begin_time;	// 房间开始的时间

	std::vector<RewardData>	reward_list;// 奖励列表（本数据不会发送）
	
	char				roomName[64];	// 房间名称
	char				champion[64];	// 冠军呢称
	uint32_t			cbOriginLen;	// 原始长度
	uint32_t			cbData;			// 奖励数据长度(压缩后的长度)
	void*				pData;			// 奖励数据(压缩后的数据)

	protoCompetRewardNotify(); 
	~protoCompetRewardNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf, uint32_t _protoLen);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

// 通知游服刷新日常
struct protoDailyResetNotify : MSG_HEAD
{
	enum { IDD = PROTO_DAILY_RESET_NOTIFY, };

	enum eFlag
	{
		Daily = 0,
		Weekly = 1,
	};
	uint32_t flag;
	protoDailyResetNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrUinLogout : MSG_HEAD
{
	enum { IDD = PROTO_SVR_UIN_LOGOUT, };

	uint32_t		uin;
	uint8_t			extra[64];

	explicit protoSvrUinLogout(uint32_t uin_ = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
	
};

struct protoGameSvrShutdown : MSG_HEAD
{
	enum { IDD = PROTO_GAME_SHUTDOWN , };

	uint32_t		gameId;
	uint8_t			extra[64];

	explicit protoGameSvrShutdown(uint32_t gameId_ = 0);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoGameSvrMigrateDataRequest : MSG_HEAD
{
	enum { IDD = PROTO_MIGRATE_DATA_REQUEST, };

	uint32_t		migrateId;	// 迁移ID，由proxy服务器处理
	uint32_t		sessionId;
	uint32_t		uin;
	uint32_t		fromGameId;
	uint32_t		toGameId;
	uint32_t		cbData;
	void*			pData;		// 用 calloc 分配内存，由析构函数释放，不需要手工释放
	uint32_t		extraId;	// 预留ID，由fromGameSvr生成

	protoGameSvrMigrateDataRequest();
	~protoGameSvrMigrateDataRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};
	
struct protoGameSvrMigrateDataRespone: MSG_HEAD
{
	enum { IDD = PROTO_MIGRATE_DATA_RESPONE, };
	enum enumResult{
		resultOK = 0,			// 处理成功，不做额外操作
		resultOKAndChangeGameSvr,		// 处理成功，切换游服

		resultSourceProtoFault = 100,	// 源端协议解析出错
		resultMigratePending,			// 正在数据传输中,需要让用户等待
		resultTransProtoFault,			// 转发协议封包出错
		resultServerOffline,			// 目的游服出错
		resultTargetReplyTimeout,		// 目的游服响应超时

		// > 200 错误号由目的服务器提供
		resultLoadUserFailure = 200,		// 目的游服加载用户失败
		resultSitDownFailure = 201,			// 目的游服坐下失败
	};

	uint32_t		migrateId;	 // 迁移ID,由请求协议生成，必须匹配
	uint32_t		sessionId;
	uint32_t		uin;
	uint32_t		fromGameId;
	uint32_t		toGameId;
	int32_t			replyToSource;	// 是否需要向请求方应答:0-不需要，1-需要
	int32_t			result;		 	// 处理结果，>=0&&<100 表示成功，>=100 表示失败
	uint32_t		extraId;		// 预留ID，由fromGameSvr生成维护

	protoGameSvrMigrateDataRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoUserChangeGameSvr : MSG_HEAD
{
	enum  { IDD = PROTO_AUTH_CHANGE_GAMESVR, };

	uint32_t	sessionId;
	uint32_t	uin;
	uint32_t	fromGameId;
	uint32_t	toGameId;

	protoUserChangeGameSvr();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoCompetRoomInfoNotify : MSG_HEAD
{
	enum  { IDD = PROTO_SVR_COMPETROOMINFO_NOTIFY, };

	uint32_t	roomId;
	time_t		openTime;
	time_t		endTime;
	int32_t		status;
	uint32_t	stayTime;
	time_t		nextTime;

	protoCompetRoomInfoNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 功能限制请求
struct protoFunctionLimitRequest : MSG_HEAD
{
	enum { IDD = FUNCTION_LIMIT_REQUEST, };

	uint32_t uin;
	uint32_t source;
	char channel[64];
	uint32_t version;

	protoFunctionLimitRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 功能限制响应
struct protoFunctionLimitRespone : MSG_HEAD
{
	enum { IDD = FUNCTION_LIMIT_RESPONE, };

	int32_t status;

	protoFunctionLimitRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 功能限制请求
struct protoFunctionRequest : MSG_HEAD
{
	enum { IDD = FUNCTION_REQUEST, };

	uint32_t uin;
	uint32_t type;

	protoFunctionRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct FunctionInfo
{
	uint32_t id;
	uint8_t status;

	FunctionInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

// 功能限制响应
struct protoFunctionResponse : MSG_HEAD
{
	enum { IDD = FUNCTION_RESPONSE, };

	std::vector<FunctionInfo> vecFunction;

	protoFunctionResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 功能开启通知
struct protoFunctionOpenNotify : MSG_HEAD
{
	enum { IDD = FUNCTION_OPEN_NOTIFY, };

	std::vector<uint32_t> vecId;

	protoFunctionOpenNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct UserLimitItem
{
	uint32_t 		limitId;	// 限制ID
	uint32_t		status;		// UIN
	uint32_t		reserve1;		// 预留1
	char			reserve2[256];	// 预留2

	UserLimitItem();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


// 用户功能限制请求
struct protoUserLimitRequest : MSG_HEAD
{
	enum { IDD = USER_FUNCTION_LIMIT_REQUEST, };

	uint32_t uin;
	uint32_t limitId;
	uint32_t version;

	protoUserLimitRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 用户功能限制响应
struct protoUserLimitRespone : MSG_HEAD
{
	enum { IDD = USER_FUNCTION_LIMIT_RESPONSE, };

	uint32_t			result;		// 结果
	std::vector<UserLimitItem>	limit_list;// 用户限制列表


	protoUserLimitRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 新浪红包回调
struct protoSinaRedPacketNotify : MSG_HEAD
{
	enum { IDD = PROTO_SINA_RED_PACKET_NOTIFY, };

	uint32_t	uin;	// 用户ID
	char	szGiftKey[33];	// 用户使用的礼包码

	protoSinaRedPacketNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 扣金币请求
struct protoSvrDeductCoinRequest : MSG_HEAD
{
	enum { IDD = PROTO_HALL_DEDUCT_COIN_REQUEST, };

	uint32_t uin;
	uint32_t currencyCount;
    uint32_t currencyType;
	uint32_t svrId;
	uint32_t channelId;
	uint32_t id;
	uint32_t cbData;
	void* pData;			// 用 calloc 分配内存，由析构函数释放，不需要手工释放

	protoSvrDeductCoinRequest();
	~protoSvrDeductCoinRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoSvrDeductCoinResponse : MSG_HEAD
{
	enum { IDD = PROTO_HALL_DEDUCT_COIN_RESPONSE, };
	
	enum eResult
	{
		Success = 0,		// 成功
		Failure = 1,		// 失败
	};

	int32_t result;
	uint32_t uin;
    uint32_t currencyCount;
    uint32_t currencyType;
	uint32_t svrId;
	uint32_t channelId;
	uint32_t id;
	uint32_t cbData;
	void* pData;			// 用 calloc 分配内存，由析构函数释放，不需要手工释放

	protoSvrDeductCoinResponse();
	protoSvrDeductCoinResponse(protoSvrDeductCoinRequest &req);
	~protoSvrDeductCoinResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoIappPayLimitRequest : MSG_HEAD
{
    enum { IDD = IAPPPAY_LIMIT_REQUEST, };

    uint32_t uin;
    uint32_t type;

    protoIappPayLimitRequest();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    int32_t getProtoLen(void);
};

struct protoIappPayLimitResp : MSG_HEAD
{
    enum { IDD = IAPPPAY_LIMIT_RESPONSE, };

    uint32_t	status;
    uint32_t	timeBegin;
    uint32_t	timeEnd;

    protoIappPayLimitResp();
    int32_t Serialize(void *pBuf, size_t cbBuf);
    int32_t Deserialize(void *pBuf, size_t cbBuf);
    uint32_t getProtoLen(void);
};

struct protoUpdatePictureNotify : MSG_HEAD
{
	enum { IDD = PROTO_SVR_UPDATE_PICTURE_NOTIFY, };

	uint32_t	nUin;
	char			szPicture[128];
	 /*
	status:
		0:审核通过
		3:审核中
	*/
	int32_t      nStatus;


	protoUpdatePictureNotify();
	protoUpdatePictureNotify(uint32_t uin, const char* pszPicture);
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};












#endif // _QW_RECHARGE_SERVER_PROTO_SERVER_H_


