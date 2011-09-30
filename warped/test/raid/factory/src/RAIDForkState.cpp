//-*-c++-*-
#include "../include/RAIDForkState.h"
using namespace std;

RAIDForkState::RAIDForkState()  {
  lastOutput = 0;
} 

void
RAIDForkState::copyState(const State* state) {
  this->lastOutput = ((RAIDForkState *) state)->lastOutput;
}

unsigned int
RAIDForkState::getStateSize() const {
  return sizeof(RAIDForkState);
}

const SerializedInstance*
RAIDForkState::serialize() {
  cerr << "Error: RAIDForkState::serialize called\n";
  return NULL;
} // End of serialize()

State*
RAIDForkState::deserialize(const SerializedInstance instance) {
  cerr << "Error: RAIDForkState::deserialize called\n";
  return NULL;
} // End of deserialize()

ostream& RAIDForkState::printState(ostream& os) {
  os << "Fork::lastOutput: " << this->lastOutput; 
  return os;
}
