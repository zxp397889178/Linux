/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  api gate server
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 16时11分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"
#include "Config.h"
#include "Server.h"
#include <unistd.h>
#include <signal.h>


bool g_bStop;
void OnTerminate(int sig)
{
    g_bStop = true;
    LogDebug("process #%u catch signal %u", getpid(), sig);
}

void Syntax(const char *app)
{
    std::cout << "syntax:" << app << " params" << std::endl;
    std::cout << "\t\t-c: -c <config.xml> start application with config" << std::endl;
    std::cout << "\t\t-s: stop application" << std::endl;
    std::cout << "\t\t-t: query application status." << std::endl;
}

int main(int argc, char **argv)
{
    CMyString strApp = argv[0];
    if (argc==1)
    {
        Syntax(strApp.c_str());
        return -1;
    }

    CMyString strConfig;
    int32_t ch(0);
    while((ch=getopt(argc, argv, "c:st")) != -1)
    {
        switch(ch)
        {
        case 'c':
            strConfig = optarg;
            break;
        case 's':
            // stop application
            break;
        case 't':
            // show application status
            break;
        default:
            Syntax(strApp.c_str());
            break;
        }
    }

    bool ret = CConfig::Instance().Load(strConfig);
    if (!ret)
    {
        return -1001;
    }
    FILE *fp(NULL);
    CMyString strPid = CConfig::Instance().getPid();
    if (!strPid.empty())
    {
        fp = fopen(strPid.c_str(), "w");
        if (NULL==fp)
        {
            LogFatal("already open same process.");
            return -1003;
        }
        fprintf(fp, "%u",getpid());
        fflush(fp);
        LogDebug("=== open %s, pid %u ===", argv[0], getpid());
    }
    CAutoFile autof(fp);

    signal(SIGINT, OnTerminate);
    signal(SIGTERM, OnTerminate);
    signal(SIGABRT, OnTerminate);
    //signal(SIGKILL, OnTerminate);
    signal(SIGPIPE, SIG_IGN);
    g_bStop = false;

    int32_t result(-2001);
    http::CHttpClient::GlobalInit();
    try
    {
        CServer serv;
        if (serv.Init())
        {
            serv.Run();
        }
        result = 0;
    }
    catch(std::exception &ex)
    {
        LogErr("%s, %s exit", ex.what(), strApp.c_str());
    }
    catch(...)
    {
        LogErr("catch unknown error, %s exit.", strApp.c_str());
    }

    http::CHttpClient::GlobalFini();
    LogImpt("%s has been terminated.", strApp.c_str());
    unlink(strPid.c_str());

    return result;
}


