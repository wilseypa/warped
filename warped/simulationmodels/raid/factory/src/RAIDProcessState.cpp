//-*-c++-*-
#include "../include/RAIDProcessState.h"
using namespace std;

RAIDProcessState::RAIDProcessState() {
  gen = NULL;
  diskOperationPending = false;
  stopProcessing       = false;
  size                 = 0;
  paritySize           = -1;
  numRequests          = 0;
  numReads             = 0;
  numWrites            = 0;
  read                 = false;
  beginningOfStrype    = -1;
  strypeSize           = -1;
  numDisks             = 0;
  diskRequests         = NULL;
  firstDiskId          = 0;
}

RAIDProcessState::~RAIDProcessState() {
  delete gen;
  delete [] diskRequests;
}

RAIDProcessState::RAIDProcessState(const RAIDProcessState &thisState) {
  delete this->gen;
  this->gen = new MLCG(*(thisState.gen));

  this->numRequests = thisState.numRequests;
  this->diskOperationPending = thisState.diskOperationPending;
  this->read = thisState.read;
  this->stopProcessing = thisState.stopProcessing;
  this->size = thisState.size;
  this->paritySize = thisState.paritySize;
  this->numReads = thisState.numReads;
  this->numWrites = thisState.numWrites;
  this->beginningOfStrype = thisState.beginningOfStrype;
  this->strypeSize = thisState.strypeSize;
  this->numDisks = thisState.numDisks;
  if (this->diskRequests != NULL) {
    delete [] this->diskRequests;
  }
  this->diskRequests = new int[thisState.numDisks];
  int i;
  for (i = 0; i < thisState.numDisks; i++) {
    this->diskRequests[i] = thisState.diskRequests[i];
  }
  this->firstDiskId = thisState.firstDiskId;
} // End of Copy Constructor.

RAIDProcessState&
RAIDProcessState::operator=(RAIDProcessState& thisState){
  (State &)*this = (State &) thisState;

  delete this->gen;
  this->gen = new MLCG(*(thisState.gen));

  this->numRequests = thisState.numRequests;
  this->diskOperationPending = thisState.diskOperationPending;
  this->read = thisState.read;
  this->stopProcessing = thisState.stopProcessing;
  this->size = thisState.size;
  this->paritySize = thisState.paritySize;
  this->numReads = thisState.numReads;
  this->numWrites = thisState.numWrites;
  this->beginningOfStrype = thisState.beginningOfStrype;
  this->strypeSize = thisState.strypeSize;
  this->numDisks = thisState.numDisks;
  if (this->diskRequests != NULL) {
    delete [] this->diskRequests;
  }
  this->diskRequests = new int[thisState.numDisks];
  int i;
  for (i = 0; i < thisState.numDisks; i++) {
    this->diskRequests[i] = thisState.diskRequests[i];
  }
  this->firstDiskId = thisState.firstDiskId;
  return *this;
}

void
RAIDProcessState::copyState(const State* state)
{
  if (this != (RAIDProcessState *) state) {
    *this = *((RAIDProcessState *) state);
  }
}

unsigned int
RAIDProcessState::getStateSize() const {
  return sizeof(RAIDProcessState);
}

const SerializedInstance*
RAIDProcessState::serialize() {
  cerr << "Error: RAIDProcessState::serialize called\n";
  return NULL;
} // End of serialize()

State*
RAIDProcessState::deserialize(const SerializedInstance instance) {
  cerr << "Error: RAIDProcessState::deserialize called\n";
  return NULL;
} // End of deserialize()

ostream& 
operator<< (ostream& os, const RAIDProcessState& rps) {
  os << "NrReqs:" << rps.numRequests << " " 
     << "DiskOpPending:" << rps.diskOperationPending << " "
     << "IsRead:" << rps.read << " " 
     << "Size:" << rps.size << " "
     << "ParitySize:" << rps.paritySize << " "
     << "Reads:" << rps.numReads << " " 
     << "Writes:" << rps.numWrites << " "
     << "CurrDiskReqs:" << " [" ;
  for (int ii = 0; ii < rps.numDisks; ii++) {
    os << rps.diskRequests[ii] << " ";
  }
  os << "] " << endl;
  return os;
}

ostream& RAIDProcessState::printState(ostream& os) {
  os << "NrReqs:" << this->numRequests << " " 
     << "DiskOpPending:" << this->diskOperationPending << " "
     << "IsRead:" << this->read << " " 
     << "Size:" << this->size << " " 
     << "ParitySize:" << this->paritySize << " "
     << "Reads:" << this->numReads << " " 
     << "Writes:" << this->numWrites << " "
     << "CurrDiskReqs:" << " [" ;
  for (int ii = 0; ii < this->numDisks; ii++) {
    os << this->diskRequests[ii] << " ";
  }
  os << "] ";
  return os;
}

