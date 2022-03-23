/*
* =====================================================================================
*
*       Filename:  protoMail.h
*
*    Description:  Mail protocol
*
*        Version:  1.0
*        Created:  2015年10月08日 14时15分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_MAILPROTOCOL_H_
#define _GFX_PROXY_MAILPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct MailInfo
{
	uint32_t	mail_id;			//邮件ID
	char		sender_name[64];	//发件人名称
	char		title[64];			//邮件TITLE
	uint32_t	mail_type;			//邮件类型
	char 		send_time[16];		//邮件时间


	MailInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


//邮件列表请求
struct protoMailListRequest : MSG_HEAD
{
	enum { IDD = MAIL_LIST_REQUEST, };
	uint32_t	uin;

	protoMailListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//邮件列表应答
struct protoMailListRespone : MSG_HEAD
{
	enum { IDD = MAIL_LIST_RESPONE, };	

	std::vector<MailInfo> mail_list;

	protoMailListRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//邮件列表请求(支持翻页)
struct protoMailListRequestEx : MSG_HEAD
{
	enum { IDD = MAIL_LIST_REQUEST_EX, };
	uint32_t	uin;	
	uint32_t	pageindex;		// 页号，从0开始

	protoMailListRequestEx();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//邮件列表应答(支持翻页)
struct protoMailListResponeEx : MSG_HEAD
{
	enum { IDD = MAIL_LIST_RESPONE_EX, };	

	uint32_t	totalpages;		// 总页数
	uint32_t	pageindex;		// 页号，从0开始
	std::vector<MailInfo> mail_list;

	protoMailListResponeEx();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//读取邮件请求
struct protoReadMailRequest : MSG_HEAD
{
	enum { IDD = READ_MAIL_REQUEST, };
	uint32_t	mail_id;
	uint32_t	uin;

	protoReadMailRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//读取邮件应答
struct protoReadMailRespone : MSG_HEAD
{
	enum { IDD = READ_MAIL_RESPONE, };
//	uint32_t	mail_id;			//邮件ID
//	char		sender_name[64];	//发件人名称
//	uint32_t	mail_type;			//邮件类型
//	char		title[64];			//邮件TITLE
//	char		content[256];		//邮件内容
//	int32_t		money;				//趣玩币数量
//	int32_t		gold;				//金币数量
//	int32_t		ticket;				//奖券数量
//	char		items[512];			//邮寄道具列表

	uint32_t mail_id;				//邮件ID
	char content[256];				//邮件内容
	uint32_t coin;					//金币
	uint32_t ticket;				//奖券数量
	uint32_t money;					//趣玩币数量
	char items[128];				//邮寄道具列表
	
	
	protoReadMailRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取邮件奖励请求
struct protoGetMailAwardRequset : MSG_HEAD
{
	enum { IDD = GET_MAIL_AWARD_REQUEST, };
	uint32_t	uin;
	uint32_t	mail_id;
	
	protoGetMailAwardRequset();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取邮件奖励响应
struct protoGetMailAwardResponse : MSG_HEAD
{
	enum { IDD = GET_MAIL_AWARD_RESPONE, };

	enum RewardResult
	{
		Failure = -1,			// 失败
		Ingore = 0,				// 忽略
		Success = 1,			// 成功
	};

	int32_t result;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	char items[128];

	protoGetMailAwardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//获取奖励数据通知
struct protoGetMailAwardNotify : MSG_HEAD
{
	enum { IDD = GET_MAIL_AWARD_NOTIFY, };
	uint32_t	uin;
	int32_t		money;			//趣玩币数量
	int32_t		gold;			//金币数量
	int32_t		ticket;			//奖券数量
	char		items[512];		//邮寄道具列表

	protoGetMailAwardNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//用户新邮件通知 bgsvr->gamesvr(broadcast)
struct protoUserNewMailNotify : MSG_HEAD
{
	enum { IDD = PROTO_BG_USER_NEWMAIL_NOTIFY, };
	std::vector<uint32_t>	vecUin;

	protoUserNewMailNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//用户新邮件通知 gamesvr->user
struct protoNewMailNotify : MSG_HEAD
{
	enum { IDD = NEW_MAIL_NOTIFY, };
	std::vector<MailInfo> mail_list;

	protoNewMailNotify();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//用户新邮件通知 gamesvr->authsvr
struct protoSvrWriteMail : MSG_HEAD
{
	enum { IDD = PROTO_GAME_WRITE_MAIL, };

	enum MailType
	{
		GiveCoinMail = 100,				// 赠送金币邮件（海外）
		PresentMail = 101,				// 赠送道具邮件
		ClanDisbandMail = 102,			// 解散家族邮件
		ClanExpelMail = 103,			// 家族踢人邮件
		PresentCoinMail = 106,			// 赠送金币邮件（国内）
		PresentCrystalMail = 107,		// 赠送炮座邮件
		H5ShareMail = 108,				// H5分享
		H5AddDesktopMail = 109,			// 赠送水晶邮件
        AuctionRefund = 111,        	// 竞购货币返还邮件
        AuctionWinner = 112,        	// 竞购中奖通知邮件
		PresentTorpedoMail = 120,		// 赠送鱼雷邮件
        ForestPartWin = 121,        	// 森林舞会中奖邮件
        BetFishWin = 122,        		// 竞猜鱼中奖邮件
        RedPackageFish = 123,       	// 红包鱼
		ThirdCoinCompensation = 124,	// 第三方兑换丢失金币后补偿邮件
		PresentReturnMail = 125,		// 赠送鱼雷退回邮件
		JackpotWarRewardMail = 126,		// 奖池战争奖励发放邮件
		SweepingMinesReturnMail = 127,	// 红包扫雷退回未领红包邮件
		CustomPicAuditingMail = 128,	// 自定义头像审核邮件
        DriftBottleRefund = 129,        // 漂流瓶退回邮件
	};
	uint32_t uin;
	uint32_t mailType;
	uint32_t coin;
	uint32_t ticket;
	uint32_t money;
	char items[128];
	uint32_t sender;
	char senderName[128];

	protoSvrWriteMail();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


#endif //  _GFX_PROXY_MAILPROTOCOL_H_
