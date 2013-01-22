
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
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: NAryBitPrinterStub.cpp
// 
//---------------------------------------------------------------------------

#include "warped.h"
#include "NAryBitPrinter.h"
#include "NAryBitPrinterStub.h"
#include "SimulationObject.h"

SimulationObject*
NAryBitPrinterStub::createSimulationObject(int numberOfArguments,
                                           ostrstream &argumentStream){
   if(numberOfArguments != 3){
      cerr << "Invalid number of parameters for NAryBitPrinter" << endl;
      cerr << "Expected: 3 but got " << numberOfArguments << endl;
      cerr << "Argument Stream = " << argumentStream.str() << endl;
      delete [] argumentStream.str();
      exit(-1);
  }

  istrstream inputStream(argumentStream.str());
  
  string firstArgument; // name of this object
  int secondArgument;   // Number of output objects
  int outputPortNumber; // output port of this object
  string destObjName;   // name of the destination object
  int inputPortNumber;  // name of input port of destination object
  int numBits;
  string printStatus;     // to print or not to print
  bool status;
  

  // break out the input stream into separate fields
  inputStream >> firstArgument >> secondArgument;

  vector<string> *outputObjectNames = new vector<string>;
  vector<int> *destinationPorts = new vector<int>;
  vector<int> *fanOutSize = new vector<int>(secondArgument, 0);

  for(int i = 0; i < secondArgument; i++){
     inputStream >> outputPortNumber >> destObjName >> inputPortNumber;
     (*fanOutSize)[outputPortNumber]++;
     outputObjectNames->push_back(destObjName);
     destinationPorts->push_back(outputPortNumber);
  }

  inputStream >> numBits >> printStatus;

  if(printStatus == "true"){
     status = true;
  }
  else {
     status = false;
  }
  

  SimulationObject *newObject = new NAryBitPrinter(firstArgument,
                                                   secondArgument,
                                                   outputObjectNames,
                                                   destinationPorts,
                                                   fanOutSize,
                                                   numBits,
                                                   status);

  delete [] argumentStream.str();

  return (SimulationObject *) newObject;
}


