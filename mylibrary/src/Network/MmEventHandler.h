/***********************************************************************
 *
 * File:        EventHandler.h
 * Author:      iotazhang
 * Revision:    $Id: MmEventHandler.h 364262 2012-11-07 03:39:36Z iotazhang $
 * Description: 事件处理器, 处理Reator产生的读写与超时事件
 *
 ***********************************************************************/

#ifndef MM_PROXY_EVENT_HANDLER_H
#define MM_PROXY_EVENT_HANDLER_H

namespace mm {

class Reactor;

class EventHandler {
public:
    EventHandler();

    virtual ~EventHandler();
    virtual int getSocket() const = 0;
    Reactor* getReactor() const;
    void setReactor(Reactor* reactor);
    virtual bool handleRead() = 0;
    virtual bool handleWrite() = 0;
    virtual bool handleTimeout(unsigned int timerId) = 0;
    virtual bool handleClose() = 0;
    virtual bool confirmDispose();
    virtual bool handleError(int events, EventHandler* handler);
    int getEvents() const;
    void setEvents(int events);
    unsigned int setTimer(int timeout);
    void cancelTimer(unsigned int timerId);

protected:
    Reactor* _reactor;
    int _events;
};

} 

#endif

