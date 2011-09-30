#ifndef APPLICATION_H
#define APPLICATION_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "warped/Configurable.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class ArgumentParser;  // see <utils/ArgumentParser.h>
class SimulationObject;
class PartitionInfo;

/** The Application abstract class.

    This is an abstract class (which is actually never instantiated)
    that acts as a place holder for the createObjects() method.  This
    method must be defined by the application.  The method should
    return a pointer to the list of simulation objects that are
    involved in this simulation.
*/

class Application : public Configurable {
public:
   /**@name Public Class Methods of Application */
  //@{

  /** This method is invoked by the kernel so that the application can make any
      initializations or processing that is required by it.  In general it is
      strongly suggested that the application must perform all the necessary
      checks in this method so that the simulation can be aborted (as early as
      possible) incase of errors. 

      @param arguments The command line parameters
      @return ErrorCode (non-zero return value implies an error)
      
      The command line parameters passed to this method are those command line
      parameters that the warped kernel did not use/recognize.  The command
      line parameters do not contain those parameters that are used by warped.
  */
  virtual int initialize( vector <string> &arguments ) = 0;

  /**
     The kernel can/will request partitioning information from the
     application.  The application must at least give back information for
     one partition - this is how the simulation objects are handed back to
     the kernel.  After the kernel calls this method, it will delete the
     returned PartitionInfo.
     
     @return A reference to partitioning information for at least one
     partition.
  */
  virtual const PartitionInfo *getPartitionInfo( unsigned int numProcessorsAvailable ) = 0;
  
  /** This method is invoked by the kernel to find out how many simulation
      objects are involved in this simulation
      
      @param MgrId The id of the simulation manager
      @return number of simulation objects. (-1) indicates error
  */
  virtual int getNumberOfSimulationObjects(int mgrId) const = 0;
  
  /** This method is invoked by the kernel so that the application can wind up
      and perform any cleanups etc.

      @return Error code (non-zero return value indicates error)
  */
  
  virtual int finalize() = 0;

  /** This method is invoked by the kernel inorder to display the list of
      valid paramters for the application modules that a user can use/needs
      to specify
  */
  
  virtual string getCommandLineParameters() const = 0;
  
  //@} // End of public class methods of Application

  /// Destructor.
  virtual ~Application() {}

  /**
     The user is not forced to override this.
  */
  virtual void configure( SimulationConfiguration & ){}

  /**
     The application should register all of it's deserializers when this
     callback is called.
  */
  virtual void registerDeserializers() = 0;

  /**
     Returns positive infinity in this application's time definition.
  */
  virtual const VTime &getPositiveInfinity() = 0;

  /**
     Returns zero in this application's time definition;
  */
  virtual const VTime &getZero() = 0;

  /**  
     Returns a specified time in this application's time definition;
  */
  virtual const VTime &getTime(string&) = 0;

protected:
  /** Default constructor. It is protected to make sure this class never
      gets instantiated directly, but instead gets instantiated by
      derivation. */

  Application() {}

private:
};

#endif
