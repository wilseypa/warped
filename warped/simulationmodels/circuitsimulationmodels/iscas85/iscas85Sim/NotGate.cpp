// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Xinyu Guo                   guox2@mail.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: NotGate.cpp
// 
//---------------------------------------------------------------------------

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

