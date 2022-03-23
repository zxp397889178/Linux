/*
 * =====================================================================================
 *
 *       Filename:  ClientWs.h
 *
 *    Description:  Websocket Client
 *
 *        Version:  1.0
 *        Created:  2021年08月24日 00时02分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "Client.h"

class CClientWs : public CClient
{
public:
    CClientWs(mm::Reactor &reactor, IClientMgr* pClientMgr, 
            uint32_t workerId, int64_t clientId, int32_t fd, uint32_t ip, 
            uint16_t serverPort, enum HeaderType hdrType, bool bSsl);
    virtual ~CClientWs();

public:
    virtual bool handleRead(void);
    virtual bool handleTimeout(unsigned int timerId);

public:
    virtual int32_t SendMsg(CByteStream &buff);
    virtual int32_t SendMsg(void* pData, size_t cbData);

protected:
    bool IsBufferOverflow(void);

private:
    int32_t WebsocketPreDecode(bool &bContinue);
    int32_t dealRequest(void);

    bool websck_realIp(CWsWrapper &wsobj);

private:
    CByteStream     m_buffer_ws; //专为websocket的接收缓存

    bool            m_bWsConnect;   // websocket 连接上
    bool            m_bWsActive;    // websocket 确认连接
    uint32_t        m_timerWsActive;    // ACK成功后，等待后续的连接确认，否则认为连接超时
};


