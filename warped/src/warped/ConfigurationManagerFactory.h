#ifndef CONFIGURATION_MANAGER_FACTORY_H
#define CONFIGURATION_MANAGER_FACTORY_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "Configurer.h"
class Application;

/** Creates ConfigurationManagers

*/
class ConfigurationManagerFactory : virtual public Configurer {
public:
  static const ConfigurationManagerFactory *instance();

  /**
     Allocates a ConfigurationManager based on the current configuration.
     
     @param configuration The configuration used to determine what to build.

     @param parent This should be a handle to the application that we're getting
     ready to start.
   */
  Configurable *allocate( SimulationConfiguration &configuration,
			  Configurable *parent ) const;

protected:
  
  /// Default destructor
  virtual ~ConfigurationManagerFactory(){};
  
private:
  /// Default constructor
  ConfigurationManagerFactory(){};

};
#endif
