#ifndef NULL_VISUALIZATION_MANAGER_H
#define NULL_VISUALIZATION_MANAGER_H


#include "VisualizationManager.h"

/** The NullVisualizationManager class.

    This is the class for visualization thingies.  More documentation to
    follow... 

        "It Better" -DJ

*/
class NullVisualizationManager : public VisualizationManager {
 public:

  /**@name Public Class Methods of NullVisualizationManager. */
  //@{

  /** Default constructor.

      @param void
   */
  NullVisualizationManager(TimeWarpSimMgrWithVisualization *);

  /// destructor
  ~NullVisualizationManager();

  virtual int openVisualizationObject(const char *name, 
				      VisualizationManager::DisplayKind kind, 
				      VisualizationManager::VisualizationTools vizTools) = 0;
 
  virtual int openVisualizationObject(const char *name, 
				      VisualizationManager::DisplayKind kind, 
				      VisualizationManager::VisualizationTools
				      vizTools, VisualizationManager::DataType,
				      void *);

  virtual void closeVisualizationObject(int);

  virtual void addListener(int, void *);
  virtual void updateVisalizationValue(int);
};

#endif
