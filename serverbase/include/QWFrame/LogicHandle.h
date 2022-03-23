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

#ifndef _QW_LOGICHANDLE_H_
#define _QW_LOGICHANDLE_H_

#include "BaseCode.h"
#include "Signal.hpp"
#include "Protocol/protoSysDef.h"
class CQWFrame;

class CLogicHandle : public IThreadEvent
{
public:
	CLogicHandle();
	~CLogicHandle();


public:
	virtual void release(void);
	virtual int32_t OnThreadCreate(void);
	virtual void OnThreadDestroy(void);
	virtual int32_t OnThreadTrigger(void);
	virtual int32_t OnThreadProcess(void);


public:
	CSignal<uint32_t, LogicReq&> &GetSignal()
	{
		return m_signal;
	}

	CSignal<string, HttpLogicReq&> &GetHttpSignal()
	{
		return m_httpSignal;
	}

	CSignal<uint32_t, WsLogicReq&> &GetWsSignal()
	{
		return m_wsSignal;
	}
public:
	bool Init(CQWFrame*);
	bool Run();

	void PushLogicReq(LogicReq &req);
	void PushHttpLogicReq(HttpLogicReq &req);
	void PushWsReq(WsLogicReq &req);
public:
	template<typename T>
	void PushMsg(T &req)
	{
		LogicReq logicReq;
		logicReq.cbData = req.getProtoLen();
		logicReq.pData = calloc(sizeof(char), logicReq.cbData + 8);
		req.Serialize(logicReq.pData, logicReq.cbData);
		this->PushLogicReq(logicReq);
	};

private:
	void HttpMsgHandle();

	// websocket protobuf协议处理
	void WsMsgHandle();
private:
	ILock*			m_pLock;
	LOGICREQ_VEC	m_vecLogicReq;
	HTTPLOGICREQ_VEC m_vecHttpLogicReq;
	WSLOGICREQ_VEC m_vecWsReq;
	CMyTimer m_timer;
	uint32_t m_threadId;
private:
	//业务回调
	CSignal<uint32_t, LogicReq&> m_signal;
	CSignal<string, HttpLogicReq&> m_httpSignal;
	CSignal<uint32_t, WsLogicReq&> m_wsSignal;
	CQWFrame* m_pFrame;

	bool m_bNeedRepost;			// 是否需要协议转发
	bool m_bDefaultMethod;		// 是否有默认处理函数, 设置了默认函数后，不支持转发函数，转发函数后续废除
};

#endif	// _QW_QWFRAME_H_
