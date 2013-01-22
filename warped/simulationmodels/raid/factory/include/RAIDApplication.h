#ifndef RAID_APPLICATION
#define RAID_APPLICATION

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
//          Randall King                kingr8@mail.uc.edu

#include <warped/Application.h>
#include <warped/IntVTime.h>

class RAIDApplication : public Application {
public:
  RAIDApplication();

  int initialize( vector<string> &arguments );

  int getNumberOfSimulationObjects(int mgrId) const;

  const PartitionInfo *getPartitionInfo( unsigned int numberOfProcessorsAvailable );
  
  int finalize();

  void registerDeserializers();
  
  string getCommandLineParameters() const { return "foo"; }

  const VTime &getPositiveInfinity(){ return IntVTime::getIntVTimePositiveInfinity(); }
  const VTime &getZero(){ return IntVTime::getIntVTimeZero(); }
  const VTime &getTime(string &time){
    IntVTime *vtime = new IntVTime( atoi(time.c_str()) );
    return *vtime;
  }

private:
  ArgumentParser &getArgumentParser();

  vector<SimulationObject *> *getSimulationObjects();

  string inputFileName;
  int numObjects;
};

#endif
