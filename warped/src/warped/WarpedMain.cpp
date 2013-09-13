
#include "warped.h"
#include "Application.h"
#include "WarpedMain.h"
#include "SimulationConfiguration.h"
#include "Simulation.h"
#include "Spinner.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>
#include <exception>

// We need these to register their deserializers
#include "CirculateInitializationMessage.h"
#include "DeserializerManager.h"
#include "EventMessage.h"
#include "GVTUpdateMessage.h"
#include "InitializationMessage.h"
#include "UsefulWorkMessage.h"
#include "IntVTime.h"
#include "MatternGVTMessage.h"
#include "NegativeEventMessage.h"
#include "NegativeEvent.h"
#include "RestoreCkptMessage.h"
#include "StartMessage.h"
#include "TerminateToken.h"
#include "WarpedMain.h"

WarpedMain::WarpedMain(Application* application, std::string configurationFileName,
                       std::string simulateUntil,  bool debug) :
    errors(0),
    warnings(0),
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

vector<string>
WarpedMain::buildArgumentVector(int argc, char** argv) {
    vector<string> retval;
    for (int i = 0; i < argc; i++) {
        retval.push_back(string(argv[i]));
    }
    return retval;
}

SimulationConfiguration*
WarpedMain::readConfiguration(const std::string& configurationFileName,
                              const vector<string>& argumentVector) {
    SimulationConfiguration* configuration = 0;
    if (!configurationFileName.empty()) {
        configuration = SimulationConfiguration::parseConfiguration(configurationFileName,
                                                                    argumentVector);
        if (configuration == 0) {
            std::cerr << "There was a problem parsing configuration " << configurationFileName
                      << ", exiting." << std::endl;
            exit(-1);
        } else {
            std::cerr << "Using configuration file: " << configurationFileName << std::endl;
        }
    }
    return configuration;
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
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::exit(std::EXIT_FAILURE);
    }


    if (configuration.get_bool({"SpinBeforeConfiguration"}, false)) {
        Spinner::spinForever();
    }

    mySimulation = Simulation::instance(&configuration, myApplication);
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
    vector<string> args = buildArgumentVector(argc, argv);
    initializeSimulation(args);

    if (simulateUntil == "") {
        simulate(myApplication->getPositiveInfinity());
    } else {
        simulate(myApplication->getTime(simulateUntil));
    }

    finalize();

    return errors;
}
