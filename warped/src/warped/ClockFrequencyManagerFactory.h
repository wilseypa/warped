#ifndef CLOCK_FREQUENCY_MANAGER_FACTORY_H
#define CLOCK_FREQUENCY_MANAGER_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Configurer.h"


/** The ClockFrequencyManagerFactory class.

    Factory for clock frequency managers

*/
class ClockFrequencyManagerFactory : public Configurer {
public:

  /**@name Public Class Methods of OutputManagerFactory. */
  //@{
  
  /** Constructor.
      
      @param SimulationManager Handle to the sim. mgr.
      @param numObjects Number of simulation objects.
  */
  ClockFrequencyManagerFactory();
  
  /// Destructor.
  virtual ~ClockFrequencyManagerFactory();
  
  /** Set the configured output manager implementation.
      
      Function to attach the particular output manager implementation in
      the kernel. Will be called by the configuration step to set the
      appropriate clock frequency manager implementation.

      @param configurationMap Configuration options.
  */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

  static const ClockFrequencyManagerFactory *instance();
   
  //@} // End of Public Class Methods of OutputManagerFactory.

private:
  static int CPUCount();
};

#endif //CLOCK_FREQUENCY_MANAGER_FACTORY_H
