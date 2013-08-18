#include "../include/NInputAndGate.h"

NInputAndGate::NInputAndGate(string &objectName, const int numInputs,
                             const int numObjects,
                             vector<string> *outputs,
                             vector<int> *desInputPorts,
                             int objectDelay)
                             :NInputGate(objectName, numInputs, numObjects,outputs, desInputPorts,objectDelay){}

int
NInputAndGate::computeOutput(const int* inputBits) {
  int result = 1;
  for(int i = 0; i < numberOfInputs; i++){
    result = (result & inputBits[i]);
  }
  return result;
}

