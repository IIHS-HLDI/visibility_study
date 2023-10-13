#pragma once

#include <stack>

typedef enum {
	PAN = 0,
	ZOOM,
	MOVE_GRID_CORNER_BEGIN,
	MOVE_GRID_CORNER_END,
	LOAD_IMAGE,
	LOAD_SESSION,
	PAINT,
	ERASE,
	PLACE_REF_POINT,
	DELETE_REF_POINT,

} EventType;

typedef struct Event {
	int id;
	EventType event_type;
};

class EventManager
{
	std::stack<Event> event_stack;

public:
	void push(Event event);
};

