#include "NotGate.h"

NotGate::NotGate(string &objectName, const int numInputs,
                 const int numObjects,vector<string> *outputs,
                 vector<int> *desInputPorts,int objectDelay)
                :NInputGate(objectName, numInputs, numObjects,outputs, desInputPorts,objectDelay){}

int
NotGate::computeOutput(const int* inputBits) {
   int result;// = 0;
     //std::cout<<"the inputbit is :"<<inputBits[0]<<std::endl;
     //std::cout<<"the inputbit is :"<<inputBits[1]<<std::endl;
     /* for(int i = 0; i < numberOfInputs; i++){
          result = (result | inputBits[i]);
     }*/ 
   switch(inputBits[0]){
     case 0:
       result = 1;
       break;
     case 1:
       result = 0;
       break;
     default:
       result = -1;
     	 std::cerr<<"the output of a NOT gate should be 1 or 0."<<std::endl;
     	 std::cerr<<"the value is "<<inputBits[0]<<std::endl;
       std::abort();
   }

   return result;
}

