#include "../include/TwoInputAndGateApplication.h"
#include <warped/WarpedMain.h>

int
main(int argc, char** argv) {
    std::string configuration = "";
    std::string simulateUntil = "";
    bool debug = false;

    try {
        TCLAP::CmdLine cmd("Two Input And Gate Simulation");

        // Arguments for WarpedMain
        TCLAP::ValueArg<std::string> configurationArg("c", "configuration", "WARPED configuration file",
                                                      false, configuration, "file", cmd);
        TCLAP::ValueArg<std::string> simulateUntilArg("u", "simulateUntil", "specify a simulation end time",
                                                      false, simulateUntil, "time", cmd);
        TCLAP::SwitchArg debugArg("d", "debug", "display debug messages", cmd, debug);

        cmd.parse(argc, argv);

        configuration = configurationArg.getValue();
        simulateUntil = simulateUntilArg.getValue();
        debug = debugArg.getValue();

    } catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }

    WarpedMain wm(new TwoInputAndGateApplication(),
                  configuration, simulateUntil, debug);

    return wm.main(argc, argv);
}

