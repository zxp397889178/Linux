/*
* =====================================================================================
*
*       Filename:  SessionHttp.h
*
*    Description:  for server connection
*
*        Version:  1.0
*        Created:  2015年09月08日 14时22分44秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
*   Organization:  www.skyunion.net(IGG)
*
* =====================================================================================
*/

#ifndef _QW_AUTHSVR_SESSIONHTTP_H_
#define _QW_AUTHSVR_SESSIONHTTP_H_

#include "Session.h"

#include "http/HttpRespone.h"

class CSessionHttp : public CSession
{
public:
	CSessionHttp(enum Actor actor, const CMyString &actorName
		, ISessionMgr *pSessionMgr
		, int32_t fd, mm::Reactor &reactor
		, uint32_t ip, const CMyString &strIp);
	virtual ~CSessionHttp();

public:
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(uint32_t timerId);
private:
	int32_t process_data(void);

public:
	void httpRespone(CByteStream &outBuff);

	void httpRespone(uint32_t code, CMyString &strStatus, CMyString &strData, http::CHttpData::NodeVector &vecHead);

	void errRespone(uint32_t code, const char *status, const char *data);
private:
	http::CHttpRequest			m_httpReq;
	http::CHttpRespone			m_httpResp;

	uint32_t					m_timerCloseWait;
	bool						m_bReceived;		// 是否已接收完数据
};

#endif // _QW_AUTHSVR_SESSION_H_


