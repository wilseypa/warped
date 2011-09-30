#ifndef OUTPUT_MANAGER_FACTORY_H
#define OUTPUT_MANAGER_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Configurer.h"

class Event;
class SimulationObject;
class OutputManager;

/** The OutputManagerFactory class.

    This is the class interface for the various output events manager
    implementations that are available in the Simulation kernel. The
    specific output events manager implementation is hidden from the
    other components of the simulation kernel by the aggregation of
    the output manager implemetations in this class interface
    ( factory pattern).  All output events manager
    implementations must be derived from the OutputManager abstract
    base class. The choices are: AggressiveOutputManager,
    LazyOutputManager, and AdaptiveOutputManager.

*/
class OutputManagerFactory : virtual public Configurer {
public:

  /**@name Public Class Methods of OutputManagerFactory. */
  //@{
  
  /** Constructor.
      
      @param SimulationManager Handle to the sim. mgr.
      @param numObjects Number of simulation objects.
  */
  OutputManagerFactory();			
  
  /// Destructor.
  virtual ~OutputManagerFactory();
  
  /** Set the configured output manager implementation.
      
      Function to attach the particular output manager implementation in
      the kernel. Will be called by the configuration step to set the
      appropriate output manager implementation.

      @param configurationMap Configuration options.
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const OutputManagerFactory *instance();
   
  //@} // End of Public Class Methods of OutputManagerFactory.
  
protected:
  
  /**@name Private Class Attributes of OutputManagerFactory. */
  //@{

  //@} // End of Private Class Attributes of OutputManagerFactory.
};

#endif
