#ifndef MATTERN_GVT_MANAGER_H
#define MATTERN_GVT_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "GVTManagerImplementationBase.h"
#include "MatternObjectRecord.h"

class SchedulingManager;
class CommunicationManager;
class ObjectID;

/** The MatternGVTManager class.

    This class implements a GVT Estimation algorithms originally
    defined by Mattern. It is essentially a distributed termination
    algorithm that has been modified for use as an GVT estimation
    scheme.

*/
class MatternGVTManager : public GVTManagerImplementationBase {

public:

  /**@name Public Class Methods of MatternGVTManager. */
  //@{

  /// Default Constructor
  MatternGVTManager( TimeWarpSimulationManager *simMgr, unsigned int period );
  /// Special Constructor for defining objectRecord elsewhere when using this manager as a base class
  MatternGVTManager( TimeWarpSimulationManager *simMgr, unsigned int period, bool objectRecordDefined );

  /// Destructor
  ~MatternGVTManager();

  /// calculate any gvt manager specific info
  void calculateGVTInfo(const VTime &receiveTime);

  /// get any gvt manager specific info 
  const string getGVTInfo(unsigned int srcSimMgr, unsigned int destSimMgr, const VTime &sendTime);
  
  /// update this simulation manager's record of events it sent out 
  void updateEventRecord( const string &infoStream, unsigned int srcSimMgr );

  /** Calculate the actual value of GVT.
       
      This function should only be called in the function that is
      responsible for the calculation of the "real" gVT.  It is
      assumed that simulation manager 0 is responsible for gVT
      calculation.
       
  */
  void calculateGVT();

  /// update everybody in the simulation with the new GVT value
  void sendGVTUpdate();

  /// register GVTManager specific message types with the comm. manager
  void registerWithCommunicationManager();

  /// the method the communication manager will call to deliver messages
  void receiveKernelMessage(KernelMessage *msg);
  
  ///Returns true when it is time to start a GVT calculation
  bool checkGVTPeriod();

  /// Resets the period counter and gVTTokenPending.
  /// Used for optimistic fossil collection.
  void ofcReset();

  bool getGVTTokenStatus();

  //@} // End of Public Class Methods of MatternGVTManager.
   
protected:

  /**@name Protected Class Attributes of MatternGVTManager. */
  //@{
  bool zeroWhiteMessagesAtStart;

  /// boolean flag that indicates if a gVTToken is in circulation or not
  bool gVTTokenPending;

  /// handle to the scheduler
  SchedulingManager *myScheduler;

  /// handle to the communication manager
  CommunicationManager *myCommunicationManager;

  /// the data structure that keeps track of sent and received messages
  MatternObjectRecord *objectRecord;

  /// dispatch the GVTTokenMessage to the next simulation manager
  void sendGVTToken(const VTime &lastScheduledEventTime,
		    const VTime &minTimeStamp);

  virtual const VTime *getEarliestEventTime(const VTime *lowEventTime);

  //@} // End of Protected Class Methods of MatternGVTManager.

};

#endif
