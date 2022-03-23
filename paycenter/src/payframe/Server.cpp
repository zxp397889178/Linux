/*
 * =====================================================================================
 *
 *       Filename:  Server.cpp
 *
 *    Description:  gate server implement
 *
 *        Version:  1.0
 *        Created:  2019年09月09日 00时59分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Server.h"
#include "MyNet.h"
#include <sys/types.h>
#include <sys/wait.h>
#include "Worker.h"
#include "Config.h"

extern bool g_bStop;

CServer::CServer()
    : m_nBalanceIndex(0)
{
}

CServer::~CServer()
{
    VEC_WORKER::iterator it(m_vecWorker.begin());
    for(; it != m_vecWorker.end(); ++it)
    {
        CWorker *pWorker = *it;
        if (NULL == pWorker)
        {
            continue;
        }
        pid_t pid = pWorker->getPid();
        ::kill(pid, SIGTERM);

        SAFE_DELETE(pWorker);
    }
    m_vecWorker.clear();
}

bool CServer::Init(void)
{
    if (!m_acceptor.Init())
    {
        LogFatal("open ports to listen failure.");
        return false;
    }

    uint32_t cfg_workerId = CConfig::Instance().getWorkerId();
    uint32_t cfg_workerCount = CConfig::Instance().getWorkerCount();
    uint32_t cfg_clientId = CConfig::Instance().getWorkerClientId();
    uint32_t cfg_clientMax = CConfig::Instance().getWorkerMaxConn();

    for(uint32_t ind=0; ind<cfg_workerCount; ++ind)
    {
        uint32_t workerId = cfg_workerId + ind;
        uint32_t clientId = cfg_clientId + ind*cfg_clientMax;
        uint32_t clientMaxId = clientId + cfg_clientMax;
        CWorker *pWorker = new CWorker(workerId, clientId, clientMaxId);
        m_vecWorker.push_back(pWorker);
        pWorker->Run();
    }
    m_nBalanceIndex = 0;

    return true;
}

void CServer::Run(void)
{
    uint64_t checkTime = Now();
    while(!g_bStop)
    {
        // 检查配置是否更新，以及进程是否存活
        uint64_t now = Now();
        if (now > checkTime + 5000)
        {
            CConfig::Instance().Update();

            this->checkWorker();
            checkTime = now;
        }

        // 接收客户端的连接
        VEC_NETACCEPT vecClient;
        int32_t nAcceptMax = CConfig::Instance().getAcceptParallel();
        int32_t ret = m_acceptor.Accept(vecClient, nAcceptMax);
        if (ret < 0)
        {
            g_bStop = true;
            continue;
        }

        // 分派到各Worker
        if (!vecClient.empty())
        {
            //printf("DEBUG>dispatch connect...\n");
            size_t cnt = m_vecWorker.size();
            size_t start  = m_nBalanceIndex;
            for(size_t idx(start); idx<(start+cnt); ++idx)
            {
                size_t workerIdx = idx % cnt;
                CWorker *pWorker = m_vecWorker.at(workerIdx);
                if (NULL == pWorker)
                {
                    continue;
                }
                int32_t ret = pWorker->AcceptSck(&vecClient[0], vecClient.size());
                if (0 == ret)
                {
                    // 分派成功, 下一个worker待命
                    m_nBalanceIndex = workerIdx;
                    ++m_nBalanceIndex;
                    break;
                }
            } // for(size_t idx...)

            // 没有可用的worker
            for(VEC_NETACCEPT::iterator it(vecClient.begin());
                    it != vecClient.end(); ++it)
            {
                int32_t &sck = *it;
                ::close(sck);
            }
        } // for(!vecNetAccept...

    } // while(true)

	// this->CheckWorker();
    LogFatal("server stop.\n");
}

void CServer::checkWorker(void)
{
	int32_t status(0);
	pid_t pid = ::waitpid(-1,&status, WNOHANG);
	if ((pid_t)-1 == pid)
	{
        // 通知“码农”群，程序CORE了
        this->postAlert(pid, status);

		if (ECHILD == errno)
		{
			LogErr("all worker exist, restart all");
			if (!g_bStop)
			{
				::kill(SIGHUP, SIGKILL);
			}
		}
		else
		{
			LogErr("wait error %d:%s", errno, strerror(errno));
		}
	}
	else if (pid != 0)
	{
        // 通知“码农”群，程序CORE了
        this->postAlert(pid, status);

		if (WIFEXITED(status))
		{
			LogErr("process %d exist %d, kill all", pid, WEXITSTATUS(status));
		}
		else if (WIFSTOPPED(status))
		{
			LogErr("process %d stopped %d, kill all", pid, WSTOPSIG(status));
		}
		else if (WIFSIGNALED(status))
		{
			LogErr("process %d signaled %d, kill all", pid, WTERMSIG(status));
		}
		else
		{
			LogErr("process %d not running %d, kill all", pid, status);
		}
        // TODO: 此处的代码好象执行有问题，无法把所有子进程都KILL掉
		if (!g_bStop)
		{
			::kill(SIGHUP, SIGKILL);
		}

        // TODO: 此处不论是否有异常，直接停掉
        g_bStop = true; 
	}
	else
	{
		bool good(true);
		for(size_t i(0); i<m_vecWorker.size(); ++i)
		{
			//TODO: 检查各个Worker的状态
		}
		if (good)
		{
			LogTrace("all workers OK");
		}
	}

	// TODO: 统计连接数
	//
}

int32_t getNameByPid(pid_t pid, CMyString &strApp, CMyString &strPath)
{
    //CMyString path1("/proc/%d/exe", pid);
    CMyString path1("/proc/self/exe");

    char dir[1024] = "";
    if (readlink(path1.c_str(), dir, sizeof(dir)) <= 0)
    {
        return -1;
    }

    char *path_end = strrchr(dir, '/');
    if (NULL == path_end)
    {
        return -2;
    }
    ++path_end;
    strApp = path_end;
    *path_end = '\0';
    strPath = dir;

    return 0;
}

void getLocalOutip(CMyString &strIp)
{
    http::CHttpClient net;
    CMyString strHeader;
    CMyString ipUrl;
    CONFIG.getIpUrl(ipUrl);
    net.HttpGet(ipUrl.c_str(), strIp, strHeader);
}

void CServer::postAlert(pid_t pid, int32_t status)
{
    CMyString alertUrl;
    CMyString alertIds;
    CMyString extraNote;
    bool val = CConfig::Instance().getAlert(alertUrl, alertIds, extraNote);
    if (!val)
    {
        return;
    }

    // 获得进程名
    CMyString strApp;
    CMyString strPath;
    int32_t ret = getNameByPid(pid, strApp, strPath);
    if (ret<0)
    {
        strApp = CConfig::Instance().getLogModule();
    }

    // 获得外网IP
    CMyString strIp;
    getLocalOutip(strIp);
    CMyString now = getGMTDate(::time(NULL));
    CMyString strNote("====== SERVER CRASH ======\ntime:%s\nprocess: %s (pid=%d)\npath:%s\nserver ip:%s\nsignal id:%d",
            now.c_str(), strApp.c_str(), pid, strPath.c_str(), strIp.c_str(), status);
    if (!extraNote.empty())
    {
        strNote.Append("\nnote:%s", extraNote.c_str());
    }
    
    // 发起警告
    CStrSpliter css;
    css.Split(alertIds, ',');
    for(int32_t i(0); i<css.size(); ++i)
    {
        CMyString strData("{\"chatId\":%s,\"text\":\"%s\"}", css[0].c_str(), strNote.c_str());
        http::VEC_STRING headers;
        headers.push_back("Content-Type: application/json");
        CMyString strHeader;
        CMyString strResp;
        http::CHttpClient net;
        net.HttpPostWithHeaders(alertUrl.c_str(), strResp, strHeader, headers, strData.c_str());
    }
}

