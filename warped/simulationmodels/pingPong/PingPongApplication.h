#ifndef PING_PONG_APPLICATION
#define PING_PONG_APPLICATION

// Copyright (c) Clifton Labs, Inc.
// All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors:
//          Dale E. Martin              dmartin@cliftonlabs.com

#include <warped/Application.h>
#include <warped/IntVTime.h>

class PingPongApplication : public Application {
public:
  PingPongApplication( unsigned int initNumObjects, 
		       unsigned int numEventsPerObject,
		       unsigned int initNumBallsAtOnce,
		       bool initRandomDelays );

  int initialize( vector<string> &arguments );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  string getCommandLineParameters() const { return "foo"; }

  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &){ return IntVTime::getIntVTimeZero(); }

private:
  vector<SimulationObject *> *getSimulationObjects();

  unsigned int numObjects;
  unsigned int numEventsPerObject;
  unsigned int numBallsAtOnce;
  bool randomDelays;
};

#endif
