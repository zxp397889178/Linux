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

#ifndef _QW_NETWORKHANDLE_H_
#define _QW_NETWORKHANDLE_H_


#include "ConnectionMgr.h"
#include "MyNet.h"
#include "Common/ISessionMgr.h"
#include "Common/IConnectorMgr.h"

class CQWFrame;
class CNetworkHandle : public mm::EventHandler
	, public IConnectorMgr
	, public ISessionMgr
{
public:
	CNetworkHandle();
	~CNetworkHandle();
	
public:
	bool Init(CQWFrame*);
	void Run();

	CConnectionMgr& GetConnectionMgr() { return m_objConnectMgr; }
//EventHandler
public:
	/**
	* 处理读事件
	*/
	virtual bool handleRead();

	/**
	* 处理写事件
	*/
	virtual bool handleWrite();

	/**
	* 处理超时事件
	*/
	virtual bool handleTimeout(unsigned int timerId);

	/**
	* 处理关闭事件
	* @return 是否delete此处理器
	*/
	virtual bool handleClose();

	/**
	* 出错处理后是否还需要销毁处理器
	* @return 是否需要delete此处理器
	*/
	virtual bool confirmDispose();

	/**
	* 处理错误事件
	* @param events epoll事件
	* @param handler 出现错误的处理器
	* @return 是否允许错误继续被其他处理器处理
	*/
	virtual bool handleError(int events, EventHandler* handler);

	virtual int32_t getSocket() const { return 0; }

	//session mgr
public:	
	bool			OnAcceptSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp);
	int32_t		OnSessionAuthorize(enum Actor actor, struct Record &record, void *pBuf, uint32_t cbBuf, CSession *pSession);
	void			OnRcvMsg(CSession *pSession, struct Record &record, void *pBuf, uint32_t cbBuf);

	virtual bool OnAcceptHttpSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp);
	virtual void OnRcvHttpMsg(CSessionHttp *pSession, http::CHttpRequest& pReq);


	// websocket protobuf协议
	virtual bool OnAcceptWebSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp);
	virtual void OnRcvWebSocketMsg(CSession *pSession, uint32_t msgId, void *pBuf, uint32_t cbBuf);

	//connector mgr
public:
	int32_t	OnConnectorRespone(CConnector *pConn, struct Record &record, void *pBuf, uint32_t cbBuf);
	void		OnConnectorShutdown(CConnector *pConn);

private:
	int32_t InitNetworkConnection();

private:
	CQWFrame* m_pFrame;

	//网络管理
	mm::Reactor			m_reactor;

	//网络连接管理
	CConnectionMgr		m_objConnectMgr;

	uint32_t			m_timerLoop;		// 主循环计时器ID
};

#endif	// _QW_NETWORKHANDLE_H_
