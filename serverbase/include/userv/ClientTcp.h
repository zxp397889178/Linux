/*
 * =====================================================================================
 *
 *       Filename:  ClientTcp.h
 *
 *    Description:  Tcp Client
 *
 *        Version:  1.0
 *        Created:  2021年08月24日 00时26分11秒
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

class CClientTcp : public CClient
{
public:
    CClientTcp(mm::Reactor &reactor, IClientMgr* pClientMgr, uint32_t workerId, int64_t clientId, int32_t fd, uint32_t ip, uint16_t serverPort, enum HeaderType hdrType, bool bSsl);
    virtual ~CClientTcp();

public:
    virtual bool handleRead(void);
    
protected:
    int32_t dealRequest(void);

};

