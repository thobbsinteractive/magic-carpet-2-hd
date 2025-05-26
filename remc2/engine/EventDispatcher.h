#pragma once

#include <map>
#include <vector>
#include "Event.h"

class EventDispatcher
{
public:
	static EventDispatcher* I;

	~EventDispatcher();
	void RegisterEvent(IEvent* event);
	template <typename ..._args>
	void DispatchEvent(EventType eventType, _args...a);

private:
	std::map<EventType, std::vector<IEvent*>> _eventList;
};