#pragma once
#ifndef IEVENT_H
#define IEVENT_H

#include <string>
#include "EventType.h"

class IEvent
{
public:
	virtual const EventType GetType() const = 0;
};

#endif //IEVENT_H