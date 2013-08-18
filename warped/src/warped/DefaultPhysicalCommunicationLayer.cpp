
#include "DefaultPhysicalCommunicationLayer.h"
using std::cerr;
using std::endl;

DefaultPhysicalCommunicationLayer::DefaultPhysicalCommunicationLayer(){};

DefaultPhysicalCommunicationLayer::~DefaultPhysicalCommunicationLayer(){};

void
DefaultPhysicalCommunicationLayer::physicalInit( SimulationConfiguration &configuration ){
   // there is nothing to be done for a
   // DefaultPhysicalCommunicationLayer since all objects are on one
   // simulation manager and so no network communication will take
   // place.
}

int
DefaultPhysicalCommunicationLayer::physicalGetId() const {
   // for a DefaultPhysicalCommunicationLayer, the id is always zero
   // since we are running a uniprocessor simulation.
   return 0;
}

void
DefaultPhysicalCommunicationLayer::physicalSend( const SerializedInstance *toSend,
						 unsigned int dest){
   cerr << "DefaultPhysicalCommunicationLayer::physicalSend "
        << "should not be called at all" << endl;
   cerr << "Exiting simulation ..." << endl;
   abort();
}

SerializedInstance *
DefaultPhysicalCommunicationLayer::physicalProbeRecv(){
   cerr << "DefaultPhysicalCommunicationLayer::physicalProbeRecv "
        << "should not be called at all" << endl;
   cerr << "Exiting simulation ..." << endl;
   abort();
   return NULL;
}

// bool
// DefaultPhysicalCommunicationLayer::physicalProbeRecvBuffer(char *buffer,
//                                                            int size,
//                                                            bool& sizeStatus){
//    cerr << "DefaultPhysicalCommunicationLayer::physicalProbeRecvBuffer "
//         << "should not be called at all" << endl;
//    cerr << "Exiting simulation ..." << endl;
//    std::abort();
//    return false;
// }

void
DefaultPhysicalCommunicationLayer::physicalFinalize(){
   // for DefaultPhysicalCommunicationLayer, we didn't initialize MPI
   // or any other communication library; so there is nothing to be
   // done for finalize.
}

int
DefaultPhysicalCommunicationLayer::physicalGetSize() const {
   // for a DefaultPhysicalCommunicationLayer, the size is always 1
   // since we are running a uniprocessor simulation.
   return 1;
}
