//-*-c++-*-

#include "../include/RAIDDiskState.h"
#include <iostream>
using namespace std;

RAIDDiskState::RAIDDiskState() {
  currentCylinder = 0;
}

RAIDDiskState::~RAIDDiskState() {
}

RAIDDiskState&
RAIDDiskState::operator=(RAIDDiskState& thisState){
  (State &)*this = (State &)thisState;
  this->currentCylinder = thisState.currentCylinder;
  return *this;
}

void
RAIDDiskState::copyState(const State* state) {
  ASSERT( state != 0 );
  const RAIDDiskState *diskState = dynamic_cast<const RAIDDiskState *>(state);
  currentCylinder = diskState->getCurrentCylinder();
}

unsigned int
RAIDDiskState::getStateSize() const {
  return sizeof(RAIDDiskState);
}

const SerializedInstance*
RAIDDiskState::serialize() {
  cerr << "Error: RAIDDiskState::serialize called\n";
  return NULL;
} // End of serialize()

State*
RAIDDiskState::deserialize(const SerializedInstance instance) {
  cerr << "Error: RAIDDiskState::deserialize called\n";
  return NULL;
} // End of deserialize()

int
RAIDDiskState::getCurrentCylinder() const {
  return currentCylinder;
} // End of getCurrentCylinder()

void
RAIDDiskState::setCurrentCylinder(int cylinder) {
  currentCylinder = cylinder;
} // End of setCurrentCylinder()

std::ostream& RAIDDiskState::printState(std::ostream& os) {
  os << "Disk::currentCylinder: " << this->currentCylinder; 
  return os;
}
