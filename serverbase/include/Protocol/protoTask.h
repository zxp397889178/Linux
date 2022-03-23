/*
* =====================================================================================
*
*       Filename:  protoTask.h
*
*    Description:  Task protocol
*
*        Version:  1.0
*        Created:  2015年09月25日 14时25分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  liuwx
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _GFX_PROXY_TASKPROTOCOL_H_
#define _GFX_PROXY_TASKPROTOCOL_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct TaskInfo
{
	uint32_t		task_id;		//任务ID
	uint32_t		status;			//任务状态
	char			complete_value[512];	//完成内容
	uint32_t		remaining_time;	//剩余时间

	TaskInfo();
	uint32_t getLength(void);
	int32_t Serialize(void* pBuf, size_t cbBuf, uint32_t &offset);
	int32_t Deserialize(void *pBuf, size_t cbBuf, uint32_t &offset);
};

//查询任务信息请求
struct QueryTaskInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_TASK_INFO_REQUEST, };
	uint32_t		uin;

	QueryTaskInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//接受任务请求
struct AcceptTaskRequest : MSG_HEAD
{
	enum { IDD = ACCEPT_TASK_REQUEST, };
	uint32_t		uin;
	uint32_t		task_id;

	AcceptTaskRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//完成任务请求
struct CompleteTaskRequest : MSG_HEAD
{
	enum { IDD = COMPLETE_TASK_REQUEST, };
	uint32_t		uin;
	uint32_t		task_id;

	CompleteTaskRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//任务信息应答
struct TaskInfoRespone : MSG_HEAD
{
	enum { IDD = TASK_INFO_NEW_RESPONE, };
	std::vector<TaskInfo> task_list;

	TaskInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询比赛活动任务信息请求
struct QueryEventTaskInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_EVENT_TASK_INFO_REQUEST, };
	uint32_t		uin;

	QueryEventTaskInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询比赛活动任务信息应答
struct QueryEventTaskInfoRespone : MSG_HEAD
{
	enum { IDD = QUERY_EVENT_TASK_INFO_RESPONE, };
	uint32_t		uin;

	QueryEventTaskInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//完成比赛活动任务信息请求
struct CompleteEventTaskRequest : MSG_HEAD
{
	enum { IDD = COMPLETE_EVENT_TASK_REQUEST, };
	uint32_t		uin;
	uint32_t		task_id;

	CompleteEventTaskRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//完成比赛活动任务信息应答
struct CompleteEventTaskRespone : MSG_HEAD
{
	enum { IDD = COMPLETE_EVENT_TASK_RESPONE, };
	uint32_t		task_id;

	CompleteEventTaskRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//活动任务信息应答
struct EventTaskInfoRespone: MSG_HEAD
{
	enum { IDD = EVENT_TASK_INFO_RESPONE, };
	std::vector<TaskInfo> task_list;

	EventTaskInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询活跃度任务信息请求
struct QueryLivenessTaskInfoRequest : MSG_HEAD
{
	enum { IDD = QUERY_LIVENESS_TASK_INFO_REQUEST, };
	uint32_t uin;

	QueryLivenessTaskInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//查询活跃度任务信息应答
struct QueryLivenessTaskInfoRespone : MSG_HEAD
{
	enum { IDD = QUERY_LIVENESS_TASK_INFO_RESPONE, };
	std::vector<TaskInfo> task_list;

	QueryLivenessTaskInfoRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取活跃度礼包请求
struct GetLivenessGiftRequest : MSG_HEAD
{
	enum { IDD = GET_LIVENESS_GIFT_REQUEST, };
	uint32_t		uin;
	uint32_t		task_id;

	GetLivenessGiftRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取活跃度礼包应答
struct GetLivenessGiftRespone : MSG_HEAD
{
	enum Result
	{
		OK = 1,						// 成功
	};

	enum { IDD = GET_LIVENESS_GIFT_RESPONE, };
	uint32_t		result;
	uint32_t		reward_id;

	GetLivenessGiftRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//获取活跃度礼包请求
struct GetTaskRewardRequest : MSG_HEAD
{
	enum { IDD = GET_TASK_REWARD_REQUEST, };
	uint32_t		uin;
	uint32_t		task_id;
	uint32_t		reward_type;	

	GetTaskRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//获取活跃度礼包应答
struct GetTaskRewardRespone : MSG_HEAD
{
	enum Result
	{
		OK = 1,						// 成功
	};

	enum { IDD = GET_TASK_REWARD_RESPONSE, };
	uint32_t		result;
	uint32_t		reward_id;
	uint32_t		reward_type;

	GetTaskRewardRespone();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};



//查询悬赏任务信息请求 OAR->OfferAReward
struct QueryOARTaskInfoRequest : MSG_HEAD
{
	enum { IDD = OFFERAREWARD_TASK_INFO_REQUEST, };
	uint32_t		uin;

	QueryOARTaskInfoRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//悬赏任务响应
struct QueryOARTaskInfoResponse : MSG_HEAD
{
	enum { IDD = OFFERAREWARD_TASK_INFO_RESPONSE, };
	uint32_t		finishTimes;
	uint32_t        totalTimes;
	uint32_t		totalScore;
	int32_t         status;
	uint32_t		currentScore;

	vector<TaskInfo> 		curTask;

	QueryOARTaskInfoResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//连续任务请求
struct protoContiTaskRequest : MSG_HEAD
{
	enum { IDD = CONTI_TASK_REQUEST, };
	uint32_t uin;
	uint32_t type;

	protoContiTaskRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//悬赏任务响应
struct protoContiTaskResponse : MSG_HEAD
{
	enum { IDD = CONTI_TASK_RESPONSE, };
	uint32_t taskId;
	uint32_t status;
	char completeValue[64];
	uint32_t leftTime;
	uint32_t taskNum;

	protoContiTaskResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};


//连续任务请求
struct protoRecvContiTaskRewardRequest : MSG_HEAD
{
	enum { IDD = RECV_CONTI_TASK_REWARD_REQUEST, };
	uint32_t		uin;

	protoRecvContiTaskRewardRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};

//悬赏任务响应
struct protoRecvContiTaskRewardResponse : MSG_HEAD
{
	enum { IDD = RECV_CONTI_TASK_REWARD_RESPONSE, };
	int32_t result;
	uint32_t rewardId;

	protoRecvContiTaskRewardResponse();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	uint32_t getProtoLen(void);
};




#endif //  _GFX_PROXY_TASKPROTOCOL_H_

