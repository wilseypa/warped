#include "NInputNandGate.h"

NInputNandGate::NInputNandGate(string &objectName, const int numInputs,
                             const int numObjects,
                             vector<string> *outputs,
                             vector<int> *desInputPorts,
                             int objectDelay)
   : NInputGate(objectName, numInputs, numObjects,outputs, desInputPorts,objectDelay){}

int
NInputNandGate::computeOutput(const int* inputBits) {
  int result = 1;
  for(int i = 0; i < numberOfInputs; i++){
    result = (result & inputBits[i]);
  }
  switch(result){
    case 0:
      result = 1;
      break;
    case 1:
      result = 0;
      break;
    default:
      result = -1;
      std::cerr<<"the output of a NAND gate should be 1 or 0."<<std::endl;
      std::cerr<<"the value is "<<result<<std::endl;
      std::abort();
    }
  return result;
}

