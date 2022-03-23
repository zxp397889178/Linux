/*
 * =====================================================================================
 *
 *       Filename:  IThreadEvent.h
 *
 *    Description:  thread event interface
 *
 *        Version:  1.0
 *        Created:  12/03/2014 01:00:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_THREAD_EVENT_H_
#define _MYLIB_BASECODE_THREAD_EVENT_H_


class IThreadEvent
{
public:
    struct Event
    {
        int32_t     event;  // 自定义事件
        void*       pData;  // 内存，必须采用calloc/malloc 进行分配内存,允许为NULL
        size_t      cbData;
    };

	virtual void release(void) = 0;

	// pre action when thread will be work
	// if return != 0, the thread will no create
	virtual int32_t OnThreadCreate(void) = 0;

	virtual void OnThreadDestroy(void) = 0;

	// when trigger semaphore post, it will call once
	// if return != 0, the thread will stop
	virtual int32_t OnThreadTrigger(Event &event) = 0;

	// main thread process
	// if return != 0, the thread will stop
	virtual int32_t OnThreadProcess(void) = 0;

protected:
	virtual ~IThreadEvent() = 0;
};



#endif  // _MYLIB_BASECODE_THREAD_EVENT_H_

