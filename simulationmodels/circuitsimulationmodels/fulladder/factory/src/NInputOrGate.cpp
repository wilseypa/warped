#include "../include/NInputOrGate.h"

NInputOrGate::NInputOrGate(string &objectName, const int numInputs,
                             const int numObjects,
                             vector<string> *outputs,
                             vector<int> *desInputPorts,
                             int objectDelay)
                             :NInputGate(objectName, numInputs, numObjects,outputs, desInputPorts,objectDelay){}

int
NInputOrGate::computeOutput(const int* inputBits) {
  int result = 0;
  for(int i = 0; i < numberOfInputs; i++){
    result = (result | inputBits[i]);
  }
  return result;
}

