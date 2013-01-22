#ifndef RAID_OBJECT_STUB_H
#define RAID_OBJECT_STUB_H

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
// $Id: RAIDProcessStub.h
// 
//---------------------------------------------------------------------------

#include "warped.h"
#include "ObjectStub.h"

class SimulationObject;

/** The class RAIDProcessStub.
*/
class RAIDProcessStub : public ObjectStub {
public:
   RAIDProcessStub(FactoryImplementationBase *owner) : ObjectStub(owner){}
   ~RAIDProcessStub(){};

   string &getName() const {
      static string name("RAIDProcess");
      return name;
   }

   const string &getInformation() const {
      static string info("A Simple RAID Object");
      return info;
   }

   const bool isLocalObject() const {
      return true;
   }

   SimulationObject *createSimulationObject(int numberOfArguments,
                                            ostringstream &argumentStream);
};

#endif
