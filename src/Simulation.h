#ifndef SIMULATION_H
#define SIMULATION_H


#include <fstream>
#include <string>                       // for string

#include "Configurable.h"               // for Configurable
#include "ConfigurationManager.h"       // for ConfigurationManager
#include "Configurer.h"                 // for string
#include "SimulationManager.h"
#include "warped.h"                     // for SEVERITY

using std::string;

class Application;
class SimulationConfiguration;
class SimulationManager;
class VTime;

/** The Simulation class.

    The Simulation class represents the top level configuration module. It
    is a singleton class that is present to start and guide the run-time
    configuration of the desired simulation.

*/
class Simulation : virtual public Configurable {
public:
    /**@name Public Class Methods of Simulation. */
    //@{

    /**
       Return the Simulation singleton.

       @param configuration This method should be called exactly once with
       this parameter specified.  It will be used to initialize the
       singleton.  Subsequent calls with a parameter specified will result in
       a runtime error.

       @return The Simulation singleton instance.
    */
    static Simulation* instance(SimulationConfiguration* configuration = 0,
                                Application* userApplication = 0);

    /** Top level configuration of SimulationManager.

        A call to this function causes the configuration of the entire
        simulator to take place.

        @param argc Pointer to the number of command line parameters
        @param argv Pointer to the command line paramters

        @return a pointer to the SimulationManager that is constructed
    */
    void configure(SimulationConfiguration& configuration);

    /** report an error condition depending on the severity level.

        A call to this function results in the formatted printing out of
        warning or error messages on the console.

        @param msg The error message.
        @param level The level of severity of the error.
    */
    static void reportError(const string& msg, const SEVERITY level);

    /**
       Returns a handle to simulation manager 0.
    */
    SimulationManager* getSimulationManager();

    /**
       Cause the initlialization of the simulation.
    */
    void initialize();

    void simulate(const VTime& simulateUntil);

    void finalize();

    const VTime& getCommittedTime();

    const VTime& getNextEventTime();

    bool simulationComplete();

    //@} // End of Public Class Methods of Simulation.

protected:

private:
    /** Default Constructor is private to ensure the Singleton pattern usage
        @param initApplication The application we were constructed with.
    */
    Simulation(Application* initApplication);

    /**@name Private Class Methods of Simulation. */
    //@{

    /// operator= defined private to prevent compiler from creating one
    void operator=(Simulation&);

    /// copy constructor defined private to prevent compiler from creating one
    Simulation(const Simulation&);

    //@} // End of Private Class Methods of Simulation.

    /**@name Private Class Attributes of Simulation. */
    //@{

    /// a handle to the configuration manager
    ConfigurationManager* myConfigurationManager;

    void cleanUp() {  delete myConfigurationManager; }

    Application* myApplication;

    //@} // End of Private Class Attributes of Simulation.

};

#endif
