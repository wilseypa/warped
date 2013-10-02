
#include "VTime.h"

VTime::~VTime() {}

const VTime&
VTime::operator= (const VTime& from) {
    std::cerr << "VTime::operator= called, aborting" << std::endl;
    abort();
}
