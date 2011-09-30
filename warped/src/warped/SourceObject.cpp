// See copyright notice in file Copyright in the root directory of this archive.

#include "SourceObject.h"
using std::cerr;
using std::endl;

SourceObject::SourceObject(const VTime& starttime):startTime(starttime),sourceFinished(false){}

SourceObject::~SourceObject(){}

bool
SourceObject::amIDone(){
  if(!sourceFinished){
    return false;
  }
  else {
    return true;
  }
}
void
SourceObject::iAmFinished(){
  sourceFinished = true;
}

const int* 
SourceObject::getFanouts(){
  cerr << "Error: SourceObject::getFanouts called\n";
  return NULL;
}

void
SourceObject::sendEvent(const Event* event){
  cerr << "Error: SourceObject::sendEvent called\n";
  cerr << "Event is " << event << endl;
}

const Event* 
SourceObject::getEvent(){
  cerr << "Error: SourceObject::getEvent called\n";
  return NULL;
}

bool
SourceObject::haveMoreEvents(){
  cerr << "Error: SourceObject::haveMoreEvents called\n";
  return false;
}


