#ifndef STATE_MANAGER_FACTORY_H
#define STATE_MANAGER_FACTORY_H


#include "warped.h"
#include <fstream>
#include "StateManager.h"
#include "SimulationManager.h"
#include "Configurer.h"

class State;
class SimulationObject;

/** The StateManagerFactory class.
    
    This is the class factory for the various state manager
    implementations that are available in the simulation kernel. The
    specific state manager implementation is hidden from the other
    components of the simulation kernel by the unification of all
    state manager implementations into this class.  All
    state manager implementations must be derived from the
    StateManager abstract base class.
**/
class StateManagerFactory : virtual public Configurer {
public:
   
  /**@name Public Class Methods of StateManagerFactory. */
  //@{
   
  /// Default constructor.
  StateManagerFactory( );
  /// Destructor.
  virtual ~StateManagerFactory();

  /** Set the configured state manager implementation.
      
  Function to attach the particular state manager implementation in the
  kernel. Will be called by the configuration step to set the
  appropriate state manager set implementation.

  @param configurationMap Set of configuration options.
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const StateManagerFactory *instance();
   
  //@} // End of Public Class Methods of StateManagerFactory. */

private:
  /**@name Private Class Attributes of StateManagerFactory. */
  //@{

  //@} // End of Private Class Attributes of StateManagerFactory.
   
};

#endif

