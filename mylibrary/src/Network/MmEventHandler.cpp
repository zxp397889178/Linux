/***********************************************************************
 *
 * File:        EventHandler.cpp
 * Author:      iotazhang
 * Revision:    $Id: MmEventHandler.cpp 364262 2012-11-07 03:39:36Z iotazhang $
 * Description: 事件处理器
 *
 ***********************************************************************/

#include "MmEventHandler.h"
#include "MmReactor.h"

namespace mm {

EventHandler::EventHandler() : _reactor(0), _events(0) {}

EventHandler::~EventHandler() {}

Reactor* EventHandler::getReactor() const {
    return _reactor;
}

void EventHandler::setReactor(Reactor* reactor) {
    _reactor = reactor;
}

/*
bool EventHandler::handleRead() {
    return true;
}

bool EventHandler::handleWrite() {
    return true;
}

bool EventHandler::handleTimeout(unsigned int) {
    return true;
}
*/

bool EventHandler::confirmDispose() {
	return true;
}

bool EventHandler::handleError(int, EventHandler*) {
    return true;
}

int EventHandler::getEvents() const {
    return _events;
}

void EventHandler::setEvents(int events) {
    if (_events != events) {        
        if (events == 0) {
			_events = 0;
            _reactor->removeHandler(this);
        } else {
            if(_reactor->addHandler(this, events))
        	{
        		_events = events;
        	}
        }
    }
}

unsigned int EventHandler::setTimer(int timeout) {
    return _reactor->setTimer(this, timeout);
}

void EventHandler::cancelTimer(unsigned int timerId) {
    _reactor->cancelTimer(timerId);
}

}


