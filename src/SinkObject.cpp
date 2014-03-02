
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, cerr, ostream, etc

#include "SinkObject.h"

class Event;
class VTime;

using std::cerr;
using std::endl;

SinkObject::SinkObject(const VTime& starttime):startTime(starttime),sinkFinished(false) {
}

SinkObject::~SinkObject() {
}

bool
SinkObject::amIDone() {
    if (!sinkFinished) {
        return false;
    } else {
        return true;
    }
}
void
SinkObject::iAmFinished() {
    sinkFinished = true;
}

const int*
SinkObject::getFanins() {
    cerr << "Error: SinkObject::getFanins called\n";
    return NULL;
}

void
SinkObject::sendEvent(const Event* event) {
    cerr << "Error: SinkObject::sendEvent called\n";
    cerr << "Event is " << event << endl;
}





