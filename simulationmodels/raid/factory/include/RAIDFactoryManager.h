#ifndef RAID_FACTORY_MANAGER_H
#define RAID_FACTORY_MANAGER_H

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

