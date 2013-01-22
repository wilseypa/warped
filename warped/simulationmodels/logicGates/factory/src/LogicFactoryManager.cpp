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
// $Id: LogicFactoryManager.cpp
// 
//---------------------------------------------------------------------------

#include "LogicFactoryManager.h"
#include "FactoryImplementationBase.h"

#include "NInputAndGateStub.h"
#include "NInputXorGateStub.h"
#include "NInputOrGateStub.h"
#include "NAryBitGeneratorStub.h"
#include "NAryBitPrinterStub.h"
#include "HalfAdderStub.h"
#include "NotGateStub.h"

LogicFactoryManager::LogicFactoryManager(FactoryImplementationBase *parent)
   : FactoryManager(parent) {
   add(new NInputAndGateStub(this));
   add(new NInputXorGateStub(this));
   add(new NInputOrGateStub(this));
   add(new NAryBitGeneratorStub(this));
   add(new NAryBitPrinterStub(this));
   add(new HalfAdderStub(this));
   add(new NotGateStub(this));
}

// This is a static method defined in FactoryManager that the user must
// define (or compilation errors will result)
// This is how the simulation kernel gets a handle to the user factory
// that needs to be instantiated.
FactoryManager*
FactoryManager::createUserFactory(){
   return ((FactoryManager *)new LogicFactoryManager(NULL));
}

