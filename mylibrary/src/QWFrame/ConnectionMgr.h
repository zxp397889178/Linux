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

#ifndef _QW_CONNECTIONMGR_H_
#define _QW_CONNECTIONMGR_H_


#include "Common/Acceptor.h"
#include "Common/Session.h"
#include "Common/SessionSet.h"
#include "Common/Connector.h"
#include "Common/ConnectorSet.h"
#include "Common/SessionHttp.h"
#include "Common/SessionWebSocket.h"

#include <map>
#include "MyNet.h"
#include <typeinfo>
#include "protoBase.h"

typedef std::map<uint32_t, CAcceptor*>			ACCEPT_MAP;				// 贞听列表，				ACTOR->CAcceptor
typedef std::map<uint32_t, CSessionSet*>		SESSIONSET_MAP;			// 接受连接列表，			ACTOR->CSessionSet
typedef std::map<uint32_t, CConnectorSet*>		CONNECTOR_MAP;				// 连接列表，				ACTOR->CConnectorSet
typedef std::map<uint32_t, CSessionHttp*>		HTTPSESSION_MAP;				// http连接列表，				sessionId->CSessionHttp
typedef std::map<uint32_t, CSessionWebSocket*>		WSSESSION_MAP;				// http连接列表，				sessionId->CSessionWebSocket

class CConnectionMgr
{
public:
	CConnectionMgr();
	~CConnectionMgr();
	
public:
	bool Init();
	bool Run();
	void Finish();

	void AddAccept(uint32_t nActor, CAcceptor* pAcceptor);
	void AddConnector(uint32_t nActor, CConnectorSet* pConnector);

	void AddPendingSession(uint32_t nActor, CSession* pSession);
	void AddSession(uint32_t nActor, uint32_t svrId, uint32_t channelId, const byte_t *pToken, CSession* pSession);
	void AddHttpSession(uint32_t sessionId, CSessionHttp* pSession);
	void AddWebSocketSession(uint32_t sessionId, CSessionWebSocket* pSession);

	void RemoveAccept(CAcceptor* pAcceptor);
	void RemoveSession(CSession* pSession);
	void RemoveConnector(CConnector* pConnector);
	void RemoveHttpSession(CSessionHttp* pSessionHttp);
	void RemoveWebSocketSession(CSessionWebSocket* pSession);
public:
	// 发送数据给客户端
	template<typename T>
	bool SendMsgToSession(Actor actor, uint32_t svrId, uint32_t channelId, struct Record &record, T &resp)
	{
		uint32_t protoLen = resp.getProtoLen();
		CServerMsgBuffer tmpBuf(protoLen);
		tmpBuf.BuildRecord(protoLen, T::IDD, record.uin, record.sessionId, record.clientIp);
		int32_t ret = resp.Serialize(tmpBuf.getProtoData(), tmpBuf.getProtoLen());
		if (ret < 0)
		{
			LogFatal("serialize %s failure, ret=%d", typeid(T).name(), ret);
			return false;
		}

		return this->SendMsgToSession(actor, svrId, channelId, tmpBuf.getBuffer(), tmpBuf.getBufferLen());
	};

	// 发送数据到服务端
	template<typename T>
	bool SendMsgToSvr(Actor actor, struct Record &record, T &req)
	{
		uint32_t protoLen = req.getProtoLen();
		CServerMsgBuffer tmpBuf(protoLen);
		tmpBuf.BuildRecord(protoLen, T::IDD, record.uin, record.sessionId, record.clientIp);
		int32_t ret = req.Serialize(tmpBuf.getProtoData(), tmpBuf.getProtoLen());
		if (ret < 0)
		{
			LogFatal("serialize %s failure, ret=%d", typeid(T).name(), ret);
			return false;
		}

		return this->SendMsgToSvr(actor, tmpBuf.getBuffer(), tmpBuf.getBufferLen());
	};

	// 发送protobuf数据到服务端
	template<typename T>
	bool SendProtoMsgToSvr(Actor actor, struct Record &record, uint32_t msgId, T &msgProto)
	{
		uint32_t headLen = sizeof(MSG_HEAD);
		uint32_t protoLen = msgProto.ByteSize() + headLen;
		CServerMsgBuffer tmpBuf(protoLen);
		tmpBuf.BuildRecord(protoLen, msgId, record.uin, record.sessionId, record.clientIp);

		// 组装头
		struct MSG_HEAD head;
		head.protoLen = protoLen;
		head.protoId = msgId;
		head.Cast();

		memcpy(tmpBuf.getProtoData(), &head, headLen);

		msgProto.SerializeToArray(tmpBuf.getProtoData() + headLen, msgProto.ByteSize());

		return this->SendMsgToSvr(actor, tmpBuf.getBuffer(), tmpBuf.getBufferLen());
	};


	// 广播消息给客户端
	template<typename T>
	bool SendBroadcastMsg(Actor actor, T& req, uint32_t clientId = 0xFFFF0004)
	{
		uint32_t protoLen = req.getProtoLen();
		CServerMsgBuffer tmpBuf(protoLen);
		tmpBuf.BuildRecord(protoLen, T::IDD, 0, clientId, 0);

		int32_t ret = req.Serialize(tmpBuf.getProtoData(), tmpBuf.getProtoLen());
		if (ret < 0)
		{
			LogFatal("can not serialize %s failure, ret=%d", typeid(T).name(), ret);
			return false;
		}
		

		return this->SendBroadcastMsg(actor, tmpBuf.getBuffer(), tmpBuf.getBufferLen());
	}
public:
	// 发送数据给客户端
	bool SendMsgToSession(Actor actor, uint32_t svrId, uint32_t channelId, void *pData, uint32_t cbData);

	// 发送数据到服务端
	bool SendMsgToSvr(Actor actor, void *pData, uint32_t cbData);

	// 广播消息给客户端
	bool SendBroadcastMsg(Actor actor, void *pData, uint32_t cbData);

public:
	/*http*/
	bool SendHttpMsg(uint32_t sessionId, HttpRespData &respData);

public:
	// 发送数据给客户端
	template<typename T>
	bool SendWsMsg(uint32_t sessionId, uint32_t msgId, T &msgProto)
	{
		uint32_t protoLen = msgProto.ByteSize();
		struct BASE_MSG_INFO head;
		head.usMsgId = msgId;
		head.usLen = protoLen + MSG_HEAD_LEN;

		uint32_t msgLen = head.usLen;

		CTempBuffer buff(head.usLen);
		head.Cast();
		memcpy(buff.data(), &head, MSG_HEAD_LEN);

		msgProto.SerializeToArray(buff.data() + MSG_HEAD_LEN, protoLen);
		LogDebug("SendWsMsg to session[%u], msgid[%u], len[%d]", sessionId, msgId, msgLen);
		return this->SendWsMsg(sessionId, buff.data(), msgLen);
	};

	// websocket发送数据
	bool SendWsMsg(uint32_t sessionId, void *pData, uint32_t cbData);

	// 关闭websocket连接
	bool CloseWsSession(uint32_t sessionId);
private:
	ACCEPT_MAP			m_mapAcceptor;
	SESSIONSET_MAP		m_mapSession;
	CONNECTOR_MAP	m_mapConnector;
	HTTPSESSION_MAP	m_mapHttpSession;
	WSSESSION_MAP m_mapWsSession;
	ILock*				m_pLock;

};

#endif	// _QW_CONNECTIONMGR_H_
