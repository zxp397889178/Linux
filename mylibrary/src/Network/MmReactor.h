/***********************************************************************
 *
 * File:        Reactor.h
 * Author:      iotazhang
 * Revision:    $Id: MmReactor.h 362747 2012-11-01 12:57:37Z iotazhang $
 * Description: ÊÂ¼þ·ÖÅÉÆ÷
 *
 ***********************************************************************/

#ifndef MM_PROXY_REACTOR_H
#define MM_PROXY_REACTOR_H

#include <sys/epoll.h>
#include <string>
#include <vector>
#include <exception>
#include "MmEventHandler.h"

#include <map>
#include <errno.h>
#include <string.h>

namespace mm {

using std::vector;
using std::map;
using std::exception;
using std::string;

class SysCallException : public exception {
public:
    SysCallException(int errCode, const string& errMsg, bool detail = true) : _errCode(errCode), _errMsg(errMsg) {
        if (detail) {
            _errMsg.append(", ").append(::strerror(errno));
        }
    }
    virtual ~SysCallException() throw () {}
    int getErrorCode() const throw () {
        return _errCode;
    }
    const char* what() const throw () {
        return _errMsg.c_str();
    }

protected:
    int     		_errCode;
	std::string  	_errMsg;
};


class SocketException : public SysCallException {
public:
    SocketException(const string& errMsg, bool detail = true)
        : SysCallException(errno, errMsg, detail) {}
    virtual ~SocketException() throw () {}
};

class Reactor {
public:
    struct Context {
        int events;
        EventHandler* handler;
    };
    Reactor(int pollSize = 30000);
    void run();
    bool addHandler(EventHandler* handler, int events = EPOLLIN | EPOLLOUT);
    void removeHandler(EventHandler* handler);
    unsigned int setTimer(EventHandler* handler, int timeout);
    void cancelTimer(unsigned int timerId);
    void addErrorHandler(EventHandler* handler);
private:
    void run(int timeout);
    void dispatch(int n);
    void fireTimeout();
    void fireError(int events, EventHandler* handler);
    void updateTime();
    int nextTimeout();
    struct Timer {
        Timer(unsigned int i, uint64_t t) : timerId(i), absTime(t) {}
        unsigned int timerId;
        uint64_t absTime;
        bool operator <(const Timer& t) const {
            return absTime > t.absTime;
        }
        void reset() {
            absTime = 0;
        }
    };
    struct Task {
        Task(EventHandler* h, int f) : handler(h), fd(f) {}

        EventHandler* handler;
        int fd;
    };
    int _pollfd;
    unsigned int _timerId;
    vector<Context> _ctxs;
    vector<epoll_event> _events;
    vector<Timer> _timerHeap;
    map<unsigned int, Task> _tasks;
    vector<EventHandler*> _errorHandlers;
    timeval _timeVal;
    uint64_t _currentMS;
};

} 

#endif
