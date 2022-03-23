/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "QWFrame.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
CQWFrame* CQWFrame::m_pInstance = NULL;
CQWFrame::CQWFrame()
{
	m_nStatus = 0;
	m_pLogicThread = NULL;
}


CQWFrame::~CQWFrame()
{

}

CQWFrame* CQWFrame::Instance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CQWFrame();
	}
	return m_pInstance;
}

bool CQWFrame::Init(int argc, char **argv, IMsgRegisterFactory* pFactory)
{
	signal(SIGINT, CQWFrame::OnExit);
	signal(SIGTERM, CQWFrame::OnExit);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, CQWFrame::OnExit);
	signal(SIGSEGV, CQWFrame::OnCoreExit);
	

	string programName = argv[0];
	string programXml;
	if (argc > 1)
	{
		programXml = argv[1];
	}

	cout << "CQWFrame::Init begin..." << endl;

	LogInit("INIT: load config file frame.xml.");
	if (!m_ojbFrameConfig.Init(programXml))
	{
		LogFatal("ERR: load config file [frame.xml] failure.");
		return false;
	}
	
	if (!m_logicHandle.Init(this)) 
	{
		return false;
	}

	if (!m_objDatabaseMgr.Init()) 
	{
		return false;
	}

	// 消息初始化
	m_nStatus = 0;
	pFactory->Init();
	m_vecMsg =  pFactory->GetMessgeList();
	if (!this->LogicObjInit())
	{
		printf("\e[0;31m ERR:logic obj init failure.\e[1;31m\n\e[0m");
		return false;
	}

	//创建逻辑线程
	m_pLogicThread = CMyThread::CreateNew(&m_logicHandle, 1, true);
	if (NULL == m_pLogicThread)
	{
		LogFatal("CQWFrame::Init: create logic thread failure.");
		return false;
	}

	if (!m_objNetworkHandle.Init(this))
	{
		return false;
	}

	if (!this->CreatePidFile(programName))
	{
		return false;
	}
	
	LogInit("INIT: CQWFrame init OK");
	cout << "CQWFrame::Init success..." << endl;
	return true;
}


void CQWFrame::Run()
{
	// 逻辑线程开始
	m_pLogicThread->Resume();

	m_objNetworkHandle.Run();
}

void CQWFrame::Exit()
{
	LogImpt("Begin exit process...");
	//逻辑线程先停止
	m_pLogicThread->Stop();

	this->LogicObjFinish();

	m_objNetworkHandle.GetConnectionMgr().Finish();

	sleep(1);
	LogImpt("End exit process...");
	return;
}

void CQWFrame::OnExit(int sig)
{
	LogImpt("Begin exit process... sig=[%d]", sig);
	
	QW_FRAME->Exit();
	
	exit(0);
	return;
}

void CQWFrame::OnCoreExit(int sig)
{
	LogImpt2("EXIT", 0, "Begin exit process by core... sig=[%d]", sig);

	QW_FRAME->Exit();

	abort();
	return;
}

bool CQWFrame::LogicObjInit()
{
	for (std::vector<IMessage*>::iterator iter = m_vecMsg.begin(); iter != m_vecMsg.end(); ++iter)
	{
		IMessage *pMsg = *iter;
		if (!pMsg->Init())
		{
			return false;
		}
	}
	return true;
}

void CQWFrame::LogicObjRun()
{
	for (std::vector<IMessage*>::iterator iter = m_vecMsg.begin(); iter != m_vecMsg.end(); ++iter)
	{
		IMessage *pMsg = *iter;
		pMsg->Run();
	}
	return;
}

void CQWFrame::LogicObjFinish()
{
	for (std::vector<IMessage*>::iterator iter = m_vecMsg.begin(); iter != m_vecMsg.end(); ++iter)
	{
		IMessage *pMsg = *iter;
		pMsg->Finish();
	}
	return;
}

bool CQWFrame::CreatePidFile(const string &programName)
{
	CMyString strPidFile("%s.pid", programName.c_str());
	FILE *fp = fopen(strPidFile.c_str(), "w");
	if (NULL == fp)
	{
		LogFatal("can not open pid file[%s]", strPidFile.c_str());
		return false;
	}
	fprintf(fp, "%u", getpid());
	fflush(fp);

	fclose(fp);

	return true;
}

