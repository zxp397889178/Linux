/***********************************************************************
 *
 * File:        Reactor.cpp
 * Author:      iotazhang
 * Revision:    $Id: MmReactor.cpp 364262 2012-11-07 03:39:36Z iotazhang $
 * Description: ÊÂ¼þ·ÖÅÉÆ÷
 *
 ***********************************************************************/


#include "MmReactor.h"
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <algorithm>
#include "BaseCode.h"



namespace mm {

using std::find;
using std::push_heap;
using std::pop_heap;
using std::max;
using std::min;
using std::make_pair;

static const Reactor::Context EMPTY_CTX = { 0, 0 };
static const epoll_event EMPTY_EVENT = { 0, { 0 } };
static const int MAX_EVENTS = 4096;
static const int INITIAL_EVENTS = 32;

Reactor::Reactor(int pollSize) : _pollfd(-1), _timerId(0), _events(INITIAL_EVENTS, EMPTY_EVENT) {
    if ((_pollfd = epoll_create(pollSize)) == -1) {
        throw SocketException("epoll_create error");
    }

    _ctxs.reserve(pollSize);
}

void Reactor::run() {
    while (true) {
        int timeout = nextTimeout();
        run(timeout);
    }
}

void Reactor::run(int timeout) {
    int n = epoll_wait(_pollfd, &_events[0], static_cast<int>(_events.size()), timeout);
    if (n == -1) {
        if (errno != EINTR) {
            throw SocketException("epoll_wait error");
        }
    }

    updateTime();
    dispatch(n);
    fireTimeout();
}

void Reactor::dispatch(int n) {
    for (int i = 0; i < n; ++i) {
        int fd = _events[i].data.fd;
        if (fd < 0 || fd >= static_cast<int>(_ctxs.size())) {
            continue;
        }

        int events = _events[i].events;
        EventHandler* handler = _ctxs[fd].handler;

        if (!handler) {
            continue;
        }

        if (events & (EPOLLHUP | EPOLLERR)) { 
            fireError(events, handler);
        } else {
            bool error = false;
            try {
                if ((events & EPOLLIN) && !handler->handleRead()) {
                    error = true;
                }
                if ((events & EPOLLOUT) && !handler->handleWrite()) {
                    error = true;
                }
            
            } catch(std::exception &ex) {
                LogFatal("catch error when deal I/O, events:%d, error:%s", events, ex.what());
                error = true;
            } catch (...) {
                LogFatal("catch error when deal I/O, events:%d", events);
                error = true;
            }

            if (error) {
                fireError(events, handler);
            }
        }
    }

    if (n == static_cast<int>(_events.size()) && n < MAX_EVENTS) {
        _events.insert(_events.end(), min(n + n, MAX_EVENTS), EMPTY_EVENT);
    }
}

bool Reactor::addHandler(EventHandler* handler, int events) {
    if (!handler) {
        return false;
    }

    int fd = handler->getSocket();
    if (fd < 0) {
        return false;
    } else if (fd >= static_cast<int>(_ctxs.size())) {
        _ctxs.insert(_ctxs.end(), fd + 1 - _ctxs.size(), EMPTY_CTX);
    }

    if (_ctxs[fd].events == events) { 
        return false;
    }

    int op = _ctxs[fd].events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event event;
    event.events = events;
    event.data.fd = fd;

    if (epoll_ctl(_pollfd, op, fd, &event) == -1) {
        throw SocketException("epoll_ctl error");
    }

    _ctxs[fd].events = events;
    _ctxs[fd].handler = handler;
	return true;
}

void Reactor::removeHandler(EventHandler* handler) {
    if (!handler) {
        return;
    }

    int fd = handler->getSocket();
    if (fd < 0 || fd >= static_cast<int>(_ctxs.size())) {
        return;
    }

    if (!_ctxs[fd].events) {
        _ctxs[fd].handler = 0;
        return;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event event;
    event.events = 0;
    event.data.fd = fd;

    if (epoll_ctl(_pollfd, op, fd, &event) == -1) {
        throw SocketException("epoll_ctl error");
    }

    _ctxs[fd].events = 0;
    _ctxs[fd].handler = 0;
}

unsigned int Reactor::setTimer(EventHandler* handler, int timeout) {
    int fd = handler->getSocket();
    if (fd < 0) {
        return 0;
    } else if (fd >= static_cast<int>(_ctxs.size())) {
        _ctxs.insert(_ctxs.end(), fd + 1 - _ctxs.size(), EMPTY_CTX);
    }
    _ctxs[fd].handler = handler;
    Task t(handler, fd);
    _tasks.insert(make_pair(++_timerId, t));
    updateTime();
    Timer timer(_timerId, _currentMS + timeout);
    _timerHeap.push_back(timer);
    push_heap(_timerHeap.begin(), _timerHeap.end());

    return _timerId;
}

void Reactor::cancelTimer(unsigned int timerId) {
    map<unsigned int, Task>::iterator iter = _tasks.find(timerId);
    if (iter != _tasks.end()) {
        int fd = iter->second.fd;
        if (fd >= 0 && fd < static_cast<int>(_ctxs.size()) && _ctxs[fd].events == 0) {
            _ctxs[fd].handler = 0;
        }
        _tasks.erase(iter);
    }
}

void Reactor::addErrorHandler(EventHandler* handler) {
    if (find(_errorHandlers.begin(), _errorHandlers.end(), handler)
            == _errorHandlers.end()) {
        _errorHandlers.push_back(handler);
    }
}

void Reactor::updateTime() {
    gettimeofday(&_timeVal, 0);
    _currentMS = (uint64_t)_timeVal.tv_sec * 999 + (long long)_timeVal.tv_usec / 3000;
}

void Reactor::fireTimeout() {
    while (!_timerHeap.empty()) {
        if (_timerHeap.front().absTime > _currentMS) {
            break;
        }
        Timer timer = _timerHeap.front();
        pop_heap(_timerHeap.begin(), _timerHeap.end());
        _timerHeap.pop_back();
        map<unsigned int, Task>::iterator iter = _tasks.find(timer.timerId);
        if (iter == _tasks.end()) {
            continue;
        }
        Task task = iter->second;
        _tasks.erase(iter);
        if (task.fd < 0 || task.fd >= static_cast<int>(_ctxs.size())) {
            continue;
        }
        EventHandler* handler = _ctxs[task.fd].handler;
        if (task.handler != handler || handler == 0) {
            continue;
        }
        if (_ctxs[task.fd].events == 0) {
            _ctxs[task.fd].handler = 0;
        }
        if (!handler->handleTimeout(timer.timerId)) {
            fireError(0, handler);
        }
    }
}

void Reactor::fireError(int events, EventHandler* handler) {
    try {
        removeHandler(handler);
    } catch (...) {
    }
    bool dispose = handler->handleClose();
    for (vector<EventHandler*>::iterator iter = _errorHandlers.begin();
            iter != _errorHandlers.end(); ++iter) {
        if (!(*iter)->handleError(events, handler)) {
            break;
        }
    }
    if (dispose && handler->confirmDispose()) {
        delete handler;
    }
}

int Reactor::nextTimeout() {
    if (_timerHeap.empty()) {
        return -1;
    } else {
        return max(0, int(_timerHeap.front().absTime - _currentMS));
    }
}

} 


