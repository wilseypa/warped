#ifndef LOGIC_FACTORY_MANAGER_H
#define LOGIC_FACTORY_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "FactoryManager.h"

class FactoryImplementationBase;

/** The LogicFactoryManager class.

    This class represents a manager class. All objects in a logic
    simulation must be registered with such a manager.

*/
class LogicFactoryManager : public FactoryManager {
public:
   
   /**@name Public Class Methods of LogicFactoryManager. */
   //@{

   /// Default constructor
   LogicFactoryManager(FactoryImplementationBase *parent);

   /// Default destructor
   ~LogicFactoryManager() {};

   /// return the name of this manager
   const string& getName() const {
      static string name("LogicFactoryManager");
      return name;
   }

   //@} // End of Public Class Methods of LogicFactoryManager.
};

#endif
  
