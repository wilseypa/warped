#ifndef SEQUENTIAL_CONFIGURATION_MANAGER_H
#define SEQUENTIAL_CONFIGURATION_MANAGER_H


#include "warped.h"
#include "ConfigurationManager.h"
#include "EventSetFactory.h"
#include "SequentialSimulationManager.h"

class Application;

/** The SequentialConfigurationManager class.

    The SequentialConfigurationManager class represents the concrete
    class that is responsible for constructing and configuring a
    Sequential Simulation Manager and its components.
*/
class SequentialConfigurationManager : public ConfigurationManager {
public:

    /**@name Public Class Methods of SequentialConfigurationManager */
    //@{

    /// Default constructor
    SequentialConfigurationManager(Application* initApplication);

    /// Default destructor
    ~SequentialConfigurationManager();

    /**
        configure the sequential simulation manager.
    */
    void configure(SimulationConfiguration& configuration);

    vector<Configurer*>& getConfigurers() const;

    /**
       Gets the simulation manager for this configuration.

       @return The Simulation Manager;
    */
    SimulationManager* getSimulationManager();

    //@} // End of Public Class Methods of SequentialConfigurationManager

protected:

    /**@name Protected Class Attributes of SequentialConfigurationManager */
    //@{

    /// handle to the sequential simulation manager
    SequentialSimulationManager* const mySimulationManager;

    /// handle to the event set factory
    EventSetFactory* myEventSetFactory;

    //@} // End of Protected Class Attributes of SequentialConfigurationManager

};
#endif
