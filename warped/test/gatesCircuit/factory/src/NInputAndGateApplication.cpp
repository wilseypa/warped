// Copyright (c) The University of Cincinnati.
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
// THE SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Manas Minglani              minglams@mail.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
//
// $Id: LogicEvent.cpp
//
//---------------------------------------------------------------------------

#include "../include/NInputAndGateApplication.h"
#include "../include/NInputAndGate.h"
#include "../src/NotGate.h"
#include "../src/OrGate.h"
#include "../include/LogicEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>
#include <utils/ArgumentParser.h>

#include "vector"
#include "iostream"
#include "fstream"
using namespace std;
using std::string;

NInputAndGateApplication::NInputAndGateApplication()
  : inputFileName( "" ),
    numObjects( 0 ){}

int
NInputAndGateApplication::initialize( vector<string> &arguments )
{
  getArgumentParser().checkArgs( arguments );
  
  if( inputFileName.empty() )
    {
      std::cerr << "A smmpSim configuration file must be specified using -simulate" << std::endl;
      abort();
    }
  
  return 0;
}

int 
NInputAndGateApplication::finalize()
{
  return 0;
}

int
NInputAndGateApplication::getNumberOfSimulationObjects(int mgrId) const 
{
  return numObjects;
}

const PartitionInfo *
NInputAndGateApplication::getPartitionInfo(unsigned int numberOfProcessorsAvailable)
{
  const PartitionInfo *retval = 0;
  Partitioner *myPartitioner = new RoundRobinPartitioner();
  string startGate;
  int totalNumOfGates;
  ifstream configfile;

  vector<string> outputObjectNames ;
  vector<int> *destinationPorts = new vector<int>;
  vector<int> *fanOutSize = new vector<int>(numberOfOutputs, 0);
  
  configfile.open(inputFileName.c_str());

  vector<SimulationObject *> *retObject = new vector<SimulationObject *>;

  if(configfile.fail())
    {
      cerr<<"Could not open file!:'"<<inputFileName<<"'"<<endl;
      cerr<<"Terminating simulation."<<endl;
      abort();
    }
  
  configfile >> totalNumOfGates;
  
  for ( int d = 0 ; d < totalNumOfGates ; d++ )
    {
      /*
	ObjectName , Number_of_Outputs , Number_of_Inputs , Object_Delay , 
	Output_Object_Names , Starting_Gate_or_Not 
      */
      
      configfile >> objName ;
      configfile >> sObjectId;
      configfile >> numberOfOutputs ; 
      configfile >> numberOfInputs ;
      configfile >> objDelay ;
  
      /* 
	 The object names of all the output gates will be recorded here and
	 fed into the vector "outputObjectNames".
       */

      if ( numberOfOutputs != 0 )
	{
	  for ( int k = 0 ; k < numberOfOutputs ; k++ )
	    {
	      configfile >> temp_output;
	      outputObjectNames.push_back(temp_output);
	    }
            
	}
      else if ( numberOfOutputs == 0 )
	{
	  configfile >> temp_output;
	}
       
      /*
	This variable records if the gate is the starting gate or not. 
	Starting gate simply refers to the initial gate which would require 
	bits to be read from a file or an event would be sent back to itself
	with initializing input bits. S - Start gate , NS - Not Start gate.
       */

      configfile >> startGate;

      if ( startGate == "S" )
	{
	  objNameTemp = objName + sObjectId;
	  outputObjectNames.push_back(objNameTemp);
	}
      else if ( startGate == "NS" )
	{
	  objNameTemp = objName + sObjectId;
	  /* Nothing needs to be pushed into the outputObjectNames vector */
	}
      // could be replaced with a case statement. 
      if ( objName == "AND" )
	{
	  retObject -> push_back ( new NInputAndGate(objNameTemp,
						     numberOfInputs,
						     numberOfOutputs,
						     outputObjectNames,
						     destinationPorts,
						     fanOutSize,
						     objDelay));
	  outputObjectNames.clear();
	}
      else if ( objName == "NOT" )
	{
	  retObject -> push_back ( new NotGate(objNameTemp,
					       numberOfInputs,
					       numberOfOutputs,
					       outputObjectNames,
					       destinationPorts,
					       fanOutSize,
					       objDelay));
	  outputObjectNames.clear();
	}
      else if ( objName == "OR" )
	{
	  retObject -> push_back ( new NotGate(objNameTemp,
					       numberOfInputs,
					       numberOfOutputs,
					       outputObjectNames,
					       destinationPorts,
					       fanOutSize,
					       objDelay));
	  outputObjectNames.clear();
	}
    }
  
  retval = myPartitioner->partition( retObject , numberOfProcessorsAvailable );
  
  delete retObject;
  
  return retval; 	
  /*
    NInputAndGate is derived from NInputGate and this is further derived from 
    LogicComponent. As we proceed further it could be evidently observed that
    Logic Component class is inherited from Simulation Object class.
    
    This makes it compulsory for Logic Component class to have following members 
    initialize() , finalize() , executeProcess() , allocateState() , deallocateState(), 
    reclaimEvent() , reportError() . Unless these members are not defined in a class 
    which is inherited from SimulationObject class the program will produce errors.
    Since NInputGate and NInputAndGate classes are not directly derived from 
    Simulation Object class therefore, they do not need to have the required functions.
    Logic Component class is the only class which needs explicit declaration and 
    definition of of these functions.     
   */

  // ostrstream arg;
  // arg<<objname<<numofoutputs<<outputportnum<<desname<<inputportnum<<numofinputs<<delay<<ends;
}

void 
NInputAndGateApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer(LogicEvent::getLogicEventDataType(),
                                                       &LogicEvent::deserialize);
}

ArgumentParser & 
NInputAndGateApplication::getArgumentParser(){
  static ArgumentParser::ArgRecord args[] = {
    {"-simulate", "input file name", &inputFileName, ArgumentParser::STRING,false},
    
    {"","",0 }
   
  };

  static ArgumentParser *myArgParser = new ArgumentParser(args);
  return *myArgParser;
}
