/*
 * =====================================================================================
 *
 *       Filename:  Acceptor.h
 *
 *    Description:  gate client acceptor
 *
 *        Version:  1.0
 *        Created:  2019年09月09日 01时11分11秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_CLIENT_ACCEPTOR_H__
#define __WEBFRAME_CLIENT_ACCEPTOR_H__

#include "BaseCode.h"
#include "Config.h"
#include <vector>

typedef std::vector<int32_t>    SOCK_VEC;
typedef struct NetCfg           NetInfo;
typedef std::vector<int32_t> VEC_NETACCEPT;

class CAcceptor
{
    typedef std::vector<NetInfo>    VEC_NETINFO;
public:
    CAcceptor();
    ~CAcceptor();

    int32_t Init(void);
    int32_t Accept(VEC_NETACCEPT &vecClient, uint32_t accMax);

    bool getNetInfo(uint32_t port, NetInfo &netinfo);

private:
    int32_t doAccept(fd_set& rd_events, fd_set& ex_events, 
            VEC_NETACCEPT &vecAcc, uint32_t accMax);

private:
    VEC_NETINFO     m_vecNetInfo;
    uint32_t        m_nBalanceIndex;
};

#endif // __WEBFRAME_CLIENT_ACCEPTOR_H__
