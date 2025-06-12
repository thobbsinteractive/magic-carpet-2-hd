#pragma once

#include <string>
#include "EventType.h"

class IEvent
{
public:
	virtual const EventType GetType() const = 0;
};