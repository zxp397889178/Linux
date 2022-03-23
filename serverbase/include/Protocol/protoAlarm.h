/*
* =====================================================================================
*
*       Filename:  protoAlarm.h
*
*    Description:  监控报警
*
*        Version:  1.0
*        Created:  2016年8月5日
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wangjy
*   Organization:  none
*
* =====================================================================================
*/

#ifndef _PROTO_ALARM_H_
#define _PROTO_ALARM_H_

#include <string.h>
#include <vector>

#include "BaseCode.h"
#include "protoDef.h"
#include "protoBase.h"
#include "protoSerialize.h"

struct protoMachineAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_MACHINE_ALARM_REQUEST };

	uint32_t machineId;
	int32_t cpu;
	int32_t mem;
	int32_t disk;
	
	protoMachineAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoThreadAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_THREAD_ALARM_REQUEST };
	uint32_t nodeId;
	char nodeName[64];
	uint32_t id;
	char name[64];
	uint32_t runTime;
	uint32_t avgTime;
	uint32_t maxTime;
	char lastTime[32];
	char remark[64];
	
	protoThreadAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoConnectAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_CONNECT_ALARM_REQUEST };

	uint32_t nodeId;
	char nodeName[64];
	uint32_t id;
	char name[64];
	char lastConnectTime[32];
	char lastDisconnectTime[32];
	uint32_t status;
	uint32_t disconnectCount;
	uint32_t maxTime;
	uint32_t minTime;
	
	protoConnectAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

struct protoDbAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_DB_ALARM_REQUEST };

	uint32_t nodeId;
	char nodeName[64];
	uint32_t id;
	char name[64];
	char lastConnectTime[32];
	uint32_t intervalTime;
	uint32_t status;
	uint32_t disconnectCount;
	uint32_t maxTime;
	uint32_t minTime;
	uint32_t visitCount;
	uint32_t execFailCount;
	
	protoDbAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoNotifyAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_NOTIFY_ALARM_REQUEST };

	uint32_t nodeId;
	char nodeName[64];
	uint32_t id;
	char name[32];
	uint32_t level;
	char event[32];
	char createTime[32];
	char remark[512];

	protoNotifyAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};


struct protoNodeAlarmRequest : MSG_HEAD
{
	enum { IDD = PROTO_NODE_ALARM_REQUEST };

	uint32_t nodeId;
	char nodeName[64];	
	uint32_t processId;
	uint32_t count;
	uint32_t cpu;
	uint32_t mem;

	protoNodeAlarmRequest();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};

#endif //_PROTO_ALARM_H_

