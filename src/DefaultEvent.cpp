
#include "DefaultEvent.h"

DefaultEvent::~DefaultEvent() {
    delete sendTime;
    delete receiveTime;
    delete sender;
    delete receiver;
}

