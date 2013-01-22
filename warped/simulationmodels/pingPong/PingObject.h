#ifndef PINGOBJECT_HH
#define PINGOBJECT_HH

// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

#include <warped/warped.h>
#include <warped/SimulationObject.h>

/**
   This class is the simulation object for the Ping Pong simulation.  The
   entire point of this simulation is to have pathologically low
   granularity.  Don't expect to get parallel speedup on this one.  :-)
*/
class PingObject : public SimulationObject {
  
public :
  /**
     Constructor
     @param initObjectId      The name of this object
     @param initDestName      Who we're hitting our ball at.
     @param initNumberOfBalls How many balls we expect to receive.
  */
  PingObject( const int initObjectId,
	      const string &destName,
	      const int numberOfBalls,
	      bool master,
	      bool randomDelays );
  
  ~PingObject();
  
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
  const int numBalls;
  SimulationObject *sendTo;
  const bool isMaster;
  const bool randomDelays;
  double lastX;

  /**
     Handles the sending of our event to our neighbor.
  */
  void sendEvent( const string &ownerName );

  /**
     Starts a new "ball" in the system.
  */
  void startBall();
};

#endif
