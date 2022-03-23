/*
 * =====================================================================================
 *
 *       Filename:  SvrProtocol.h
 *
 *    Description:  protocol of server
 *
 *        Version:  1.0
 *        Created:  04/11/2015 03:05:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_PROXY_SVRPROTOCOL_H_
#define _GFX_PROXY_SVRPROTOCOL_H_

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <vector>
#include "BaseCode/ByteOrder.h"
//#include "protoDef.h"

/*
 *====================================================================================
 * server 角色预定义
 *====================================================================================
 */
enum Actor
{
	actorDummy = 0,

	actorUpdsvr = 1,		// 自动更新服务器
	actorWebproxy = 2,		// webproxy, 所有WEB请求
	actorProxy = 3,			// proxy, 所有TCP请求
	actorGameSvr = 4,		// gamesvr, 处理游戏逻辑
	actorAuthSvr = 5,		// authsvr,	认证及用户数据服务器
	actorRechargesvr = 6,	// recharge,充值服务器
	actorBgsvr = 7,			// bgsvr, 后台服务、数据流水服务、全局服务器
	actorStatServer = 8,	// statsvr, 预计与bgsvr合并
	actorRobotSvr = 9,		// robotSvr 机器人服务器
	actorBehaveSvr = 10,		// behavesvr 客户端的行为服务器
	actorPartner = 11,		// 合作方认证服务器
	actorTableSvr = 12,		// 桌子服务器
	actorCompetSvr = 13,		// 比赛服务器
	actorMsgSvr = 14,		// 消息服务器
	actorAlarmsvr = 15,		// 监控报警服务器
	actorWebStore = 16,		// Web商城服务器
	actorAccountCenter = 17,	// 账号中心
	actorMallSvr = 18,		// 商城
	actorFishSvr = 19,		// 打鱼服务器
	actorHallSvr = 20,		// 大厅
	actorCenterSvr = 21,		// 中心节点
	actorActivitysvr = 22,  //activitysvr
	actorBusinesssvr = 23,		// 业务服务器
	actorLogsvr = 24,			// 日志服务器
	actorHallSvrSync = 25, 		// hallsvr 同步访问
	actorGameClient = 100,
};

/*
 *====================================================================================
 * server 的clientId 预定义
 *====================================================================================
 */
#define CLIENTID_RECHARGESVR		0xFFFE0001
#define CLIENTID_GAMESVR			0xFFFE0002
#define CLIENTID_PROXYSVR			0xFFFE0003
#define CLIENTID_WEBPROXY			0xFFFE0005
#define CLIENTID_WEBSTAT			0xFFFE0006
#define CLIENTID_STATSVR			0xFFFE0007
#define CLIENTID_GLOBALSVR			0xFFFE0008
#define CLIENTID_AUTHSVR			0xFFFE0009
#define CLIENTID_ACCOUNTCENTER		0xFFFE0009

// 消息同步头
const uint8_t g_cSyncCode[] = {219, 109, 'S', 'y', 'n', 'C', 85, 170};

/*
 *====================================================================================
 * server 之间通讯的记录头,主要是用于包含client的session信息
 *====================================================================================
 */
//记录头
struct Record {
	enum enumRecordFlag{
		FRM_FIRST = 0x01,				//首个请求包文
		FRM_LAST  = 0x02,				//最后一个请求包文
		FRM_ALL = FRM_FIRST | FRM_LAST,	//完整的数据包
		
		FRM_ERR = 0x04,					//错误包文
		FRM_NULL = 0x08,				//空包文，指没有附加数据
		
		HAS_ATTR = 0x10,				//带扩展属性数据
		
		END_OF_QUEUE = 0x20,			//队列结束,暂未用
		REQ_PENDING = 0x40,				//正在处理中
		REQ_TRANSIT = 0x80,				//产中中转请求，暂未用
	};
	
	enum enumRecordmask {
		GAME_PROTOCOL = 0x01,			//游戏数据
		CHAT_PROTOCOL = 0x02,			//聊天数据，暂未用
		VOIP_PROTOCOL = 0x04,			//VOIP数据，暂未用
		INNER_PROTOCOL = 0x08,			//INNER protocol, not use for client
	};		

