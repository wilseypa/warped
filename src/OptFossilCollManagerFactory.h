#ifndef OPTFOSSILCOLL_MANAGER_FACTORY_H
#define OPTFOSSILCOLL_MANAGER_FACTORY_H


#include "Configurer.h"                 // for Configurer
#include "OptFossilCollManager.h"
#include "SimulationManager.h"
#include "ThreadedOptFossilCollManager.h"
#include "warped.h"

class Configurable;
class SimulationConfiguration;

/** The GVTManagerFactory class.

    This is the  factory interface for the various Optimistic Fossil
    Collection implementations in the Simulation kernel.

*/
class OptFossilCollManagerFactory : virtual public Configurer {
public:

    /**@name Public Class Methods of OptFossilCollManagerFactory. */
    //@{

    /// Constructor.
    OptFossilCollManagerFactory();

    /// Destructor
    virtual ~OptFossilCollManagerFactory();

    /** Configures the OptimisiticFossilCollection manager.

    @param configurationMap The set of configuration options set.
    */
    Configurable* allocate(SimulationConfiguration& configuration,
                           Configurable* parent) const;

    static const OptFossilCollManagerFactory* instance();

    //@} // End of Public Class Methods of OptFossilCollManagerFactory.

private:

    /**@name Private Class Attributes of OptFossilCollManagerFactory. */
    //@{

    //@} // End of  Private Class Attributes of OptFossilCollManagerFactory.

};

#endif
