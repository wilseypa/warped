// See copyright notice in file Copyright in the root directory of this archive.

#include "../include/ProcessState.h"
#include <string.h>

ProcessState::ProcessState() {
  stateBulk = NULL;
  gen = NULL;
  numSent = 0;
  numReceived = 0;
}

ProcessState::ProcessState(int stateSize):gen(NULL),sizeOfStateData(stateSize),numSent(0),numReceived(0){
  stateBulk = new char[stateSize];
}

ProcessState::~ProcessState() {
  delete [] stateBulk;
  delete gen;
}

ProcessState::ProcessState( const ProcessState& state) {
  copyState(&state);
}

void
ProcessState::copyState(const State* toCopy){
  ASSERT( toCopy != 0);
  const ProcessState *thisState = dynamic_cast< const ProcessState* >(toCopy);

  if(sizeOfStateData != thisState->sizeOfStateData){
    sizeOfStateData = thisState->sizeOfStateData;
    delete [] stateBulk;
    stateBulk = new char[sizeOfStateData];
  }

  delete gen;
  gen = new MLCG(*(thisState->gen));

  numSent = thisState->numSent;
  numReceived = thisState->numReceived;
}

ostream&
operator<< (ostream& os, const ProcessState& rps) {
  
  os << " sizeOfStateData: " << rps.sizeOfStateData << "\n"
     << rps.gen->seed1() << ' ' << rps.gen->seed2() << "\n";
  return( os );
}
