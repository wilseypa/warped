#ifndef RAID_FACTORY_MANAGER_H
#define RAID_FACTORY_MANAGER_H

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
// $Id: RAIDFactoryManager.h
// 
//---------------------------------------------------------------------------

#include "warped.h"
#include "FactoryManager.h"

class FactoryImplementationBase;

/** The class RAIDFactoryManager.

    The RAID Factory.
*/
class RAIDFactoryManager : public FactoryManager {
public:
  /**@name Public Class Methods of RAIDFactoryManager */
  //@{

  /** Constructor.

      @param parent Handle to the parent factory.
  */
  RAIDFactoryManager(FactoryImplementationBase *parent);

  /// Destructor.
  ~RAIDFactoryManager() {};

  /** Get the name of the factory manager.

      @return A reference to the factory manager's name.
  */
  const string& getName() const {
     static string name("RAIDFactoryManager");
     return name;
  }
  //@} // End of Public Class Methods of RAIDFactoryManager.
};

#endif

