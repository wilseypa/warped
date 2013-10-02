
#include "ControlManager.h"
#include "TimeWarpSimulationManager.h"
#include "SimulationConfiguration.h"
#include "CommunicationManager.h"
#include <fstream>
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

ControlManager::ControlManager(TimeWarpSimulationManager* simMgr):mySimulationManager(simMgr) {}

ControlManager::~ControlManager() {
//   const vector<Sensor *> *sensors = sensorDataBase.toVector();

//   const vector<Sensor *>::const_iterator iter_begin = sensors->begin();
//   const vector<Sensor *>::const_iterator iter_end   = sensors->end();

//   *infoStream << "------------------" << endl;
//   *infoStream << "Sensor-Value Pairs" << endl;
//   *infoStream << "------------------" << endl;

//   vector<Sensor *>::iterator current = iter_begin;
//   while( current != iter_end ){
//     (*current)->dump(*infoStream);
//     ++current;
//   }
}

void
ControlManager::configure(SimulationConfiguration&) {
    abort();
}

void
ControlManager::registerSensor(string& name, Sensor* sensor) {
    if ((*(sensorDataBase.find(name))).second != sensor) {
        sensorDataBase.insert(std::make_pair(name, sensor));
    } else {
        cerr << "Sensor " << name << " already exists !!" << endl;
        cerr << "ControlManager Error: aborting simulation" << endl;
        abort();
    }
}

void
ControlManager::registerActuator(string& name, Actuator* actuator) {
    if ((*(actuatorDataBase.find(name))).second != actuator) {
        actuatorDataBase.insert(std::make_pair(name, actuator));
    } else {
        cerr << "Actuator " << name << " already exists !!" << endl;
        cerr << "ControlManager Error: aborting simulation" << endl;
        abort();
    }
}

// the ControlManager must register the following message types with
// the communication manager: ControlStatusMessage, InitiateAdjustmentMessage
void
ControlManager::registerWithCommunicationManager() {
    const int numberOfMessageTypes = 2;
    string messageType[numberOfMessageTypes] = { "ControlStatusMessage",
                                                 "InitiateAdjustmentMessage"
                                               };
    ASSERT(mySimulationManager != NULL);
    for (int count = 0; count < numberOfMessageTypes; count++) {
        mySimulationManager->getCommunicationManager()->registerMessageType(messageType[count], this);
    }
}

// the communication manager will call this method to deliver a
// ControlStatusMessage or a InitiateAdjustmentMessage.
void
ControlManager::receiveKernelMessage(KernelMessage*) {}

void
ControlManager::initiateParameterAdjustment() {}

void
ControlManager::initiateControlAction() {}

