#include "door.h"

extern uint8_t door_state;

void closeDoor()
{
    if(door_state == CLOSED)
        return;
}

void openDoor()
{
}

int isDoorClosed()
{
    return door_state;
}

