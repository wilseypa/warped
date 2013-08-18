#include "../include/NInputXorGate.h"

NInputXorGate::NInputXorGate(string &objectName, const int numInputs,
                             const int numObjects,
                             /*const int numInValue,*/
                             vector<string> *outputs,
                             vector<int> *destInputPorts,
                             int objectDelay)
                             :NInputGate(objectName, numInputs, numObjects,/*numInValue,*/outputs, destInputPorts,objectDelay){}

int
NInputXorGate::computeOutput(const int* inputBits) {
  int result =0;
  for(int i = 0; i < numberOfInputs; i++){
    result = (result^inputBits[i]);
  }
  return result;
}

