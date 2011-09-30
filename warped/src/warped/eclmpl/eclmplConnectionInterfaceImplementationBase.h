#ifndef ECLMPL_CONNECTION_INTERFACE_IMPLEMENTATION_BASE
#define ECLMPL_CONNECTION_INTERFACE_IMPLEMENTATION_BASE

#include "eclmplCommonInclude.h"
#include "eclmplConnectionInterface.h"
#include "eclmplContactInfo.h"
#include "eclmplConfigFileTable.h"

/** The slaveStartupInfo struct.

    This structure is used for storing information necessary to
    start-up a slave.
*/
struct slaveStartupInfo {
  /**@name Public Struct Attributes of slaveStartupInfo. */
  //@{
  
  /// Configuration table entry for a particular slave.
  vector<string> configTableEntry;

  /// Information that the slave needs to contact the master.
  eclmplContactInfo masterContactInfo;

 //@} // End of Public Struct Attributes of slaveStartupInfo.
};

/** The eclmplConnectionInterfaceImplementationBase class.

    This class implements some basic features of the abstract class
    eclmplConnectionInterface. This class cannot be used in itself as
    a connection interface, but provides implementations for tasks
    that can be directly used by a most connection interfaces, such as
    parsing a configuration file and forking off slaves by a
    master. 
*/
class eclmplConnectionInterfaceImplementationBase : public eclmplConnectionInterface {
public:
  /**@name Public Class Methods of eclmplConnectionInterfaceImplementationBase. */
  //@{

  /** Constructor.

      @param mtuSize Maximum transfer unit (in bytes).
  */
  eclmplConnectionInterfaceImplementationBase(const unsigned int &mtuSize);

  /// Destructor.
  virtual ~eclmplConnectionInterfaceImplementationBase();

  /** Establish connections.

      See eclmplConnectionInterface for description.

      @return True if successful, false otherwise.
      @param argc Number of startup arguments.
      @param argv Startup arguments.  
  */
  virtual bool establishConnections(const int * const argc, 
				    const char * const * const * const argv);

  /** Tear down connections.

      See eclmplConnectionInterface for description.
  */  
  virtual void tearDownConnections();

  /** Send message.

      See eclmplConnectionInterface for description.

      This method must be implemented by anyone inheriting this class.

      @param msgSize Size of message in bytes.
      @param msg Message to be transmitted.
      @param destinationId Destination of the message.  
  */
  virtual void send(const unsigned int &msgSize, const char * const msg, 
		    const unsigned int &destinationId);

  /** Received message.

      See eclmplConnectionInterface for description.

      This method must be implemented by anyone inheriting this class.

      @return True if a message was retrieved, false otherwise.
      @param msgSize Value-return parameter for retrieved message.
      @param msg A buffer than should be allocated prior to method call.
      @param sourceId Id of peer that sent message.
  */
  virtual bool recv(unsigned int &msgSize, char * const msg, unsigned int &sourceId);  

  //@} // End of Public Class Methods of eclmplConnectionInterfaceImplementationBase.

protected:
  /**@name Protected Class Methods of eclmplConnectionInterfaceImplementationBase. */
  //@{

  /** Default constructor.

      Having default constructor as protected prohibits instantiation
      without specifying mtu.
  */
  eclmplConnectionInterfaceImplementationBase(){}

  /** Scan a configuration file and store it.

      This method will scan a configuration file and store it in an
      eclmplConfigFileTable object. A new object will be created and
      returned, so whoever is calling this method is responsible for
      freeing up the memory that was allocated.

      @return A new populated configuration table.
      @param fileName Full path file name of the configuration file.
      @param argsPerEntry Number of arguments per line in the file.
  */
  virtual eclmplConfigFileTable* scanConfigFile( const string &fileName, 
						 int argsPerEntry = 2 );

  /** Parse command line arguments.

      This method will parse command line arguments that were passed
      by the master who forked the slave that is this connection
      interface. A slaveStartupInfo structure will be populated so
      that the slave knows how to contact the master.
      
      @return Startup info for the slave.
      @param argc Number of arguments to parse.
      @param argv Arguments to parse.
   */
  virtual slaveStartupInfo parseCommandLineArguments(const int * const argc, 
						     const char * const * const * const argv);

  /** Create command line arguments for the slave to parse.

      This method will create the command line arguments that will be
      read by a forked off slave in order to successfully establish
      connections.

      @param argc Number of arguments that the master was started with.
      @param argv Arguments that the master was started with.
      @param newArgc Number of arguments that the slave will be started with.
      @param newArgv Arguments that the slave will be started with.
      @param configTableEntry Configuration file entry for the slave.
      @param masterContactInfo Information necessary for the slave to contact the master.
      @param id Connection id of slave to be started.
  */
  virtual void createSlaveCommandLineArguments(const int * const argc, 
					       const char * const * const * const argv,
					       int &newArgc, char **&newArgv,
					       const vector<string> &configTableEntry,
					       const eclmplContactInfo &masterContactInfo,
					       const unsigned int &id);

  /** Fork off slave.

      This method is called by the master in order to fork off
      slaves. The total number of slaves that will be forked off is
      numberOfConnections - 1.

      @param argc Number of arguments that the master was started with.
      @param argv Arguments that the master was started with.
      @param connTable The scanned configuration file.
      @param masterContactInfo Information about how to contact the master.
      @param id Connection Id of slave to fork off.
  */
  virtual void forkOffSlave(const int * const argc, 
			    const char * const * const * const argv,
			    const eclmplConfigFileTable &connTable,
			    const eclmplContactInfo &masterContactInfo,
			    const unsigned int &id);

  /** Fork off slaves.

      This method is called by the master in order to fork off
      slaves. The total number of slaves that will be forked off is
      numberOfConnections - 1.

      @param argc Number of arguments that the master was started with.
      @param argv Arguments that the master was started with.
      @param connTable The scanned configuration file.
      @param masterContactInfo Information about how to contact the master.
  */
  virtual void forkOffSlaves(const int * const argc, 
			     const char * const * const * const argv, 
			     const eclmplConfigFileTable &connTable, 
			     const eclmplContactInfo &masterContactInfo) ;

  /** Distribute a configuration table to all slaves.

      @param connTable The configuration table.
  */
  virtual void distributeConfigFileTable(const eclmplConfigFileTable &connTable);

  /** Master establishes connections wiht slaves.

      This method is called by the master in order to establish
      connections with the slaves.

      @param argc Number of arguments that the master was started with.
      @param argv Arguments that the master was started with.
      @param connTable The scanned configuration file.
  */
  virtual void establishConnections(const int * const argc, 
				    const char * const * const * const argv,
				    const eclmplConfigFileTable &connTable) = 0;

  /** Slave establishes connections with everyone else.

      This method is called by the slave in order to establish
      connections with everyone else. In most cases this means first
      establishing a connection with the master, receiving information
      about all peers, and then finally establishing connections with
      all these peers.

      @param info Information necessary to initialize slave and begin connection establishment.
  */
  virtual void establishConnections(const slaveStartupInfo &info) = 0;

  //@} // End of Protected Class Methods of eclmplConnectionInterfaceImplementationBase.
};

#endif
