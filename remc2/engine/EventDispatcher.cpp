#include "EventDispatcher.h"

EventDispatcher::~EventDispatcher()
{
	for (auto el : _eventList)
	{
		for (auto e : el.second)
			delete e;
	}
}

void EventDispatcher::RegisterEvent(IEvent* event)
{
	if (event)
		_eventList[event->GetType()].push_back(event);
}

template <typename ..._args>
void EventDispatcher::DispatchEvent(EventType eventType, _args...a)
{
	auto it_eventList = _eventList.find(eventType);
	if (it_eventList == _eventList.end())
		return;

	for (auto ie : it_eventList->second)
	{
		if (Event<_args...>* event = dynamic_cast<Event<_args...>*>(ie))
			event->Trigger(a...);
	}
}