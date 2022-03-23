/*
 * =====================================================================================
 *
 *       Filename:  protoPhoneu.h
 *
 *    Description:  丰游第三方合作接入
 *
 *        Version:  1.0
 *        Created:  2016年01月15日 14时57分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_PHONEU_PARTNER_PROTOCOL_H_
#define _QW_CRAZYFISH_PHONEU_PARTNER_PROTOCOL_H_

#include "BaseCode.h"
#include "protoBase.h"

#define MAIN_PROTOCOL_BYGOLD            129
#define SUB_PROTOCOL_GOLD_OUT_APPLY     1
#define SUB_PROTOCOL_GOLD_OUT_FINISH    2
#define SUB_PROTOCOL_GOLD_OUT_CANCEL    3
#define SUB_PROTOCOL_GOLD_IN_APPLY      4
#define SUB_PROTOCOL_GOLD_IN_FINISH     5
#define SUB_PROTOCOL_USER_AUTH			6
#define SUB_PROTOCOL_GOLD_QUERY			7
#define SUB_PROTOCOL_USER_QUIT			8

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct MSG_HEAD_PHONEU
{
	uint32_t	msgLen;
	uint32_t	mainId;
	uint32_t	subId;
	uint32_t	msgId;	// 捕鱼游戏自行维护序号
	uint64_t	route;
	uint32_t	encryptionType;
	uint32_t	checksum;

	MSG_HEAD_PHONEU()
		: msgLen(0), mainId(0), subId(0), msgId(0), route(0), encryptionType(0), checksum(0)
	{
	}
	MSG_HEAD_PHONEU(uint32_t cmdId, uint32_t subCmdId)
		: msgLen(0), mainId(cmdId), subId(subCmdId), msgId(0), route(0)
		, encryptionType(0), checksum(0)
	{

	}
	void Dump(void)
	{
		printf("HEAD: msgLen  [%u]\n\t  mainId  [%u]\n\t  subId   [%u]\n\t  msgId   [%u]\n\t  route   [%lu]\n\t  encrypt [%u]\n\t  checksum[%u]\n",
				msgLen, mainId, subId, msgId, route, encryptionType, checksum);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 用户账号身份认证
struct PhoneuUserAuthReq : MSG_HEAD_PHONEU
{
	enum{ IDD = SUB_PROTOCOL_USER_AUTH, };

	int64_t	 lUserId;
	char	 key[32];

	PhoneuUserAuthReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

struct PhoneuUserAuthRsp : MSG_HEAD_PHONEU
{
	enum  { IDD = SUB_PROTOCOL_USER_AUTH, };

	int64_t	lUserId;
	int32_t rtn;

	PhoneuUserAuthRsp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 查询账号在丰游平台上的财富
struct PhoneuUserGoldReq: MSG_HEAD_PHONEU
{
	enum { IDD = SUB_PROTOCOL_GOLD_QUERY, };

	int64_t  lUserId;
	uint32_t lRoleId;
	char	 key[32];

	PhoneuUserGoldReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

struct PhoneuUserGoldRsp: MSG_HEAD_PHONEU
{
	enum { IDD = SUB_PROTOCOL_GOLD_QUERY, };

	int64_t		lUserId;
	uint32_t	lRoleId;
	int64_t		lGold;
	int64_t		lCoupon;

	PhoneuUserGoldRsp();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//申请金豆转出请求
struct TransferOutApplyReq : MSG_HEAD_PHONEU
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_APPLY, };

    int64_t 	lUserId;      //用户ID
    uint32_t 	lRoleId;      //
    char  		key[32];      //
	int64_t		lOutGold;
	int64_t		lOutCoupon;

	TransferOutApplyReq();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

//申请金豆转出响应
struct TransferOutApplyRsp : MSG_HEAD_PHONEU
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_APPLY, };

	int64_t lUserId;	  // 用户ID
	uint32_t lRoleId;
    int32_t rtn;          //处理结果
	char  sn[32];         //转出金豆单号
    int64_t lOutGold;     //转出金豆
    int64_t lOutCoupon;   //转出奖券
    int64_t lLeftGold;    //剩余金豆
    int64_t lLeftCoupon;  //剩余奖券

	TransferOutApplyRsp(); 
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//金豆转出完成请求
struct TransferOutFinishReq : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_FINISH, };

	int64_t lUserId;	// 用户ID
	uint32_t lRoleId;
	char key[32];
    char sn[32];        //转出金豆单号
	int64_t lOutGold;
	int64_t lOutCoupon;
	int64_t lTotalCoin;
	int64_t lTotalTicket;

	TransferOutFinishReq();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//金豆转出完成响应
struct TransferOutFinishRsp : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_FINISH, };

	int64_t lUserId;
	uint32_t lRoleId;
    int32_t  rtn;        //处理结果
	int64_t lOutGold;	 // 转出的金豆
	int64_t lOutCoupon;	 // 转出的奖券

	TransferOutFinishRsp();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);

	void Dump(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//金豆转出取消请求
struct TransferOutCancelReq  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_CANCEL, };

	int64_t lUserId;
	uint32_t lRoleId;
	char key[32];
    char sn[32];        //转出金豆单号

	TransferOutCancelReq();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//金豆转出取消响应
struct TransferOutCancelRsp  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_OUT_CANCEL, };

	int64_t lUserId;
	uint32_t lRoleId;
    int32_t  rtn;        //处理结果
	
	TransferOutCancelRsp();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//金豆转入申请请求
struct TransferInApplyReq  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_IN_APPLY , };

    int64_t lUserId;      		//用户ID
    uint32_t lRoleId;      		//
    char  key[32];      		//
    int64_t lInGold;        	//转入金豆数量
	int64_t lInCoupon;      	//转入奖券数量
    int64_t lTotalCoin;        	//转入金豆数量
	int64_t lTotalTicket;      	//转入奖券数量

	TransferInApplyReq();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//金豆转入申请响应
struct TransferInApplyRsp  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_IN_APPLY , };

	int64_t lUserId;
	uint32_t lRoleId;
    int32_t   rtn;        //处理结果
	char sn[32];
    int64_t lInGold;        	//转入金豆数量
	int64_t lInCoupon;      	//转入奖券数量
    int64_t lTotalGold;			//代入前金豆数量
	int64_t lTotalCoupon;		//代入前奖券数量

	TransferInApplyRsp();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//金豆转入完成请求
struct TransferInFinishReq  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_IN_FINISH, };

	int64_t lUserId;
	uint32_t lRoleId;
	char key[32];
    char sn[32];
	int64_t lLeftCoin;
	int64_t lLeftTicket;

	TransferInFinishReq();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

//金豆转入完成响应
struct TransferInFinishRsp  : MSG_HEAD_PHONEU 
{
	enum { IDD = SUB_PROTOCOL_GOLD_IN_FINISH, };

	int64_t lUserId;
	uint32_t lRoleId;
    int32_t   rtn;        //处理结果
	int64_t lInGold;	  // 转入的金豆
	int64_t lInCoupon;	  // 转入的奖券
	int64_t lTotalGold;	  // 丰游平台总金豆
	int64_t lTotalCoupon; // 丰游平台总奖券

	TransferInFinishRsp();  
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 退出的通知消息
struct PhoneuUserQuit : MSG_HEAD_PHONEU
{
	enum  { IDD = SUB_PROTOCOL_USER_QUIT, };
	int64_t		lUserId;
	uint32_t	lRoleId;
	char		key[32];
	int64_t		lTotalCoin;		// 用户在捕鱼游戏中的可用金币数	

	PhoneuUserQuit();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 丰游的心跳包
struct PhoneuHeartbeat : MSG_HEAD_PHONEU
{
	PhoneuHeartbeat();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif // _QW_CRAZYFISH_PHONEU_PARTNER_PROTOCOL_H_


