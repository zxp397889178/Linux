/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.h
 *
 *    Description:  QWFrame
 *
 *        Version:  1.0
 *        Created:  2017年06月01日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef _QW_QWFRAME_H_
#define _QW_QWFRAME_H_


#include "QWFrame/QWFrameConfig.h"
#include "QWFrame/LogicHandle.h"
#include "QWFrame/NetworkHandle.h"
#include "QWFrame/DatabaseMgr.h"
#include "QWFrame/IMsgRegisterFactory.h"
#include "QWFrame/Protocol/protoUserData.h"
#include "QWFrame/IpTables.h"

class CQWFrame
{
public:
	static CQWFrame* Instance();
	~CQWFrame();


public:
	bool Init(int argc, char **argv, IMsgRegisterFactory* pFactory);
	void Run();
	void Exit();

	static void OnExit(int sig);
	static void OnCoreExit(int sig);

	bool LogicObjInit();
	void LogicObjRun();
	void LogicObjFinish();


	bool CreatePidFile(const string &programName);
public:
	CLogicHandle& GetLogicHandle() { return m_logicHandle; }
	CQWFrameConfig& GetFrameConfig() { return m_ojbFrameConfig; };
	CDatabaseMgr& GetDatabaseMgr() { return m_objDatabaseMgr; };
	CNetworkHandle& GetNetworkHandle() { return m_objNetworkHandle; };

	

private:
	CQWFrame();
	static CQWFrame* m_pInstance;

private:

	//网络线程模块管理
	CNetworkHandle m_objNetworkHandle;

	//逻辑线模块管理
	CLogicHandle m_logicHandle;
	CMyThread* m_pLogicThread;

	//数据库连接管理
	CDatabaseMgr	m_objDatabaseMgr;
	
	//配置管理
	CQWFrameConfig m_ojbFrameConfig;

	int32_t	m_nStatus;

	std::vector<IMessage*> m_vecMsg;
};

#define QW_FRAME CQWFrame::Instance() 
#define REGISTER_MSG(msgId, pObj, func, description) QW_FRAME->GetLogicHandle().GetSignal().Bind(msgId, pObj, &func); LogInit("INIT: Bind message[%u] success.",msgId);
#define REGISTER_HTTP_MSG(msgId, pObj, func, description) QW_FRAME->GetLogicHandle().GetHttpSignal().Bind(msgId, pObj, &func); LogInit("INIT: Bind http message[%s] success.",msgId);
#define REGISTER_WS_MSG(msgId, pObj, func, description) QW_FRAME->GetLogicHandle().GetWsSignal().Bind(msgId, pObj, &func); LogInit("INIT: Bind websocket protobuf message[%u] success.",msgId);
#define CONNECTION_MGR QW_FRAME->GetNetworkHandle().GetConnectionMgr()
#define DB_MGR QW_FRAME->GetDatabaseMgr()


////////////////////////////////////////////////////////////////////////////////////
//序列化，反序列化常用宏定义

#define _STR(T)		#T
#define STR(T)		_STR(T)
#define DESERIALIZE_REQ(T, data, len)		T req; int32_t ret = req.Deserialize(data, len); \
											if (ret < 0) { LogFatal("deserialize %s failure, ret=%d, datalen %u", STR(T), ret, len); return -1; }

#define SERIALIZE_RESP(T, resp)				uint32_t protoLen = resp.getProtoLen();\
											CTempBuffer tmpBuf(protoLen); \
											ret = resp.Serialize(tmpBuf.data(), protoLen); \
											if (ret < 0) { LogFatal("serialize %s failure, ret=%d, protoLen=%u", STR(T), ret, protoLen); return -10; }


#define PROTOBUF_PRASE(T, data, len)		T req; bool bRet = req.ParseFromArray(data, len); \
											if (!bRet) { LogFatal("deserialize %s failure, datalen %u", STR(T), len); return -1; }

#define SVRPROTOBUF_PRASE(T, data, len)		T req; bool bRet = req.ParseFromArray(data+sizeof(MSG_HEAD), len-sizeof(MSG_HEAD)); \
											if (!bRet) { LogFatal("deserialize %s failure, datalen %u", STR(T), len); return -1; }

#endif	// _QW_QWFRAME_H_
