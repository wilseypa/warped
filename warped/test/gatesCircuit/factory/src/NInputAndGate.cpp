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

//---------------------------------------------------------------------------
// 
// $Id: NInputAndGate.cpp
// 
//---------------------------------------------------------------------------

#include "../include/NInputAndGate.h"
#include "../include/NInputGateState.h"
#include "../include/LogicEvent.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

// outputs has been changed from a vector of pointers 
// to a vectorof string type variables. if reverting back to it please
//put a star in front of outputs. 

NInputAndGate::NInputAndGate(string &objectName, 
			     const int numInputs,
                             const int numOutputs,
                             vector<string> outputs,
                             vector<int> *destInputPorts,
                             vector<int> *fanOutSize,
                             int objectDelay)
{
  myObjectName    = objectName;
  numberInputs    = numInputs;
  numObjects      = numOutputs; 
  outputs_send    = outputs; 
  viOutputPorts   = destInputPorts;
  viFanOut        = fanOutSize;
  gateObjectDelay = objectDelay;
}

void 
NInputAndGate::initialize()
{
  int i = 0; // the index
  x = 1; // 
  /* I will be incorporating file reader here */
  /* from file input bit stream would be read and the computation would start */

  if (numObjects != 0 ) // store handles of receivers
    {
      outputHandles.resize( outputs_send.size(),NULL );
      for ( i = 0; i < outputHandles.size(); i++ )
	{
	  outputHandles[i] = getObjectHandle ( outputs_send[i] );
	}
    }

  cout << " my object name is : " << myObjectName << endl ;
  
  if ( outputs_send.back() == myObjectName )
    {
      NInputGateState *newState = (NInputGateState *) getState();
      
      //cout << "my object name is : " <<  myObjectName << endl;
      //cout<<"numberOfInputs is:"<<numberInputs<<endl;
      
      newState->outputBit1 = 0;
      
      IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
      LogicEvent *firstEvent = new LogicEvent(sendTime,
					      sendTime + 1,
					      this,
					      outputHandles.back());
      
      firstEvent -> setObjectName(myObjectName);
      firstEvent -> setbitValue1(1);
      firstEvent -> setbitValue2(1);
      firstEvent -> setsourcePort(0);
      firstEvent -> setdestinationPort(2);
      
      /**
	 this->receiveEvent(firstEvent);
      */
      
      (outputHandles.back()) -> receiveEvent(firstEvent);
    }
}

void 
NInputAndGate::executeProcess ()
{
  NInputGateState *state = (NInputGateState *) getState();
  LogicEvent *logicEvent  = NULL;
  int result = 1;
  int x = 0;
  iRecvEvent = 0;
  sTempObjectName = myObjectName ;
  
  if ( outputs_send.back() == myObjectName )
    {
      while(true == haveMoreEvents())
	{
	  logicEvent = (LogicEvent *) getEvent();
	  if(logicEvent !=NULL)
	    {
	      state->inputBits[logicEvent->destinationPort - 1] = logicEvent->bitValue2;
	      cout << logicEvent->destinationPort-1 << ": got bit value " << logicEvent->bitValue2 << endl;
	      
	      state->inputBits[logicEvent->destinationPort - 2] = logicEvent->bitValue1;
	      cout << logicEvent->destinationPort-2 << ": got bit value " << logicEvent->bitValue1 << endl;
	    }
	}
    }
  else if ( outputs_send.back() != myObjectName )
    {
      /* 
	 the condition here might have to be changed 
	 i may have to do while ( x != 2 )
	 { while (true == haveMoreEvents() )}
	 it will keep on checking for events till it has received two
	 only then it will fill up two bits in the inputsbits array 
	 and then compute the final result and send it to the output  
	 but for the time being it is perfectly fine to use it this way. 
      */
      
      while ( iRecvEvent != 2 )
	{
	  while(true == haveMoreEvents())
	    {
	      logicEvent = (LogicEvent *) getEvent();
	      if(logicEvent !=NULL)
		{
		  iRecvEvent = iRecvEvent + 1;
		  
		  sRecvObjectName = logicEvent -> sObjectName;
		  if ( sRecvObjectName != sTempObjectName )
		    {
		      sTempObjectName = sRecvObjectName;
		      
		      if ( iRecvEvent == 1 )
			{
			  state->inputBits[logicEvent->destinationPort - 1] = logicEvent -> bitValue2;
			  cout << logicEvent->destinationPort - 1 << ": got bit value " << logicEvent -> bitValue2 << endl;
			}
		      else if ( iRecvEvent == 2 )
			{
			  state->inputBits[logicEvent -> destinationPort - 2] = logicEvent -> bitValue2;
			  cout << logicEvent -> destinationPort - 2 << ": got bit value " << logicEvent -> bitValue2 << endl;
			}
		    }
		}
	    }
	}
    }
  
  for(int i = 0; i < numberInputs; i++)
    {
      result = (result and (state -> inputBits[i]));
    }
  cout << "result from " << myObjectName << ": "<< result << endl; 
  
  /* the below given codes send the result of this and gate to the output gate . */
  if ( numObjects != 0 )
    {
      NInputGateState *state = (NInputGateState *) getState();
	    
      state -> outputBit1 = 0;
      
      /** 
	  the below given section is just to generate a bit stream and feed it 
	  back to the originating gate. This originating gate will process the input
	  bits and send an output bit stream 
      */
	    
      IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
      	    
      outputHandles.resize(outputs_send.size(),NULL);
      outputHandles[0] = getObjectHandle(outputs_send[0]);
	    
      LogicEvent *firstEvent = new LogicEvent(sendTime,
					      sendTime + 1,
					      this,
					      outputHandles[0]);

      firstEvent -> setObjectName(myObjectName);	    
      firstEvent -> setbitValue1(0);
      firstEvent -> setbitValue2(result);
      firstEvent -> setsourcePort(0);
      firstEvent -> setdestinationPort(2);
	    
      (outputHandles[0]) -> receiveEvent(firstEvent);
    }
}

State*
NInputAndGate::allocateState()
{
  return new NInputGateState();
}

void
NInputAndGate::deallocateState( const State* state )
{
  delete state;
}

void 
NInputAndGate::reclaimEvent( const Event *event )
{
  delete event;
}

void 
NInputAndGate::reportError( const string& , SEVERITY )
{
  // Nothing is described here
}

void 
NInputAndGate::finalize()
{}

/**
  while(true == haveMoreEvents())
  {
  logicEvent = (LogicEvent *) getEvent();
  if(logicEvent !=NULL)
  {
  if (x == 0)
  {
  state->inputBits[logicEvent->destinationPort - 1] = logicEvent->bitValue2;
  cout << logicEvent->destinationPort-1 << ": got bit value " << logicEvent->bitValue2 << endl;
  x = 1;
  }
  else if (x == 1) 
  {
  state->inputBits[logicEvent->destinationPort - 2] = logicEvent->bitValue2;
  cout << logicEvent->destinationPort-2 << ": got bit value " << logicEvent->bitValue1 << endl;
  }
  }
*/
