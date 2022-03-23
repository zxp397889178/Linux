/*
 * =====================================================================================
 *
 *       Filename:  MyThread.cpp
 *
 *    Description:  wrapper of pthread
 *
 *        Version:  1.0
 *        Created:  12/03/2014 01:09:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#include "MyThread.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
//#include "BaseCode.h"
#include "TimeFunc.h"


extern pid_t gettid(void); 

IThreadEvent::~IThreadEvent()
{
}

CMyThread::CMyThread(IThreadEvent *pEvent, bool bAutoDestroyEvent)
	: m_thread()
	, m_pEvent(pEvent)
	, m_interval(0)
	, m_bDetached(true)
	, m_eStatus(CMyThread::PENDING)
	, m_bStopFlag(false)
	, m_tid((pid_t)-1)
	, m_pSemSuspend(NULL)
	, m_pSemResume(NULL)
	, m_pSemTrigger(NULL)
	, m_pMutexStop(NULL)
    , m_pEventLock(NULL)
{
	m_pSemSuspend = OpenSemaphore((uint32_t)0);
	m_pSemResume = OpenSemaphore((uint32_t)0);
	m_pSemTrigger = OpenSemaphore((uint32_t)0);
	m_pMutexStop = OpenMutex();
    m_pEventLock = OpenMutex();
	if (NULL==m_pSemSuspend || NULL==m_pSemResume || NULL==m_pSemTrigger
			|| NULL==m_pMutexStop)
	{
		THROW_EX(Bad_ConcurrentObj("create concurrent object failure."));
	}
	m_bAutoDestroyEvent = bAutoDestroyEvent;
}

CMyThread::~CMyThread()
{
	if (pthread_self() != m_thread)
	{
		// end by another thread
		pthread_cancel(m_thread);

		pthread_join(m_thread, NULL);
	}

	SAFE_DELETE(m_pSemSuspend);
	SAFE_DELETE(m_pSemResume);
	SAFE_DELETE(m_pSemTrigger);
	SAFE_DELETE(m_pMutexStop);
    SAFE_DELETE(m_pEventLock);

	// release event handler
	if (m_bAutoDestroyEvent)
	{
		SAFE_RELEASE(m_pEvent);
	}

    // release event contain
    for(auto it(m_event.begin()); it != m_event.end(); ++it)
    {
        IThreadEvent::Event &event = *it;
        if (NULL != event.pData)
        {
            free(event.pData);
        }
    }
}

int CMyThread::Create(uint32_t interval, bool bCreateSuspend)
{
	// pthread attr_t : // http://blog.sina.com.cn/s/blog_6dc9e4cf0100xcvp.html
	
	m_interval = interval;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int nDetachState(PTHREAD_CREATE_DETACHED);
	if (!m_bDetached)
	{
		nDetachState = PTHREAD_CREATE_JOINABLE;
	}
	pthread_attr_setdetachstate(&attr, nDetachState);

	int ret = pthread_create(&m_thread, &attr, CMyThread::Run, (void*)this);
	pthread_attr_destroy(&attr);
	if (0 != ret)
	{
		return -1;
	}
	if (!bCreateSuspend)
	{
		//if not suspend, run thread.
		m_pSemResume->Post();
	}

	return 0;
}

void* CMyThread::Run(void *args)
{
	CMyThread *pThread = static_cast<CMyThread*>(args);
	if (NULL == pThread)
	{
		return (void*)1;
	}

	pThread->m_tid = gettid();
	pThread->m_pSemResume->Wait();
	pThread->m_eStatus = CMyThread::RUNNING;
	IThreadEvent &thrEvent = *(pThread->m_pEvent);
	int32_t ret = thrEvent.OnThreadCreate();
	if (0 != ret)
	{
		pThread->m_eStatus = CMyThread::STOPPED;
		pThread->m_tid = (pid_t)-1;
		return (void*)2;
	}

	bool bRun(true);
	uint64_t nLoopBegin;
	uint32_t costOnce;
	try
	{
		do {
			// check whether thread is suspended
			if (pThread->m_pSemSuspend->TryWait(0))
			{
				pThread->m_eStatus = CMyThread::SUSPEND;
				pThread->m_pSemResume->Wait();
				pThread->m_eStatus = CMyThread::RUNNING;

				//after wake up, check whether thread has been stopped
				pThread->m_pMutexStop->Lock();
				bRun = !pThread->m_bStopFlag;
				pThread->m_pMutexStop->Unlock();
				if (!bRun)
				{
					break;
				}
			}

			nLoopBegin = Now(false);
			// run thread main process
			ret = thrEvent.OnThreadProcess();
			if (ret != 0)
			{
				break;
			}

			// run trigger event 
			if (pThread->m_pSemTrigger->TryWait(0))
			{
                bool bBreak(false);
                do
                {
                    // TODO: 若解发太多事件时，有可能陷入长期循环
                    IThreadEvent::Event event;
                    if (!pThread->PopEvent(event))
                    {
                        break;
                    }
                    CAutoArrayEx guard((byte_t*)event.pData);
				    ret = thrEvent.OnThreadTrigger(event);
    				if (ret != 0)
	    			{
                        bBreak = true;
		                break;
				    }
                }while(true);
                if (bBreak) { break; }  // 退出主循环
			}

			costOnce = Now(false)-nLoopBegin;
			// get the sleep time for this loop
#if 1
			int32_t nWait(0);
			if (pThread->m_interval>0)
			{
				nWait = (pThread->m_interval>costOnce) ? (pThread->m_interval-costOnce) : 0;
				if (nWait>0)
				{
					nsleep(nWait);
				}
			}
#endif

			// check whether thread has been stopped
			pThread->m_pMutexStop->Lock();
			bRun = !pThread->m_bStopFlag;
			pThread->m_pMutexStop->Unlock();

		}while(bRun);
	}
	catch(std::exception &e)
	{
		fprintf(stderr, "\033[0;40;31m<pid:%d, tid:%d> thread exit:%s\033[0m\n", 
				getpid(), gettid(), e.what());
	}
	catch(...)
	{
		fprintf(stderr, "\033[0;40;31m<pid:%d, tid:%d> thread exit:[%d]%s\033[0m\n", 
				getpid(), gettid(), errno, strerror(errno));
	}

/*    
 *  // 已经退出循环了，不需要再执行Event
	// check trigger is post
	if (pThread->m_pSemTrigger->TryWait(0))
	{
		thrEvent.OnThreadTrigger();
	}
*/
	// destroy thread
	thrEvent.OnThreadDestroy();
	pThread->m_eStatus = CMyThread::STOPPED;
	pThread->m_tid = (pid_t)-1;

	return (void*)0;
}

