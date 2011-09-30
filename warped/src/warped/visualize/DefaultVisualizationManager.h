#ifndef DEFAULT_VISUALIZATION_MANAGER_H
#define DEFAULT_VISUALIZATION_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>
#include <string>
//g++-4.2 does not allow unordered_map to be used in this case
#include <map>
#include "VisualizationManager.h"
#include <warped/Socket.h>
#include <stdlib.h>
#include <string.h>

using std::string;
using std::map;
using std::cerr;

/** The DefaultVisualizationManager class.

    This is the class for visualization thingies.  More documentation to
    follow... 

        "It Better" -DJ

*/
class DefaultVisualizationManager : public VisualizationManager {
 public:

  /**@name Public Class Methods of DefaultVisualizationManager. */
  //@{

  /** Default constructor.

      @param void
   */
  DefaultVisualizationManager(TimeWarpSimMgrWithVisualization *, 
			      map<const string, string>& configurationMap);

  /// destructor
  ~DefaultVisualizationManager();

  int openVisualizationObject(const char *name, 
			      VisualizationManager::DisplayKind kind, 
			      VisualizationManager::VisualizationTools vizTools);
 
  int openVisualizationObject(const char *name, 
			      VisualizationManager::DisplayKind kind, 
			      VisualizationManager::VisualizationTools vizTools, 
			      VisualizationManager::DataType, void *);


  void closeVisualizationObject(int);

  void addListener(int, void *);
  void updateVisalizationValue(int);

  void receiveData(char *);

protected:

  Socket *server;

private:

};

#endif
