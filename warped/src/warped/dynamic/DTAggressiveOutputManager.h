#ifndef DTAGGRESSIVEOUTPUTMANAGER_H_
#define DTAGGRESSIVEOUTPUTMANAGER_H_

// See copyright notice in file Copyright in the root directory of this archive.

#include <fstream>
#include "warped.h"
#include "DTOutputManagerImplementationBase.h"

class Event;
class SimulationObject;
class OutputManager;

/** The DTAggressiveOutputManager class.

    This class implements an aggressive cancellation scheme as a part of
    its output manager functionality.

*/
class DTAggressiveOutputManager : public DTOutputManagerImplementationBase {
public:

  /**@name Public Class Methods of DTAggressiveOutputManager. */
  //@{

  /** Constructor.

      @param simMgr Handle to the simulation manager.
  */
  DTAggressiveOutputManager( DTTimeWarpSimulationManager *simMgr );

  /// Destructor.
  ~DTAggressiveOutputManager();


  /** Send out anti-messages.

      @param rollbackTime Bound for how many antimessages will be sent.
      @param object A pointer to the object who experienced rollback.
  */
  void rollback( SimulationObject *object,  const VTime &rollbackTime , int threadID);

  virtual void configure( SimulationConfiguration & ){}

  //@} // End of Public Class Methods of DTAggressiveOutputManager.

};


#endif /* DTAGGRESSIVEOUTPUTMANAGER_H_ */
