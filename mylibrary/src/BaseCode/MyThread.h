/*
 * =====================================================================================
 *
 *       Filename:  MyThread.h
 *
 *    Description:  wrapper of pthread
 *
 *        Version:  1.0
 *        Created:  12/03/2014 01:06:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_MYTHREAD_H_
#define _MYLIB_BASECODE_MYTHREAD_H_

#include "mydef.h"
#include "myconf.h"
#include "IThreadEvent.h"
#include "ILock.h"
#include "IException.h"
#include "ObjectPtrGuard.hpp"
#include <pthread.h>
#include <vector>
#include <queue>

// Bad_ConcurrentObj
class CMyThread
{
	enum Status
	{
		PENDING,	// pending: first created
		RUNNING,
		SUSPEND,
		STOPPED,
	};
public:
	// pEvent ==> new IThreadEvent, the CMyThread will destroy pEvent
	CMyThread(IThreadEvent *pEvent, bool bAutoDestroyEvent=true);

	// interval ==> ms
	int Create(uint32_t interval = 100, bool bCreateSuspend = false);
	static CMyThread* CreateNew(IThreadEvent *pEvent, uint32_t nInterval=100,
			bool bCreateSuspend=false, bool bAutoDestroyEvent = true);
	void release(void);

	void Resume(void);
	void Suspend(void);
	void Stop(void);

	inline bool IsRunning(void) const { return m_eStatus==CMyThread::RUNNING; }
	inline bool IsSuspend(void) const { return m_eStatus==CMyThread::SUSPEND; }
	inline bool IsActive(void) const  
	{ return m_eStatus==CMyThread::RUNNING||m_eStatus==CMyThread::SUSPEND; }
	inline pid_t GetThreadId(void) const { return m_tid; }

    IThreadEvent* GetEvent(void) { return m_pEvent; }

	void Trigger(IThreadEvent::Event &event);
    bool PopEvent(IThreadEvent::Event &event);

protected:
	~CMyThread();

private:
	static void* Run(void *args);

private:
	pthread_t		m_thread;
	IThreadEvent*	m_pEvent;
	uint64_t		m_interval;		// in us

	bool			m_bDetached;
	enum Status		m_eStatus;
	bool			m_bStopFlag;
	pid_t			m_tid;			// thread id, not thread_self()

	ISemaphore*		m_pSemSuspend;
	ISemaphore*		m_pSemResume;
	ISemaphore*		m_pSemTrigger;
	ILock*			m_pMutexStop;

	bool			m_bAutoDestroyEvent;

private:
    // 事件所引用的容器
    std::deque<IThreadEvent::Event>     m_event;
    ILock*                              m_pEventLock;
};


#endif //_MYLIB_BASECODE_MYTHREAD_H_

