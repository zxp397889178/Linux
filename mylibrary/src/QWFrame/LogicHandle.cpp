/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "LogicHandle.h"
#include "QWFrame.h"
CLogicHandle::CLogicHandle() : m_timer(500, true)
{
	m_pLock = OpenRwLock();
	m_bNeedRepost = false;
	m_bDefaultMethod = false;
}

CLogicHandle::~CLogicHandle()
{

}


void CLogicHandle::release(void)
{

}


int32_t CLogicHandle::OnThreadCreate(void)
{
	m_threadId = gettid();
	LogInit("CLogicHandle::OnThreadCreate m_threadId=%u", m_threadId);

	//CMyString messageId("%u", protoSysRepostMsg::IDD);
	uint32_t messageId = protoSysRepostMsg::IDD;
	if (m_signal.Find(messageId))
	{
		m_bNeedRepost = true;
	}
	messageId = PROTO_SYS_DEFAULT_MSG;
	if (m_signal.Find(messageId))
	{
		m_bDefaultMethod = true;
	}
	return 0;
}


void CLogicHandle::OnThreadDestroy(void)
{
	return ;
}

int32_t CLogicHandle::OnThreadTrigger(void)
{
	return 0;
}

int32_t CLogicHandle::OnThreadProcess(void)
{
	LOGICREQ_VEC vecLogicReq;
	{
		CGuard guard(m_pLock, false);
		if (!m_vecLogicReq.empty())
		{
			vecLogicReq.swap(m_vecLogicReq);
		}
	}

	for (LOGICREQ_VEC::iterator it(vecLogicReq.begin()); it != vecLogicReq.end(); ++it)
	{
		LogicReq &req = *it;
		CBufferGuard guard(req.pData);

		struct Record &record = req.record;
		struct MSG_HEAD head;
		memcpy(&head, req.pData, sizeof(head));
		head.Cast();

		uint64_t begin = Now();		

		int32_t ret(0);
		try
		{
			//CMyString messageId("%u", head.protoId);

			LogDebug2("LOGIC:TCP", record.uin, "BEGIN exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u]", head.protoId, req.cbData, req.svrId, req.channelId);

			ret = m_signal.Handle(head.protoId, req);

			if (ret == -999)
			{
				if (m_bDefaultMethod)
				{
					uint32_t msgid = PROTO_SYS_DEFAULT_MSG;
					ret = m_signal.Handle(msgid, req);
				}
				else if (m_bNeedRepost)
				{
					protoSysRepostMsg protoRepostMsg;
					protoRepostMsg.msgId = head.protoId;

					size_t protoLen = req.cbData;
					CServerMsgBuffer tmpBuf(protoLen);
					memcpy(tmpBuf.getProtoData(), req.pData, protoLen);

					tmpBuf.BuildRecord(record.recordLen, record.cmdId, record.uin, record.sessionId, record.clientIp);

					protoRepostMsg.cbData = tmpBuf.getBufferLen();
					protoRepostMsg.pData = calloc(protoRepostMsg.cbData, sizeof(char));
					if (protoRepostMsg.pData != NULL)
					{
						memcpy(protoRepostMsg.pData, tmpBuf.getBuffer(), protoRepostMsg.cbData);
						this->PushMsg(protoRepostMsg);
					}
					ret = 0;
				}
			}
		}
		catch (...)
		{
			LogErr2("LOGIC", record.uin, "[session#%u]exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] occur ERROR",
				record.sessionId, head.protoId, req.cbData, req.svrId, req.channelId);
		}
		
		uint64_t cost = Now() - begin;		
		LogDebug2("LOGIC:TCP", record.uin, "END exec cmdId %u, cmdLen %u, cost %lu ms, from [svr#%u, ch#%u] result [%d]", head.protoId, req.cbData, cost, req.svrId, req.channelId, ret);

		if (ret < 0)
		{			
			// handler error
			LogErr2("LOGIC", record.uin, "[session#%u]exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] result [%d]",
				record.sessionId, head.protoId, req.cbData, req.svrId, req.channelId, ret);	
		}

		if (cost > 500)
		{			
			LogErr2("LOGIC", record.uin, "[session#%u]exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] cost [%lu ms]",
				record.sessionId, head.protoId, req.cbData, req.svrId, req.channelId, cost);	
		}
	}

	// http消息处理
	this->HttpMsgHandle();

	// protobuf消息处理
	this->WsMsgHandle();

	try
	{
		if(m_timer.ToNextTime(10))
		{
			// 10毫秒执行一次
			QW_FRAME->GetFrameConfig().Update();
			QW_FRAME->LogicObjRun();
		}
		
	}
	catch (std::exception &e)
	{
		LogFatal2("QWFRAME", 0, "error[%s]", e.what());
	}
	catch (...)
	{
		LogFatal2("QWFRAME", 0, "error[%d] error[%s]", errno, strerror(errno));
	}

	return 0;
}

