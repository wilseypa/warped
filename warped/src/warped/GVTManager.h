#ifndef GVT_MANAGER_H
#define GVT_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "CommunicatingEntity.h"
#include "Configurable.h"

class ObjectID;

/** The GVTManager abstract base class.

    This is the abstract base class for the various GVT
    Estimation Algorithm implementations in the Simulation kernel.

*/
class GVTManager : virtual public CommunicatingEntity,
		   virtual public Configurable {
public:
   
  /**@name Public Class Methods of GVTManager. */
  //@{

  /// Destructor
  virtual ~GVTManager(){};

  /// is it time to start a GVT estimation cycle
  virtual bool checkGVTPeriod() = 0;
   
  /// return the GVT value to the caller
  virtual const VTime &getGVT() = 0;

  virtual void setGVT(const VTime &setTime) = 0;
   
  /// Calculate any gvt manager specific info. 
  virtual void calculateGVTInfo(const VTime &receiveTime) = 0;

  /// Get any gvt manager specific info.
  virtual const string getGVTInfo( unsigned int srcSimMgr,
				   unsigned int destSimMgr,
				   const VTime &sendTime ) = 0;
   
  /// update this simulation manager's record of events it sent out 
  virtual void updateEventRecord( const string &infoStream,
				  unsigned int srcSimMgr ) = 0;
   
  /** Calculate the actual value of GVT.
       
  This function should only be called in the function that is
  responsible for the calculation of the "real" gVT.  It is
  assumed that simulation manager 0 is responsible for gVT
  calculation.
       
  */
  virtual void calculateGVT() = 0;
   
  /// update everybody in the simulation with the new GVT value
  virtual void sendGVTUpdate() = 0;

  virtual void registerWithCommunicationManager() = 0;

  /** Needed when optimistic fossil collection is used.
  */
  virtual void ofcReset() = 0;

  virtual bool getGVTTokenStatus() = 0;

  //@} // End of Public Class Methods of GVTManager.

protected:
   
  /**@name Protected Class Methods of GVTManager. */
  //@{
   
  GVTManager(){};
   
  //@} // End of Protected Class Methods of GVTManager.
};
#endif

