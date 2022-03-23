/*
 * =====================================================================================
 *
 *       Filename:  ISessionMgr.h
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_AUTHSVR_SESSIONMGR_H_
#define _QW_AUTHSVR_SESSIONMGR_H_

#include <stdint.h>
#include <sys/types.h>
#include "protoBase.h"
#include "MyNet.h"
enum State
{
	stateInit = 0,		// 初始状态
	stateActive,		// 等待验证
	stateAuthorize,		// 已验证
	stateClosing,		// 关闭中
};

class CSession;
class CSessionHttp;
//class CHttpRequest;
class ISessionMgr
{
public:
	virtual ~ISessionMgr() { };
	virtual bool OnAcceptSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp) = 0;

	virtual int32_t OnSessionAuthorize(enum Actor actor, struct Record &record,
			void *pBuf, uint32_t cbBuf, CSession *pSession) = 0;

	virtual void OnRcvMsg(CSession *pSession, struct Record &record, void *pBuf, uint32_t cbBuf) = 0;

	virtual bool OnAcceptHttpSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp) = 0;
	virtual void OnRcvHttpMsg(CSessionHttp *pSession, http::CHttpRequest& pReq) = 0;
	
	virtual bool OnAcceptWebSck(uint32_t port, int32_t fd, uint32_t ip, const char *pszIp) = 0;
	virtual void OnRcvWebSocketMsg(CSession *pSession, uint32_t msgId, void *pBuf, uint32_t cbBuf) = 0;
};

#define TIMER_EVENT_BASE	20000200
#define TIMER_ZOMBIE		TIMER_EVENT_BASE+2	// 是否僵死连接
#define TIMER_AUTHORIZE		TIMER_EVENT_BASE+4	// 是否验证身份
#define TIMER_HEARTBEAT		TIMER_EVENT_BASE+8	// 心跳检查

#endif	// _QW_AUTHSVR_SESSIONMGR_H_

