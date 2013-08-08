#include "../include/TwoInputAndGateApplication.h"
#include "../include/NInputAndGate.h"
#include "../incoude/"

#include <tclap/CmdLine.h>

int
TwoInputAndGateApplication::initialize(){
  try {
    TCLAP::CmdLine cmd("Two Input AND gate Simulation");

    TCLAP::ValueArg<string> inputFileNameArg("", "simulate", "configuration file name",
                                              true, inputFileName, "string", cmd);

    cmd.parse(arguments);

    inputFileName = inputFileNameArg.getValue();
  } catch (TCLAP::ArgException &e) {
      std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
      exit(-1);
  }

  return 0;
}