	uint8_t		synccode[8];	//if comminute error, will sync by this flag 
	uint32_t	recordLen;		//totalLen, include Record+protocol
	uint32_t	cmdId;			//通讯协议
	uint32_t	uin;			//客户端唯一ID
	uint32_t	sessionId;		//客户端套接字ID,仅proxy用
	uint32_t	clientIp;		//客户端IP
	uint16_t	flag;			//记录标识
	uint16_t	mask;			//预留

	Record() 
		: recordLen(0)
		, cmdId(0)  
		, uin(0)
		, sessionId(0)
		, clientIp(0)
		, flag(FRM_ALL)
		, mask(GAME_PROTOCOL)  
	{
		memcpy(&synccode[0], &g_cSyncCode[0], 8);
	}
	Record(uint32_t _protoLen, uint32_t _protoId, 
			uint32_t _uin, uint32_t _sessionId, uint32_t _clientIp)
		: recordLen(sizeof(Record)+_protoLen)
		, cmdId(_protoId)
		, uin(_uin)
		, sessionId(_sessionId)
		, clientIp(_clientIp)
		, flag(FRM_ALL)
		, mask(GAME_PROTOCOL)
	{
		memcpy(&synccode[0], &g_cSyncCode[0], 8);
	}
	void Cast(void)
	{
		recordLen	= CByteOrder::Cast(recordLen);
		cmdId 		= CByteOrder::Cast(cmdId);
		uin 		= CByteOrder::Cast(uin);
		sessionId 	= CByteOrder::Cast(sessionId);
		clientIp 	= CByteOrder::Cast(clientIp);
		flag 		= CByteOrder::Cast(cmdId);
		mask 		= CByteOrder::Cast(mask);
	}
	void Build(uint32_t _protoLen, uint32_t _protoId, 
			uint32_t _uin, uint32_t _sessionId, uint32_t _clientIp)
	{
		memcpy(&synccode[0], &g_cSyncCode[0], 8);
		recordLen 	= sizeof(Record)+_protoLen;
		cmdId		= _protoId;
		uin			= _uin;
		sessionId	= _sessionId;
		clientIp	= _clientIp;
		flag		= FRM_ALL;
		mask		= GAME_PROTOCOL;
	}
};

//扩展属性结构 (暂未用)
struct BasicAttr {
	uint32_t	seqId;			//客户端消息序号
	uint64_t	reqTime;		//proxy接收到客户端的请求时间,记录到微秒
	uint64_t	revStamp[6];	//服务端的时间戳，最多支持3级跳转记录，以微秒计算，暂未用
	uint8_t		deviceId[16];	//设备ID，唯一号，有可能为IMEI码
	uint32_t	version;		//版本号
	uint8_t		sessionKey[16];	//通讯KEY
	uint8_t		reverse[64];	//预留

};


/*
 *====================================================================================
 * client <--> server 通讯的协议头
 *====================================================================================
 */
struct MSG_HEAD
{
	uint32_t	protoLen;	// 消息长度，包括MSG_HEAD
	uint32_t	protoId;	// cmdId
	uint64_t	checkCode;	// 校验码，仅在client <--> proxy 之间有用，server不作校验

	MSG_HEAD()
		: protoLen(0)
		, protoId(0)
		, checkCode(0)
	{
	}
	MSG_HEAD(uint32_t _protoLen, uint32_t _protoId)
		: protoLen(_protoLen)
		, protoId(_protoId)
		, checkCode(0)  
	{
	}
	void Cast(void)
	{
		protoLen  = CByteOrder::Cast(protoLen);
		protoId   = CByteOrder::Cast(protoId);
		checkCode = CByteOrder::Cast(checkCode);
	}
};

/*
*====================================================================================
* protobuf的协议头
*====================================================================================
*/
struct BASE_MSG_INFO
{
	unsigned short	usLen;			// 消息长度
	unsigned short	usMsgId;		// 消息ID

	void Cast(void)
	{
		usLen = CByteOrder::Cast(usLen);
		usMsgId = CByteOrder::Cast(usMsgId);
	}
};

#define MSG_HEAD_LEN sizeof(BASE_MSG_INFO)


/*
 *====================================================================================
 * 
 *====================================================================================
 */

typedef int32_t		SOCKET;

