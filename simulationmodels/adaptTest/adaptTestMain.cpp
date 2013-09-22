#include "AdaptTestApplication.h"
#include "warped/WarpedMain.h"

#include "tclap/CmdLine.h"

#include <string>
#include <iostream>


int
main(int argc, char** argv) {
    int numStragglers = 6;
    int numObjects = 6;
    std::string outputMode = "lazy";
    int adaptiveState = false;

    std::string configuration = "";
    std::string simulateUntil = "";
    bool debug = false;

    try {
        TCLAP::CmdLine cmd("Adapt Test Simulation");

        TCLAP::ValueArg<int> numObjectsArg("", "numObjects", "number of simulation objects",
                                           false, numObjects, "int", cmd);
        TCLAP::ValueArg<int> numStragglersArg("", "numStragglers", "number of stragglers",
                                              false, numStragglers, "int", cmd);
        TCLAP::SwitchArg adaptiveStateArg("", "adaptiveState",
                                          "should the object execution and state saving time vary",
                                          cmd, false);

        std::vector<string> allowed {"lazy", "aggr", "adapt"};
        TCLAP::ValuesConstraint<string> constraint(allowed);
        TCLAP::ValueArg<string> outputModeArg("", "outputMode", "event output mode",
                                              false, outputMode, &constraint, cmd);

        // Arguments for WarpedMain
        TCLAP::ValueArg<std::string> configurationArg("c", "configuration", "WARPED configuration file",
                                                      false, configuration, "file", cmd);
        TCLAP::ValueArg<std::string> simulateUntilArg("u", "simulateUntil", "specify a simulation end time",
                                                      false, simulateUntil, "time", cmd);
        TCLAP::SwitchArg debugArg("d", "debug", "display debug messages", cmd, debug);

        cmd.parse(argc, argv);

        numObjects = numObjectsArg.getValue();
        numStragglers = numStragglersArg.getValue();
        adaptiveState = adaptiveStateArg.getValue();
        outputMode = outputModeArg.getValue();

        configuration = configurationArg.getValue();
        simulateUntil = simulateUntilArg.getValue();
        debug = debugArg.getValue();

    } catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }

    WarpedMain wm(new AdaptTestApplication(numStragglers, numObjects, outputMode, adaptiveState),
                  configuration, simulateUntil, debug);

    // Defaults - 6 objects, 6 stragglers, regenerate same events, no delays.
    return wm.main(argc, argv);
}

