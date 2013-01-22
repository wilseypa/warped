#ifndef ADAPTTESTOBJECT_HH
#define ADAPTTESTOBJECT_HH

// See copyright notice in file Copyright in the root directory of this archive.

#include <warped/warped.h>
#include <warped/SimulationObject.h>
#include <vector>
using std::vector;

/**
   This class is the simulation object for the AdaptTest simulation.
   The purpose of this simulation is to induce rollbacks.
*/
class AdaptTestObject : public SimulationObject {
  
public :
  /**
     Constructor
     @param initObjectId      The name of this object
     @param initDestName      Who we're hitting our ball at.
     @param initNumberOfBalls How many balls we expect to receive.
  */
  AdaptTestObject( const int initObjectId,
	           const string &destName,
	           const int numberOfBalls,
	           bool adaptiveState,
                   string outputMode );
  
  ~AdaptTestObject();
  
  void initialize();
  void finalize();
  
  void executeProcess();
  
  State *allocateState();
  void deallocateState( const State* state );
  
  void reclaimEvent(const Event *event);

  static string getName( int forId );

  const string &getName() const { return myObjectName; }
  
private :
  const string myObjectName;
  const string myDestObjectName;
  const int numStragglers;
  SimulationObject *sendTo;
  const bool adaptiveState;
  bool rollbackOccurred;
  int rollbackCount;
  string outputMode;
  vector<ObjectID> objects;

  void startEvent();
};

#endif

