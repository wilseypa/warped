#include "PingPongApplication.h"
#include "WarpedMain.h"

#include "tclap/CmdLine.h"

#include <string>
#include <iostream>

int
main(int argc, char** argv) {
    int numEventsPerObject = 10000;
    int numObjects = 5;
    int numBallsAtOnce = 1;
    bool randomDelays = false;

    std::string configuration = "";
    std::string simulateUntil = "";
    bool debug = false;

    try {
        TCLAP::CmdLine cmd("Ping Pong Simulation");

        TCLAP::ValueArg<int> numBallsArg("", "numBalls", "number of balls",
                                         false, numEventsPerObject, "int", cmd);
        TCLAP::ValueArg<int> numPlayersArg("", "numPlayers",
                                           "number of simulation objects to pass the balls",
                                           false, numObjects, "int", cmd);
        TCLAP::ValueArg<int> numAtOnceArg("", "numAtOnce", "number of balls circulating at once",
                                          false, numBallsAtOnce, "int", cmd);
        TCLAP::SwitchArg randomDelaysArg("", "randomDelays",
                                         "should the simulation objects wait random amounts of time",
                                         cmd, randomDelays);

        // Arguments for WarpedMain
        TCLAP::ValueArg<std::string> configurationArg("c", "configuration", "WARPED configuration file",
                                                      false, configuration, "file", cmd);
        TCLAP::ValueArg<std::string> simulateUntilArg("u", "simulateUntil", "specify a simulation end time",
                                                      false, simulateUntil, "time", cmd);
        TCLAP::SwitchArg debugArg("d", "debug", "display debug messages", cmd, debug);

        cmd.parse(argc, argv);

        numEventsPerObject = numBallsArg.getValue();
        numObjects = numPlayersArg.getValue();
        numBallsAtOnce = numAtOnceArg.getValue();
        randomDelays = randomDelaysArg.getValue();

        configuration = configurationArg.getValue();
        simulateUntil = simulateUntilArg.getValue();
        debug = debugArg.getValue();

    } catch (TCLAP::ArgException& e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }

    WarpedMain wm(new PingPongApplication(numObjects, numEventsPerObject, numBallsAtOnce, randomDelays),
                  configuration, simulateUntil, debug);

    // Defaults - 5 objects, 100 events apiece
    return wm.main(argc, argv);
}
