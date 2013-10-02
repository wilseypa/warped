#ifndef GVT_MANAGER_FACTORY_H
#define GVT_MANAGER_FACTORY_H


#include "warped.h"
#include "GVTManager.h"
#include "SimulationManager.h"
#include "Configurer.h"

/** The GVTManagerFactory class.

    This is the  factory interface for the various GVT
    Estimation Algorithm implementations in the Simulation kernel. The
    specific gvt estimation implementation is hidden from the other
    components of the simulation kernel by the aggregation of the
    different implemetations in this class interface ( factory
    pattern).  All gvt estimation implementations must however be
    derived from the GVTManager abstract base class.

*/
class GVTManagerFactory : virtual public Configurer {
public:

    /**@name Public Class Methods of GVTManagerFactory. */
    //@{

    /// Constructor.
    GVTManagerFactory();

    /// Destructor
    virtual ~GVTManagerFactory();

    /** Configures the GVT manager.

    @param configurationMap The set of configuration options set.
    */
    Configurable* allocate(SimulationConfiguration& configuration,
                           Configurable* parent) const;

    static const GVTManagerFactory* instance();

    //@} // End of Public Class Methods of GVTManagerFactory.

private:

    /**@name Private Class Attributes of GVTManagerFactory. */
    //@{

    //@} // End of  Private Class Attributes of GVTManagerFactory.

};

#endif
