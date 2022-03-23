/*
 * =====================================================================================
 *
 *       Filename:  Worker.cpp
 *
 *    Description:  appserver worker
 *
 *        Version:  1.0
 *        Created:  09/08/2019 11:10:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Worker.h"
#include "Config.h"

CWorker::CWorker(uint32_t workerId, uint32_t startId, uint32_t maxId)
    : m_pid(-1)
    , m_fd(-1)
    , m_workerId(workerId)
    , m_startId(startId)
    , m_maxId(maxId)
{
}

CWorker::~CWorker()
{
    LogDebug("worker #%u destroy.", m_workerId);
}

void CWorker::Run(void)
{
    if (m_fd != -1)
    {
        ::close(m_fd);
        m_fd = -1;
    }

    int32_t fd[2];
    ::socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    m_pid = fork();
    if (0 == m_pid)
    {
        // child process
        m_pid = getpid();
        LogImpt("worker #%u running, pid %d", m_workerId, m_pid);
        CONFIG.setProcessWorkerId(m_workerId);

        ::close(fd[0]);
        m_fd = fd[1];
        bool ret = m_manager.Init(m_fd, m_workerId, m_startId, m_maxId);
        if (ret)
        {
            m_manager.Run();
        }
        //printf("DEBUG>sub-process #%u finished.\n", m_pid);
        LogImpt("worker #%u stopped, pid %d.", m_workerId, m_pid);
        exit(0);
    }
    else if (m_pid > 0)
    {
        // parent process
        m_pid = getpid();
        LogImpt("worker #%u parent running, pid %d", m_workerId, m_pid);

        ::close(fd[1]);
        m_fd = fd[0];
    }
    else
    {
        LogFatal("worker #%u failure, errno: %d, error: %s",
                m_workerId, errno, strerror(errno));
        exit(0);
    }
}

int32_t CWorker::AcceptSck(int32_t *fds, size_t cbFds)
{
    char c = '\0';
    struct iovec iov;
    iov.iov_base = &c;
    iov.iov_len = 1;

    size_t cbSock = sizeof(int32_t) * cbFds;
    size_t cbBuf = CMSG_SPACE(cbSock);
    CTempBuffer tmpBuf(cbBuf);
    char *buf = tmpBuf.data();
    struct msghdr msg = {0};
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = cbBuf;

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(cbSock);
    memcpy(CMSG_DATA(cmsg), fds, cbSock);

    size_t s = ::sendmsg(m_fd, &msg, 0);
    if (s < 0)
    {
        LogFatal("worker #%u accept client failure, errno:%d, error:%s",
                m_workerId, errno, strerror(errno));
        return -1;
    }
    LogDebug("worker #%u accept socket size %lu",
            m_workerId, cbFds);

    return 0;
}

