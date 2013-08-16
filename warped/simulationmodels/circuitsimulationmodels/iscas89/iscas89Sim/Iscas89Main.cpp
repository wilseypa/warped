#include "Iscas89Application.h"
#include "warped/WarpedMain.h"

#include "tclap/CmdLine.h"

#include <string>
#include <iostream>

int 
main(int argc, char **argv){
    std::string inputFileName = "";
    std::string testCaseFileName = "";
    int numObjects = 0;

    std::string configuration = "";
    std::string simulateUntil = "";
    bool debug = false;
    try {
        TCLAP::CmdLine cmd("Iscas89 Simulation");

        TCLAP::ValueArg<string> inputFileNameArg("", "simulate", "configuration file name",
                true, inputFileName, "file", cmd);
        TCLAP::ValueArg<string> testCaseFileNameArg("", "simulate", "test case file name",
                true, testCaseFileName, "file", cmd);
        TCLAP::ValueArg<int> numObjectsArg("", "numObjects",
                                           "number of simulation objects",
                                           false, numObjects, "int", cmd);

        // Arguments for WarpedMain
        TCLAP::ValueArg<std::string> configurationArg("c", "configuration", "WARPED configuration file",
                                                      false, configuration, "file", cmd);
        TCLAP::ValueArg<std::string> simulateUntilArg("u", "simulateUntil", "specify a simulation end time",
                                                      false, simulateUntil, "time", cmd);
        TCLAP::SwitchArg debugArg("d", "debug", "display debug messages", cmd, debug);

        cmd.parse(argc, argv);

        inputFileName = inputFileNameArg.getValue();
        testCaseFileName = testCaseFileNameArg.getValue();
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

    // Note that this simulation treats the --simulate argument differently than
    // other simulations.
    string path = "circuitsimulationmodels/iscas89/iscas89Sim/";
    inputFileName = path + inputFileName + "/" + inputFileName + "config";

    WarpedMain wm(new Iscas89Application(inputFileName, testCaseFileName, numObjects),
                  configuration, simulateUntil, debug);

    return wm.main(argc, argv);
}

