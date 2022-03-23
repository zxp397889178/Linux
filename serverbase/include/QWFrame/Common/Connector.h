/*
 * =====================================================================================
 *
 *       Filename:  Connector.h
 *
 *    Description:  通用的客户端连接器，用于连接到服务端，并处理消息
 *
 *        Version:  1.0
 *        Created:  2015年09月02日 11时01分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */
#ifndef _QW_GAMESVR_CONNECTOR_H_
#define _QW_GAMESVR_CONNECTOR_H_

#include "BaseCode.h"
#include "MyNet.h"
#include "IConnectorMgr.h"

#include "../Protocol/protoSystem.h"

class CConnector : public mm::EventHandler
{
public:
	CConnector(uint32_t svrId, uint32_t channelId
			, enum Actor actorSelf, enum Actor actor, const CMyString &actorName
			, IConnectorMgr *pConnectorMgr
			, const CMyString &strHost, uint32_t port
			, mm::Reactor &reactor);
	virtual ~CConnector();

	int32_t Init(void);
	void SendMsg(void *pBuf, size_t cbBuf);

	enum Actor  getActor(void) 		{ return m_actor; }
	enum Actor  getActorSelf(void)  { return m_actorSelf; }
	const char* getActorName(void) 	{ return m_actorName.c_str(); }
	uint32_t	getSvrId(void)		{ return m_svrId; }
	uint32_t	getId(void)  		{ return m_channelId; }
	void		setId(uint32_t nId) { m_channelId = nId; }

	uint32_t	getTotalSend(void)	{ return m_sendBytes; }
	uint32_t	getTotalRecv(void)	{ return m_recvBytes; }

public:
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(uint32_t timerId);
	virtual bool handleClose();
	virtual bool handleError(int events, mm::EventHandler *pHandler);
	virtual int32_t getSocket() const { return m_fd; }

	virtual bool confirmDispose(void);

private:
	int32_t Authorize(void);
	int32_t ProcessData(void);

private:
	enum Actor			m_actor;			// 对端的角色
	CMyString			m_actorName;		// 对端的角色名称
	IConnectorMgr*		m_pConnectorMgr;	// 连接管理器
	mm::Reactor&			m_reactor;			// 反应器
	CMyString			m_strHost;			// 对端IP
	uint32_t			m_port;				// 对端的port
	int32_t				m_fd;				// socket
	enum Actor			m_actorSelf;		// 自身的角色

private:
	ILock*				m_pLock;
	CByteStream			m_sendBuf;			// 发送消息缓存
	CByteStream			m_recvBuf;			// 接收消息缓存
	uint32_t			m_svrId;			// 服务器ID
	uint32_t			m_channelId;		// 频道ID，自身真实的ID

	uint32_t			m_sendBytes;
	uint32_t			m_recvBytes;
};

#endif // _QW_GAMESVR_CONNECTOR_H_



