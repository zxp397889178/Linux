/*
* =====================================================================================
*
*       Filename:  protoOperateActivity.h
*
*    Description:  运营活动协议
*
*        Version:  1.0
*        Created:  2016年7月19日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_OPERATE_ACTIVITY_EX_H_
#define _PROTO_OPERATE_ACTIVITY_EX_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct OpActivityInfo
{
	uint32_t acti_id;  	// 活动ID
	uint32_t type;		// 活动类型 100：充值回馈， 101：收集鱼种 102：消耗金币 103：单笔充值 201：中秋博饼 301：兑换商城
	uint32_t opType;	// 操作类型 1：收集类 2：游戏类 3：展示类
	char title[128];	//标题
	uint32_t priority;  //排序
	uint32_t status;    //状态
	uint32_t enter;		// 游戏入口放置， 1：活动列表 2：大厅 3：活动列表和大厅 4：捕鱼场
	
	OpActivityInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct OperateActivityDataEx
{
	uint32_t acti_id;
	uint32_t postion;
	char note[128];
	uint32_t status;
	char complete_value[128];
	char target_value[128];
	uint32_t rewardid;  			// 奖励

	OperateActivityDataEx();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

struct OperateActivityResult
{
	uint32_t do_id;		// 参与ID
	uint32_t do_time;	// 参与时间
	uint32_t status;	// 完成状态
	char note[128]; 	// 说明
	uint32_t rewardid;  // 奖励

	OperateActivityResult();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};


// --------------------新活动协议----------------------------------


// 开放活动列表请求 
struct protoOpActiListRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_LIST_REQUEST };

	uint32_t uin;

	protoOpActiListRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 开放活动列表响应 
struct protoOpActiListResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_LIST_RESPONSE };

	vector<OpActivityInfo> acti_list;

	protoOpActiListResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 活动详情请求
struct protoOpActiDetialRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DETIAL_REQUEST };

	uint32_t uin;
	uint32_t acti_id;

	protoOpActiDetialRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoOpActiDetialResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DETIAL_RESPONSE };

	uint32_t acti_id;  	//活动ID
	uint32_t type;		//活动类型 100：充值回馈， 101：收集鱼种 102：消耗金币 103：单笔充值 201：中秋博饼 301：兑换商城
	uint32_t op_type;	//操作类型 1：收集类 2：游戏类 3：展示类
	uint32_t status;	//活动状态
	char title[128];	//活动标题
	char detial[1024];	//详情
	char rule_url[256];	//规则说明url
	uint32_t begin_time;//开始时间
	uint32_t end_time;	//结束时间
	char extra[1024];	//活动特有数据，根据每个不同的活动定义数据结构

	vector<OperateActivityDataEx> acti_data; //活动数据
	
	protoOpActiDetialResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 参于活动
struct protoOpActiDoRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DO_REQUEST };

	uint32_t uin;
	uint32_t acti_id;

	char do_data[1024];		// 参与活动执行数据，根据每个不同的活动定义

	protoOpActiDoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoOpActiDoResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DO_RESPONSE };

	// 内容待定
	uint32_t acti_id;
	uint32_t result;

	char do_result[1024];		// 参与活动完成数据，根据每个不同的活动定义

	protoOpActiDoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询公告
struct protoOpActiNoticeRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_NOTICE_REQUEST };

	uint32_t uin;
	uint32_t acti_id;

	protoOpActiNoticeRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoOpActiNoticeResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_NOTICE_RESPONSE };
	uint32_t acti_id; 
	vector<string> notice_list;

	protoOpActiNoticeResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

// 查询参与结果
struct protoOpActiDoHistoryRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DO_HISTORY_REQUEST };

	uint32_t uin;
	uint32_t acti_id;
	
	char history_data[1024];		// 参与活动执行数据，根据每个不同的活动定义

	protoOpActiDoHistoryRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoOpActiDoHistoryResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DO_HISTORY_RESPONSE };

	// 内容待定
	uint32_t acti_id;	
	char history_result[1024];		// 参与活动执行数据，根据每个不同的活动定义

	vector<OperateActivityResult> do_result;

	protoOpActiDoHistoryResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 暂存，新版本后要删除
struct protoOpActiDoHistoryResponse_Old : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_DO_HISTORY_RESPONSE };

	// 内容待定
	uint32_t acti_id;	

	vector<OperateActivityResult> do_result;

	protoOpActiDoHistoryResponse_Old();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


// 领取奖励
struct protoOpActiRewardRequest : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_REWARD_REQUEST };

	uint32_t uin;
	uint32_t acti_id;
	char do_idlist[256];
	char reward_data[256];

	protoOpActiRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoOpActiRewardResponse : MSG_HEAD
{
	enum { IDD = PROTO_OP_ACTI_REWARD_RESPONSE };

	// 内容待定
	uint32_t result;
	uint32_t acti_id;
	char do_idlist[256];

	protoOpActiRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_OPERATE_ACTIVITY_H_

