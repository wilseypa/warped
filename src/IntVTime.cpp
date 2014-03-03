
#include <iostream>                     // for ostringstream, etc
#include <string>                       // for string

#include "DeserializerManager.h"        // for DeserializerManager, string
#include "IntVTime.h"
#include "SerializedInstance.h"         // for SerializedInstance
#include "warped.h"                     // for ostringstream, etc

class Serializable;
class VTime;

using std::ostringstream;
using std::cerr;
using std::endl;


const VTime&
IntVTime::getIntVTimeZero() {
    static const IntVTime ZERO = IntVTime(0);
    return ZERO;
}

const VTime&
IntVTime::getIntVTimePositiveInfinity() {
    static const IntVTime PINFINITY = IntVTime(getWarped32Max());
    return PINFINITY;
}

void
IntVTime::serialize(SerializedInstance* serialized) const {
    serialized->addInt(getTime());
}

Serializable*
IntVTime::deserialize(SerializedInstance* serialized) {
    warped64_t time = serialized->getInt();
    IntVTime* retval = new IntVTime(time);
    return retval;
}

// We might be able to move this to VTime
void
IntVTime::registerDeserializer() {
    ASSERT(DeserializerManager::instance() != 0);
    DeserializerManager::instance()->registerDeserializer(getIntVTimeDataType(),
                                                          &deserialize);
}

const VTime&
IntVTime::operator= (const VTime& rhs) {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    myTime = intVrhs.getTime();

    return *this;
}

bool
IntVTime::operator< (const VTime& rhs) const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime < intVrhs.getTime();
}

bool
IntVTime::operator> (const VTime& rhs) const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime > intVrhs.getTime();
}

bool
IntVTime::operator==(const VTime& rhs)const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime == intVrhs.getTime();
}

bool
IntVTime::operator!= (const VTime& rhs) const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime != intVrhs.getTime();
}

bool
IntVTime::operator<= (const VTime& rhs) const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime <= intVrhs.getTime();
}

bool
IntVTime::operator>= (const VTime& rhs) const {
    const IntVTime& intVrhs = static_cast<const IntVTime&>(rhs);
    return myTime >= intVrhs.getTime();
}


const IntVTime&
IntVTime::operator=(const IntVTime& rhs) {
    myTime = rhs.getTime();
    return *this;
}

bool
IntVTime::operator< (const IntVTime& right) const {
    return myTime < right.getTime();
}


bool
IntVTime::operator> (const IntVTime& right) const {
    return myTime > right.getTime();
}

/// Overloaded operator
bool
IntVTime::operator== (const IntVTime& right) const {
    return myTime == right.getTime();
}

/// Overloaded operator
bool
IntVTime::operator!= (const IntVTime& right) const {
    return myTime != right.getTime();
}

/// Overloaded operator
bool
IntVTime::operator<= (const IntVTime& right) const {
    return myTime <= right.getTime();
}

/// Overloaded operator
bool
IntVTime::operator>= (const IntVTime& right) const {
    return myTime >= right.getTime();
}


const IntVTime
IntVTime::operator+ (const IntVTime& right) const {
    return IntVTime(myTime + right.getTime());
}

const IntVTime
IntVTime::operator- (const IntVTime& right) const {
    return IntVTime(myTime + right.getTime());
}

const IntVTime
IntVTime::operator+ (int right) const {
    return IntVTime(myTime + right);
}

const IntVTime
IntVTime::operator- (int right) const {
    return IntVTime(myTime - right);
}

const string
IntVTime::toString() const {
    ostringstream stream;
    stream << myTime;
    return stream.str();
}
