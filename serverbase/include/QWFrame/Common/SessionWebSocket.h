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

#ifndef _QW_AUTHSVR_SESSIONWEBSOCKET_H_
#define _QW_AUTHSVR_SESSIONWEBSOCKET_H_

#include "Session.h"

#include "http/HttpRespone.h"

class CSessionWebSocket : public CSession
{
public:
	CSessionWebSocket(enum Actor actor, const CMyString &actorName
		, ISessionMgr *pSessionMgr
		, int32_t fd, mm::Reactor &reactor
		, uint32_t ip, const CMyString &strIp);
	virtual ~CSessionWebSocket();


	void SendMsg(void *pBuf, size_t cbBuf);

	void OnClose();
public:
	virtual bool handleRead(void);
	virtual bool handleTimeout(uint32_t timerId);
private:
	int32_t process_data_ex(void);

	int32_t process_data(void);


private:
	void HttpResponse(uint32_t code, const char *status, const char *data, bool bSuccess);

	void HttpResponse(uint32_t code, const char *status, void *pData, size_t cbData);

	void SendMsg(CByteStream &outBuff);

	string ShaWithBase64(const string& data);

	void WebSocketAck();
	void WebSocketRemoveHeader();
	void WebSocketAddHeader(uint64_t length);

	string Base64Encode(const unsigned char * cRawData, unsigned int iLength);	

private:
	bool IsWebSocketConnected();

private:
	http::CHttpRequest			m_httpReq;
	http::CHttpRespone			m_httpResp;
	uint32_t					m_timerCloseWait;
	bool						m_bReceived;		// 是否已接收完数据
	CByteStream			m_buffer;
};

#endif // _QW_AUTHSVR_SESSIONWEBSOCKET_H_


