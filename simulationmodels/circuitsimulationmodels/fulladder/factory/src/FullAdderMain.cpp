#include "../include/FullAdderApplication.h"
#include "warped/WarpedMain.h"

#include "tclap/CmdLine.h"

#include <string>
#include <iostream>

int
main(int argc, char** argv) {
    int numObjects = 0;
    std::string inputFileName = "";

    std::string configuration = "";
    std::string simulateUntil = "";
    bool debug = false;

    try {
        TCLAP::CmdLine cmd("FullAdder Simulation");

        TCLAP::ValueArg<int> numObjectsArg("", "numObjects",
                                           "number of simulation objects",
                                           false, numObjects, "int", cmd);
        TCLAP::ValueArg<string> inputFileNameArg("", "simulate", "configuration file name",
                                                 true, inputFileName, "string", cmd);


        // Arguments for WarpedMain
        TCLAP::ValueArg<std::string> configurationArg("c", "configuration", "WARPED configuration file",
                                                      false, configuration, "file", cmd);
        TCLAP::ValueArg<std::string> simulateUntilArg("u", "simulateUntil", "specify a simulation end time",
                                                      false, simulateUntil, "time", cmd);
        TCLAP::SwitchArg debugArg("d", "debug", "display debug messages", cmd, debug);

        cmd.parse(argc, argv);

        inputFileName = inputFileNameArg.getValue();
        numObjects = numObjectsArg.getValue();

        configuration = configurationArg.getValue();
        simulateUntil = simulateUntilArg.getValue();
        debug = debugArg.getValue();

    } catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }

    if (inputFileName.empty()) {
        std::cerr << "A configuration file must be specified using --simulate" << std::endl;
        abort();
    }

    WarpedMain wm(new FullAdderApplication(numObjects, inputFileName),
                  configuration, simulateUntil, debug);

    return wm.main(argc, argv);
}