void CMyThread::Resume(void)
{
	m_pSemResume->Post();
}

void CMyThread::Suspend(void)
{
	m_pSemSuspend->Post();
}

void CMyThread::Trigger(IThreadEvent::Event &event)
{
    {
        CGuard guard(m_pEventLock, false);
        m_event.push_back(event);
    }
	m_pSemTrigger->Post();
}

void CMyThread::Stop(void)
{
	m_pMutexStop->Lock();
	m_bStopFlag = true;
	m_pMutexStop->Unlock();
}

void CMyThread::release(void)
{
	static const int s_nMaxStep(50);

	switch(m_eStatus)
	{
	case CMyThread::SUSPEND:
		this->Resume();
		//go throught
	case CMyThread::RUNNING:
		//this->Trigger();
		this->Stop();
		break;
	default:
		break;
	}

	for(int nStep=0; nStep<s_nMaxStep; ++nStep)
	{
		if (m_eStatus==CMyThread::STOPPED)
		{
			break;
		}
		nsleep(100);
	}

	delete this;
}

bool CMyThread::PopEvent(IThreadEvent::Event &event)
{
    CGuard guard(m_pEventLock, false);
    if (m_event.empty())
    {
        return false;
    }
    event = m_event.front();
    m_event.pop_front();
    return true;
}

CMyThread* CMyThread::CreateNew(
		IThreadEvent *pEvent, uint32_t nInterval, bool bCreateSuspend, bool bAutoDestroyEvent)
{
	CMyThread *pThread(NULL);
	try
	{
		pThread = new CMyThread(pEvent, bAutoDestroyEvent);
	}
	catch(std::exception &e)
	{
		fprintf(stderr, "\033[0;40;31m%s\033[0m\n", e.what());
		SAFE_RELEASE(pEvent);
		return NULL;
	}
	catch(...)
	{
		fprintf(stderr, "\033[0;40;31m[%d]%s\033[0m\n", errno, strerror(errno));
		SAFE_RELEASE(pEvent);
		return NULL;
	}

	int ret = pThread->Create(nInterval, bCreateSuspend);
	if (ret != 0)
	{
		fprintf(stderr, "\033[0;40;31mcreate thread failure:[%d]%s\033[0m\n", errno, strerror(errno));
		SAFE_DELETE(pThread);
		return NULL;
	}

	return pThread;
}