/* 用于服务器之间转发消息的缓存结构
 * 使用方法如下：
 * 		protoAuthorizeReq req;
 * 		req....
 *
 * 		CServerMsgBuff tmpBuff(req.getprotoLen());
 * 		tmpBuff.BuildRecord(...)
 * 		int32_t ret = req.Serialize(tmpBuff.getProtoData(), tmpBuff.getProtoLen());
 *		...
 *		this->SendMsg(tmpBuff.getBuffer(), tmpBuff.getBufferLen());
 *		...
 *
 *		// tmpBuff 不需要释放内存，由类自行管理
 *
 */
class CServerMsgBuffer
{
public:
	/* protoLen 指消息的长度，不包括 Record头
	 * 
	 */
	explicit CServerMsgBuffer(uint32_t protoLen);
	~CServerMsgBuffer();

	// 构建内部消息流转的记录头
	void BuildRecord(uint32_t _protoLen, uint32_t _protoId, 
			uint32_t _uin, uint32_t _sessionId, uint32_t _clientIp);

	struct Record* getRecord(void);

	/* 用于消息转发的实际消息指针，以及长度
	 * 可以取出该指针用于消息结构的序列化操作
	 * */
	char*		   getProtoData(void);
	uint32_t	   getProtoLen(void);

	/* 服务器内部消息的实际缓存长度，及指针
	 * */
	uint32_t	   getBufferLen(void);
	void*		   getBuffer(void);

private:
	char		m_buffer[1024];
	char*		m_pBuf;
	uint32_t	m_cbBuf;
	bool		m_bNewAlloc;
};

// 内存辅助类
class CBufferGuard
{
public:
	CBufferGuard(void *ptr);
	~CBufferGuard();
	void Keep(bool val = true);
private:
	void*	m_ptr;
	bool  	m_autoRelease;
};


/* 生成服务器之间协议传输所需要的内存块 
 * 参数：	rec		消息头，唯一会改变的就是cmdId和recordLen
 * 			proto	传输的协议
 * 			pBuf	生成的内存块，若ret>0必须手动释放
 * 			cbBuf	内存块大小
 * 返回值： <0 		序列化协议出错,-n表示第n个域出错
 * 			>0		序列化的内存大小
 */
template<typename T>
int32_t generate_proto_data(struct Record &rec, T &proto, void *&pBuf, size_t &cbBuf)
{
	uint32_t protoLen = proto.protoLen;
	uint32_t protoId = proto.protoId;

	cbBuf = protoLen + sizeof(Record);	
	pBuf = calloc(sizeof(char), cbBuf+8);

	struct Record *pRecord = (struct Record*)pBuf;
	memcpy(pRecord, &rec, sizeof(struct Record));
	pRecord->cmdId     = protoId;
	pRecord->recordLen = cbBuf;
	pRecord->Cast();

	char *pc = (char*)pBuf + sizeof(struct Record);
	int32_t ret = proto.Serialize((void*)pc, protoLen);
	if (ret < 0)
	{
		cbBuf = 0;
		free(pBuf);
		pBuf = NULL;
		return ret;
	}

	//GenerateCheckCode(pc);

	return (int32_t)cbBuf;
}

/* 生成服务器之间协议传输所需要的内存块 , 与前一函数的区别在于不分配内存
 * 参数：	rec		消息头，唯一会改变的就是cmdId和recordLen
 * 			proto	传输的协议
 * 			pBuf	初始的内存块
 * 			cbBuf	内存块大小
 * 返回值： -99		内存不足
 * 			<0 		序列化协议出错,-n表示第n个域出错
 * 			>0		序列化的内存大小
 */
template<typename T>
int32_t generate_proto_data_ex(struct Record &rec, T &proto, void *pBuf, size_t cbBuf)
{
	uint32_t protoLen = proto.protoLen;
	uint32_t protoId = proto.protoId;

	uint32_t cbLen = protoLen + sizeof(Record);	
	if (cbLen > cbBuf)
	{
		return -99;
	}

	struct Record *pRecord = (struct Record*)pBuf;
	memcpy(pRecord, &rec, sizeof(struct Record));
	pRecord->cmdId     = protoId;
	pRecord->recordLen = cbBuf;
	pRecord->Cast();

	char *pc = (char*)pBuf + sizeof(struct Record);
	int32_t ret = proto.Serialize((void*)pc, protoLen);
	if (ret < 0)
	{
		return ret;
	}

	//GenerateCheckCode(pc);

	return (int32_t)cbBuf;
}

#endif // _GFX_PROXY_SVRPROTOCOL_H_
