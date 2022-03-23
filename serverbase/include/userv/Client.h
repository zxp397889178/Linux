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
#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include "IClientMgr.h"
#include "Decoder.h"

class CClient : public mm::EventHandler
{
protected:
	enum STATE
	{
		statePending,		// accept 成功 
		stateActive,		// 有收到第一个包
        stateWsActive,      // websocket连接成功
		stateAuthorize,		// 服务器验证通过
		stateClosing,		// 等待关闭 （管理器移除映射表）
		stateClosed,		// 关闭
	};

public:
    CClient(mm::Reactor &reactor, IClientMgr* pClientMgr, uint32_t workerId, 
            int64_t clientId, int32_t fd, uint32_t ip, uint16_t serverPort, 
            enum NetMode netMode, enum HeaderType hdrType, bool bSsl);
    virtual ~CClient();

public:  // mm::EventHandler
	virtual bool handleRead(void) = 0;
	virtual bool handleWrite(void);
	virtual bool handleTimeout(unsigned int timerId);
	virtual bool handleClose(void);
	virtual bool handleError(int32_t events, mm::EventHandler *pHandler);
	virtual int32_t getSocket(void) const { return m_fd; }
    virtual bool confirmDispose();

public: 
    IClientMgr*     getClientMgr(void)  const { return const_cast<IClientMgr*>(m_pClientMgr); }
    uint16_t        getServerPort(void) const { return m_serverPort; } 
    enum NetMode    getNetMode(void)    const { return m_netMode; }
    enum HeaderType getHeaderType(void) const { return m_headerType; } 
    uint32_t        getWorkerId(void)   const { return m_workerId; }
    int32_t         getSck(void)        const { return m_fd; }
    int64_t         getClientId(void)   const { return m_clientId; }

    uint32_t        getIp(void)         const { return m_ip; }
    CMyString       getIpStr(void)      const { return m_strIp; }
    const char*     getIpCStr(void)     const { return m_strIp.c_str(); }

    void            setIp(const CMyString &ip);
    void            setIp(uint32_t ip);
    void            setPendingClose(void);
    //void            close(void);
    bool            needSend(void);

public:
    /* 发送消息的类
     * 参数：buff       数据缓冲区
     *       pData      数据缓冲区
     *       cbData     数据缓冲区大小
     * 返回值: <0       出错，与客户端断开
     *         =0       成功
     *         >0       发送的字节数，一般较少用
     */
    virtual int32_t SendMsg(CByteStream &buff);
    virtual int32_t SendMsg(void* pData, size_t cbData);

protected:
    bool IsBufferOverflow(void);

protected:
    IClientMgr* const       m_pClientMgr;
    const uint16_t          m_serverPort;
    const enum NetMode      m_netMode;
    const enum HeaderType   m_headerType;

    const int64_t           m_clientId;
    const uint32_t          m_workerId;
    int32_t                 m_fd;
    const bool              m_bSsl;

    uint32_t                m_ip;
    CMyString               m_strIp;

    enum STATE              m_eState;

protected:
    CByteStream             m_buffer;   // 接收缓存
    CByteStream             m_output;   // 发送缓存
    CDecoder                m_decoder;  // 解码器

protected:
    uint32_t                m_timerZombie;      // 连接上后，未发任何消息，认为是僵尸
    uint32_t                m_timerCloseWait;
};


