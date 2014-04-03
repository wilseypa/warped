#ifndef COMMUNICATION_MANAGER_FACTORY_H
#define COMMUNICATION_MANAGER_FACTORY_H


#include <string>                       // for string

#include "Configurer.h"                 // for string, Configurer
#include "warped.h"

using std::string;

class CommunicationManager;
class Configurable;
class PhysicalCommunicationLayer;
class SimulationConfiguration;
class TimeWarpSimulationManager;

/** The CommunicationManagerFactory class.

The CommunicationManagerFactory class is the interface
through which the various communication manager implementations
that are available in the simulation kernel are accessed. The
specific communication manager implementation is hidden from the
other components of the simulation kernel by the aggregation of
the communication manager implementations within this class
( factory pattern).  All communication manager
implementations ({\it DefaultCommunicationManager} and {\it
MsgAggregatingCommunicationManager}) must, however, be derived
from the CommunicationManager abstract base class.
*/

class CommunicationManagerFactory : virtual public Configurer {
public:

    /**@name Public Class Methods of CommunicationManagerFactory. */
    //@{

    /// Constructor.
    CommunicationManagerFactory();

    /// Destructor.
    virtual ~CommunicationManagerFactory();

    /** Set the configured communication manager implementation.

      Function to attach the particular communication manager
      implementation in the kernel. Will be called by the
      configuration step to set the appropriate communication manager
      set implementation.

      @param configurationMap Contains configuration parameters.
      @param mySimulationManager handle to the sim. mgr.
    */
    Configurable* allocate(SimulationConfiguration& configuration,
                           Configurable* parent) const;

    static const CommunicationManagerFactory* instance();

    //@} // End of Public Class Methods of CommunicationManagerFactory

private:

    /**@name Private Class Attributes of CommunicationManagerFactory. */
    //@{

    /** The MPI layer may or may not be compiled in.  This method will
    return a MPIPhysicalCommunicationLayer if it was, and 0 if it
    wasn't.
      */
    static PhysicalCommunicationLayer* allocatePhysicalCommunicationLayer(const string&);

    //@} // End of Private Class Attributes of CommunicationManagerFactory.
};

#endif
