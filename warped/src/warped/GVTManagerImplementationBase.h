#ifndef GVT_MANAGER_IMPLEMENTATION_BASE_H
#define GVT_MANAGER_IMPLEMENTATION_BASE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "GVTManager.h"

class TimeWarpSimulationManager;
class ObjectID;
/** The GVTManagerImplementationBase base class.

    This is the implementation base class for the various GVT
    Estimation Algorithm implementations in the Simulation
    kernel. Data/Methods common to all gvt managers are defined here.

*/
class GVTManagerImplementationBase : public GVTManager {
public:
   
  /**@name Public Class Methods of GVTManagerImplementationBase. */
  //@{

  /// Default constructor
  GVTManagerImplementationBase( TimeWarpSimulationManager *simMgr,
			       unsigned int period);
   
  /// Destructor
  virtual ~GVTManagerImplementationBase();

  /// Set the LTSE while circulating the red token
  virtual void calculateGVTInfo(const VTime &receiveTime);

  /// get any gvt manager specific info 
  virtual const string getGVTInfo( unsigned int srcSimMgr,
				   unsigned int destSimMgr,
				   const VTime &sendTime);
   
  /// update this simulation manager's record of events it sent out
  virtual void updateEventRecord(const char *infoStream,
				 unsigned int srcSimMgr);

  /// is it time to start a GVT estimation cycle
  virtual bool checkGVTPeriod();
   
  /// return the GVT value to the caller
  const VTime &getGVT();
   
  /** Calculate the actual value of GVT.
       
  This function should only be called in the function that is
  responsible for the calculation of the "real" gVT.  It is
  assumed that simulation manager 0 is responsible for gVT
  calculation.
       
  */
  virtual void calculateGVT();
   
  /// update everybody in the simulation with the new GVT value
  virtual void sendGVTUpdate();

  void configure( SimulationConfiguration &configuration );
   
  /**
     Set a new GVT value.
  */
  void setGVT( const VTime &newGVT );

  virtual bool getGVTTokenStatus();


  //@} // End of Public Class Methods of GVTManager.
   
protected:
  /// number of cycles to wait before gvt estimation is attempted
  unsigned int gVTPeriodCounter;

  /// number of cycles to wait - user specifies this value
  unsigned int gVTPeriod;

  /// handle to the simulation manager that owns this gvt manager
  TimeWarpSimulationManager *mySimulationManager;
   
  
private:
  /**@name Protected Class Attributes of GVTManagerImplementationBase. */
  //@{
   
  /// the global virtual time of the simulation
  const VTime *gVT;
   
  //@} // End of Protected Class Methods of GVTManagerImplementationBase.
};
#endif

