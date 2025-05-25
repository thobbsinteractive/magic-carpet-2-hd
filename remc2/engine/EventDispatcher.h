#pragma once
#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <string>
#include <map>
#include <vector>
#include "Event.h"

class EventDispatcher
{
public:
	~EventDispatcher();
	void RegisterEvent(IEvent* event);
	template <typename ..._args>
	void DispatchEvent(EventType eventType, _args...a);

private:
	std::map<EventType, std::vector<IEvent*>> _eventList;
};

#endif //EVENT_DISPATCHER_H