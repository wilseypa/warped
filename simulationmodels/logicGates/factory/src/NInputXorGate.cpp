#include "NInputXorGate.h"

NInputXorGate::NInputXorGate(string &objectName, const int numInputs,
                             const int numObjects,
                             vector<string> *outputs,
                             vector<int> *outputPortNumbers,
                             vector<int> *fanOutSize,
                             int objectDelay)
   : NInputGate(objectName, numInputs, numObjects, outputs, outputPortNumbers,
                fanOutSize, objectDelay){}

int
NInputXorGate::computeOutput(const int* inputs) {
   int result = 0;
   for(int i = 0; i < numberOfInputs; i++){
      result = result ^ inputs[i];
   }
   return result;
}
