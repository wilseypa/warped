
#include "FlatState.h"
#include <string.h>

FlatState::FlatState(){}

FlatState::~FlatState(){}

void
FlatState::copyState(const State* state) {
  memcpy(this, state, ((FlatState *)state)->getSize());
}
