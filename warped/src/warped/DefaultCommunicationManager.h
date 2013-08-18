#ifndef DEFAULT_COMMUNICATION_MANAGER_H
#define DEFAULT_COMMUNICATION_MANAGER_H


#include "warped.h"
#include "SimulationManager.h"
#include "CommunicationManagerImplementationBase.h"

#include <vector>

/** The DefaultCommunicationManager class.

    This is the default communication manager implementation seen by
    the kernel. There are no optimizations built into this manager.
    This class is derived from the CommunicationManager abstract base
    class.

*/
class DefaultCommunicationManager :
   public CommunicationManagerImplementationBase {

public:

   /**@name Public Class Methods of DefaultCommunicationManager. */
   //@{
  
   /** Constructor.

       @param initPhysicalLayer Handle to the physical comm. layer.
       @param initSimulationManager Handle to the Simulation Manager.
   */
   DefaultCommunicationManager( PhysicalCommunicationLayer *initPhysicalLayer,
				TimeWarpSimulationManager *initSimulationManager );

   /// Destructor.
   ~DefaultCommunicationManager();

  virtual void configure( SimulationConfiguration & ){}
  
  //@} // End of Public Class Methods of DefaultCommunicationManager

};

#endif
