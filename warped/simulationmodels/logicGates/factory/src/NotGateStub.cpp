// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
// THE SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
//
// $Id: NotGateStub.cpp
//
//---------------------------------------------------------------------------

#include "NotGate.h"
#include "NotGateStub.h"
#include "SimulationObject.h"

SimulationObject*
NotGateStub::createSimulationObject(int numberOfArguments,
                                    ostrstream &argumentStream){
  
   if(numberOfArguments < 3){
      cerr << "Invalid number of parameters for NotGate" << endl;
      cerr << "Expected at least: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
   }

   istrstream inputStream(argumentStream.str());
  
   string objName;
   int numberOfOutputs; // Number of output objects
   string destObjName;
   int inputPortNumber;
   int outputPortNumber;
   int objDelay;

    // break out the input stream into separate fields
   inputStream >> objName >> numberOfOutputs;
   
   vector<string> *outputObjectNames = new vector<string>;
   vector<int> *destinationPorts = new vector<int>;
   vector<int> *fanOutSize = new vector<int>(numberOfOutputs, 0);

   for(int i = 0; i < numberOfOutputs; i++){
      inputStream >> outputPortNumber >> destObjName >> inputPortNumber;
      (*fanOutSize)[outputPortNumber - 1]++;
      outputObjectNames->push_back(destObjName);
      destinationPorts->push_back(inputPortNumber);
   }
   
   inputStream >> objDelay;
   
   SimulationObject *newObject = new NotGate(objName,
                                             numberOfOutputs,
                                             outputObjectNames,
                                             destinationPorts,
                                             fanOutSize,
                                             objDelay);

   delete [] argumentStream.str();

   return (SimulationObject *) newObject;
}

