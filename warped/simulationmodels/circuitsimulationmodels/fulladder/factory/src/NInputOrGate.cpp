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
//          Manas Minglani              minglams@mail.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: NInputAndGate.cpp
// 
//---------------------------------------------------------------------------

#include "../include/NInputOrGate.h"

NInputOrGate::NInputOrGate(string &objectName, const int numInputs,
                             const int numObjects,
                             /*const int numInValue,*/
                             vector<string> *outputs,
                             vector<int> *desInputPorts,
                             int objectDelay)
   : NInputGate(objectName, numInputs, numObjects,/*numInValue,*/outputs, desInputPorts,objectDelay){}

int
NInputOrGate::computeOutput(const int* inputBits) {
   int result = 0;
   for(int i = 0; i < numberOfInputs; i++){
      result = (result | inputBits[i]);
   }
   return result;
}

