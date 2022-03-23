/*
 * =====================================================================================
 *
 *       Filename:  Server.h
 *
 *    Description:  gate server 
 *
 *        Version:  1.0
 *        Created:  2019年09月09日 00时57分50秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_GATE_SERVER_H__
#define __WEBFRAME_GATE_SERVER_H__

#include "BaseCode.h"
#include "Acceptor.h"
#include <vector>


class CWorker;

class CServer
{
    typedef std::vector<CWorker*>   VEC_WORKER;
public:
    CServer();
    ~CServer();

    bool Init(void);
    void Run(void);

private:
    void checkWorker(void);
    void postAlert(pid_t pid, int32_t status);

private:
    uint32_t    m_nBalanceIndex;
    CAcceptor   m_acceptor;

    VEC_WORKER  m_vecWorker;
};

#endif // __WEBFRAME_GATE_SERVER_H__