bool CLogicHandle::Init(CQWFrame* pFrame)
{
	if (NULL == pFrame)
	{
		return false;
	}
	
	m_pFrame = pFrame;
	return true;
}

bool CLogicHandle::Run()
{
	return true;
}


void CLogicHandle::PushLogicReq(LogicReq &req)
{
	CGuard guard(m_pLock, false);
	m_vecLogicReq.push_back(req);
}

void CLogicHandle::PushHttpLogicReq(HttpLogicReq &req)
{
	CGuard guard(m_pLock, false);
	m_vecHttpLogicReq.push_back(req);
}

void CLogicHandle::PushWsReq(WsLogicReq &req)
{
	CGuard guard(m_pLock, false);
	m_vecWsReq.push_back(req);
}

void CLogicHandle::HttpMsgHandle()
{
	HTTPLOGICREQ_VEC vecHttpLogicReq;
	{
		CGuard guard(m_pLock, false);
		if (!m_vecHttpLogicReq.empty())
		{
			vecHttpLogicReq.swap(m_vecHttpLogicReq);
		}
	}

	for (HTTPLOGICREQ_VEC::iterator it(vecHttpLogicReq.begin()); it != vecHttpLogicReq.end(); ++it)
	{
		HttpLogicReq &req = *it;

		int32_t ret(0);
		try
		{
			LogDebug2("LOGIC:HTTP", 0, "BEGIN exec http url[%s]", req.httpData.getUrl().c_str());

			string url = req.httpData.getUrl();
			ret = m_httpSignal.Handle(url, req);

			if (ret < 0)
			{
				// handler error
				if (ret == -999)
				{
					// 查询下有没有注册默认的
					string url = "default";
					ret = m_httpSignal.Handle(url, req);
					if (ret == -999)
					{
						LogErr2("LOGIC:HTTP", 0, "END exec http url[%s]. url not found.", req.httpData.getUrl().c_str(), ret);
						req.respData.code = 404;
						req.respData.strStatus = "Not Found";
						req.respData.strResp = "Message Not Found";
					}
					else
					{
						LogErr2("LOGIC:HTTP", 0, "END exec http url[%s] error. result [%d]", url.c_str(), ret);
					}
					
				}
				else
				{
					LogErr2("LOGIC:HTTP", 0, "END exec http url[%s] error. result [%d]", req.httpData.getUrl().c_str(), ret);
				}
			}

			LogDebug2("LOGIC:HTTP", 0, "END exec http url[%s], resp code[%u], status[%s], user define head size[%lu], resp data[%s], sync=[%s]",
				req.httpData.getUrl().c_str(), req.respData.code, req.respData.strStatus.c_str(), req.respData.vecHead.size(), req.respData.strResp.c_str(), req.bSync ? "true":"false");

			if (req.bSync)
			{
				if (!m_pFrame->GetNetworkHandle().GetConnectionMgr().SendHttpMsg(req.nSessionId, req.respData))
				{
					LogErr2("LOGIC:HTTP", 0, "END exec http url[%s]. send msg to session [%u] failure.", req.httpData.getUrl().c_str(), req.nSessionId);
				}
			}
			

		}
		catch (...)
		{
			LogErr2("LOGIC:HTTP", 0, "END exec http url[%s] error. result [%d]", req.httpData.getUrl().c_str(), ret);
		}
		
	}

	return;
}


void CLogicHandle::WsMsgHandle()
{
	WSLOGICREQ_VEC vecWsReq;
	{
		CGuard guard(m_pLock, false);
		if (!m_vecWsReq.empty())
		{
			vecWsReq.swap(m_vecWsReq);
		}
	}

	for (WSLOGICREQ_VEC::iterator it(vecWsReq.begin()); it != vecWsReq.end(); ++it)
	{
		WsLogicReq &req = *it;
		CBufferGuard guard(req.pData);

		int32_t ret(0);
		try
		{
			//CMyString messageId("%u", head.protoId);

			LogDebug2("WEBSOCKET", 0, "BEGIN exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u]", req.msgId, req.cbData, req.svrId, req.channelId);

			ret = m_wsSignal.Handle(req.msgId, req);

			LogDebug2("WEBSOCKET", 0, "END exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] result [%d]", req.msgId, req.cbData, req.svrId, req.channelId, ret);
		}
		catch (...)
		{
			LogErr2("WEBSOCKET", 0, "exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] occur ERROR", req.msgId, req.cbData, req.svrId, req.channelId);
		}
		if (ret < 0)
		{
			// handler error
			LogErr2("WEBSOCKET", 0, "exec cmdId %u, cmdLen %u, from [svr#%u, ch#%u] result [%d]", req.msgId, req.cbData, req.svrId, req.channelId, ret);
		}
	}

	return;
}

