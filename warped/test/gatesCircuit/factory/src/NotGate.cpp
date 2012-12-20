//#include "../include/NInputAndGate.h"
//#include "../include/NInputGateState.h"
//#include "../include/NInputGate.h"

#include "../include/LogicEvent.h"
#include "../src/NotGate.h"
#include "../src/NotGateState.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

// outputs has been changed from a vector of pointers 
// to a vectorof string type variables. if reverting back to it please
// put a star in front of outputs. 

/**
   Notations :- 
   vs(variable_name) - vector of string 
   vi(variable_name) - vector of integer
*/

NotGate::NotGate(string &objectName, 
		 const int numInputs,
		 const int numObjects,
		 vector<string> outputs,
		 vector<int> *outputPortNumbers,
		 vector<int> *fanOutSize,
		 int objectDelay)
{
  myObjectName    = objectName;
  numberInputs    = numInputs;
  numberObjects   = numObjects;
  vsOutputGates   = outputs;
  viOutputPorts   = outputPortNumbers;
  viFanOut        = fanOutSize;
  gateObjectDelay = objectDelay;
}

void 
NotGate::initialize()
{
  if (numberObjects != 0 )
    {
      outputHandles.resize( vsOutputGates.size(),NULL );
      for ( int i = 0; i < vsOutputGates.size(); i++ )
	{
	  outputHandles[i] = getObjectHandle ( vsOutputGates[i] );
	}
    }
}

void 
NotGate::executeProcess ()
{
  NotGateState *state = (NotGateState *) getState();
  LogicEvent *logicEvent  = NULL;

  while(true == haveMoreEvents())
    {
      logicEvent = (LogicEvent *) getEvent();
      if(logicEvent !=NULL)
	{
	  state -> iInputBits[logicEvent->destinationPort - 1] = logicEvent -> bitValue2;
	  cout << logicEvent -> destinationPort << ": got bit value " << logicEvent -> bitValue2 << endl;
	  
	  state -> iResult  = !(state -> iInputBits[1]);
	  cout << "result from Not Gate :" << state -> iResult << endl;
	  state -> iResult = 1;
	}
    }
}

State*
NotGate::allocateState()
{
  return new NotGateState();
}

void
NotGate::deallocateState( const State* state )
{
  delete state;
}

void NotGate::reclaimEvent( const Event *event )
{
  delete event;
}

void 
NotGate::reportError( const string& , SEVERITY )
{
  // Nothing is described here
}

void 
NotGate::finalize()
{}
