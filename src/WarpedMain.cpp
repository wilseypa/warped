
#include <cstdlib>                      // for exit, EXIT_FAILURE
#include <exception>                    // for exception
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for operator==, string, etc

#include "Application.h"                // for Application
// We need these to register their deserializers
#include "CirculateInitializationMessage.h"
#include "EventMessage.h"               // for EventMessage
#include "GVTUpdateMessage.h"           // for GVTUpdateMessage
#include "InitializationMessage.h"      // for InitializationMessage
#include "IntVTime.h"                   // for IntVTime
#include "MatternGVTMessage.h"          // for MatternGVTMessage
#include "NegativeEvent.h"              // for NegativeEvent
#include "NegativeEventMessage.h"       // for NegativeEventMessage
#include "RestoreCkptMessage.h"         // for RestoreCkptMessage
#include "Simulation.h"                 // for Simulation
#include "SimulationConfiguration.h"    // for SimulationConfiguration
#include "Spinner.h"                    // for spinForever
#include "StartMessage.h"               // for StartMessage
#include "TerminateToken.h"             // for TerminateToken
#include "UsefulWorkMessage.h"          // for UsefulWorkMessage
#include "WarpedDebug.h"                // for enableDebug, debugout
#include "WarpedMain.h"

WarpedMain::WarpedMain(Application* application, std::string configurationFileName,
                       std::string simulateUntil,  bool debug) :
    configurationFileName(configurationFileName),
    debugFlag(debug),
    simulateUntil(simulateUntil),
    myApplication(application),
    mySimulation(0) {}

WarpedMain::~WarpedMain() {
    delete mySimulation;
    delete myApplication;
}

void
WarpedMain::registerKernelDeserializers() {
    CirculateInitializationMessage::registerDeserializer();
    EventMessage::registerDeserializer();
    GVTUpdateMessage::registerDeserializer();
    InitializationMessage::registerDeserializer();
    UsefulWorkMessage::registerDeserializer();
    IntVTime::registerDeserializer();
    MatternGVTMessage::registerDeserializer();
    NegativeEvent::registerDeserializer();
    NegativeEventMessage::registerDeserializer();
    RestoreCkptMessage::registerDeserializer();
    StartMessage::registerDeserializer();
    TerminateToken::registerDeserializer();
}

void
WarpedMain::initializeSimulation() {
    registerKernelDeserializers();
    myApplication->registerDeserializers();

    if (debugFlag == true) {
        debug::enableDebug();
        debug::debugout << "Debug output enabled." << std::endl;
    }

    try {
        SimulationConfiguration configuration(configurationFileName);

        if (configuration.get_bool({"SpinBeforeConfiguration"}, false)) {
            Spinner::spinForever();
        }

        mySimulation = Simulation::instance(&configuration, myApplication);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE); // EXIT_FAILURE is a macro, so no namespace
    }

    mySimulation->initialize();
}

void
WarpedMain::simulate(const VTime& simulateUntil) {
    mySimulation->simulate(simulateUntil);
}

bool
WarpedMain::simulationComplete() {
    return mySimulation->simulationComplete();
}

void
WarpedMain::finalize() {
    mySimulation->finalize();
}

const VTime&
WarpedMain::getCommittedTime() {
    return mySimulation->getCommittedTime();
}

const VTime&
WarpedMain::getNextEventTime() {
    return mySimulation->getNextEventTime();
}

int
WarpedMain::main(int argc, char** argv) {
    //bind manager thread to core 0
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }

    initializeSimulation();

    if (simulateUntil == "") {
        simulate(myApplication->getPositiveInfinity());
    } else {
        simulate(myApplication->getTime(simulateUntil));
    }

    finalize();

    return 0;
}
