//#include "../include/NInputAndGate.h"
//#include "../include/NInputGateState.h"
//#include "../include/NInputGate.h"

#include "../include/LogicEvent.h"
#include "../src/OrGate.h"
#include "../src/OrGateState.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

// outputs has been changed from a vector of pointers 
// to a vectorof string type variables. if reverting back to it please
// put a star in front of outputs. 

OrGate::OrGate(string &objectName, 
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
OrGate::initialize()
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
OrGate::executeProcess ()
{
  OrGateState *state = (OrGateState *) getState();
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
OrGate::allocateState()
{
  return new OrGateState();
}

void
OrGate::deallocateState( const State* state )
{
  delete state;
}

void OrGate::reclaimEvent( const Event *event )
{
  delete event;
}

void 
OrGate::reportError( const string& , SEVERITY )
{
  // Nothing is described here
}

void 
OrGate::finalize()
{}
