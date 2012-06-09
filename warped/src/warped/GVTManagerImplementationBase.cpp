// See copyright notice in file Copyright in the root directory of this archive.

#include "GVTManagerImplementationBase.h"
#include "TimeWarpSimulationManager.h"

using std::cerr;
using std::endl;

GVTManagerImplementationBase::GVTManagerImplementationBase( TimeWarpSimulationManager
							    *simMgr,
							    unsigned int
							    period) :
  gVTPeriodCounter(0),
  gVTPeriod( period ),
  mySimulationManager( simMgr ),
  gVT( simMgr->getZero().clone() ){}

GVTManagerImplementationBase::~GVTManagerImplementationBase(){
  delete gVT;
}

bool
GVTManagerImplementationBase::checkGVTPeriod(){
	cerr << "GVTManagerImplementationBase::checkGVTPeriod() called: " << endl;
	abort();
}

void
GVTManagerImplementationBase::updateEventRecord(const char *infoStream,
                                                unsigned int srcSimMgr){
  cerr << "GVTManagerImplementationBase::updateEventRecord() called: "
       << "(" << srcSimMgr << "), " << infoStream << endl;
  abort();
}

const VTime &GVTManagerImplementationBase::getGVT(){
	ASSERT( gVT != 0 );
	return *gVT;
}

void GVTManagerImplementationBase::setGVT( const VTime &newGVT ){
  //It is possible some thread is using gVT so it cant be deleted here
  //for the threadedTimeWarp Version
  //delete gVT;
  gVT = newGVT.clone();
}

void
GVTManagerImplementationBase::calculateGVTInfo(const VTime &receiveTime){
  cerr << "GVTManagerImplementationBase::calculateGVTInfo(ObjectID &objectID) called !!"
       << endl;
  abort();
}

const string
GVTManagerImplementationBase::getGVTInfo(unsigned int srcSimMgr,
                                         unsigned int destSimMgr,
					 const VTime &sendTime){
  cerr << "GVTManagerImplementationBase::getGVTInfo() called: ("
       << srcSimMgr << "," << destSimMgr << ")" << endl;
  abort();
  return NULL;
}


void
GVTManagerImplementationBase::calculateGVT(){
  cerr << "GVTManagerImplementationBase::calculateGVT() called !!"
       << endl;
  abort();
}

void
GVTManagerImplementationBase::sendGVTUpdate(){
  cerr << "GVTManagerImplementationBase::sendGVTUpdate() called !!"
       << endl;
  abort();
}
bool
GVTManagerImplementationBase::getGVTTokenStatus(){
  cerr << "GVTManagerImplementationBase::getGVTTokenStatus() called !!"
       << endl;
  abort();
}

void
GVTManagerImplementationBase::configure( SimulationConfiguration &configuration ){
  registerWithCommunicationManager();
}
