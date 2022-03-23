/*
 * =====================================================================================
 *
 *       Filename:  Worker.h
 *
 *    Description:  server worker
 *
 *        Version:  1.0
 *        Created:  09/08/2019 11:08:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __WEBFRAME_WORKER_H__
#define __WEBFRAME_WORKER_H__

#include "BaseCode.h"
#include "Manager.h"

class CWorker
{
public:
    CWorker(uint32_t workerId, uint32_t startId, uint32_t maxId);
    ~CWorker();

    void Run(void);
    int32_t AcceptSck(int32_t *fds, size_t cbFds);

    pid_t getPid(void) { return m_pid; }
    uint32_t getWorkerId(void) { return m_workerId; }

private:
    pid_t       m_pid;
    int32_t     m_fd;

    const uint32_t  m_workerId;
    const uint32_t  m_startId;
    const uint32_t  m_maxId;
    
    CManager    m_manager;
};

#endif  // __WEBFRAME_WORKER_H__
