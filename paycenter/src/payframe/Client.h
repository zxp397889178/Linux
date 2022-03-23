/*
 * =====================================================================================
 *
 *       Filename:  Client.h
 *
 *    Description:  client handler
 *
 *        Version:  1.0
 *        Created:  09/10/2019 09:39:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_CLIENT_H__
#define __WEBFRAME_CLIENT_H__

#include "BaseCode.h"
#include "MyNet.h"
#include "IWebClientMgr.h"
#include "IClient.h"

class CClient : public mm::EventHandler
              , public IClient
{
	enum STATE
	{
		statePending,		// accept 成功 
		stateActive,		// 有收到第一个包
		stateAuthorize,		// 服务器验证通过
		stateClosing,		// 等待关闭 （管理器移除映射表）
		stateClosed,		// 关闭
	};
public:
    CClient(mm::Reactor &reactor, uint32_t id, int32_t fd, uint32_t ip, IWebClientMgr *pClientMgr, uint32_t workerId);
    virtual ~CClient();

public:  // mm::EventHandler
	virtual bool handleRead(void);
	virtual bool handleWrite(void);
	virtual bool handleTimeout(unsigned int timerId);
	virtual bool handleClose(void);
	virtual bool handleError(int32_t events, mm::EventHandler *pHandler);
	virtual int32_t getSocket(void) const { return m_fd; }

public: // IClient
    virtual void* allocateNewBlock(uint32_t cbSize);
    virtual void* getDataBlock(void) { return m_pDataBlock; }
    virtual uint32_t getClientId(void) const { return m_clientId; }
    virtual uint32_t getWorkerId(void) const { return m_workerId; }
    virtual uint32_t getIp(void) const { return m_ip; }
    virtual const char* getIpStr(void) { return m_strIp.c_str(); }

    virtual void setIp(const CMyString &strIp);

    virtual void SendMsg(CByteStream &buff);
    virtual void SendMsg(const void* pData, size_t cbData);

private:
	void httpResponse(uint32_t cmdId, CByteStream &outBuff);
	void httpResponse(uint32_t cmdId, uint32_t code, const char* status, const char* data = NULL);
	void httpResponse(uint32_t code, const char *status, 
            std::map< CMyString, CMyString > &header, 
            const char *pData, uint32_t cbData);
    int32_t dealHttpData(void);

private:
    IWebClientMgr*      m_pClientMgr;
    const uint32_t      m_clientId;
    int32_t             m_fd;
    uint32_t            m_ip; //2020-9-2
    const uint32_t      m_workerId;
    CMyString           m_strIp;

    enum STATE          m_eStatus;
    uint32_t            m_timerZombie;
    uint32_t            m_timerCloseWait;

    
	CByteStream				m_output;
	http::CHttpRequest		m_httpReq;
	http::CHttpRespone		m_httpResp;

private:
    int64_t        m_timeBegin;     // time when object create
    int64_t        m_timeReadBegin; // time begin read msg, in us
    int64_t        m_timeExecBegin; // time when begin execute cgi, in us
    int64_t        m_timeExecEnd;   // time when execute cgi finished, in us
    CMyString       m_cgiName;   // http cgi name, for cost time
    
private:
    // for extern data use.
    void*           m_pDataBlock;
};

#endif // __WEBFRAME_CLIENT_H__
