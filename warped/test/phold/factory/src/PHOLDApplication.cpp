// See copyright notice in file Copyright in the root directory of this archive.

#include "../include/PHOLDApplication.h"
#include "../include/Process.h"
#include "../include/PHOLDEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>
#include <utils/ArgumentParser.h>

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
using std::string;

PHOLDApplication::PHOLDApplication( ) 
  : inputFileName( "" ),
    numObjects( 0 ){}

int 
PHOLDApplication::initialize( vector<string> &arguments ){ 
  getArgumentParser().checkArgs( arguments );

  if( inputFileName.empty() ){
    std::cerr << "A pholdSim configuration file must be specified using -simulate" << std::endl;
    abort();
  }

  return 0;
}

int 
PHOLDApplication::getNumberOfSimulationObjects(int mgrId) const { 
  return numObjects;
}

vector<SimulationObject *> *
PHOLDApplication::getSimulationObjects(){

  unsigned int msgDen;
  string distributionString; // distribution_t
  distribution_t dist;
  double seed;
  string genType;
  bool generateObjs = false;

  string name;
  unsigned int procNum;
  unsigned int stateSize;
  unsigned int grain;
  unsigned int numOutputs;
  string temp;

  ifstream configFile;
  configFile.open( inputFileName.c_str() );

  if(configFile.fail()){
    cerr << "Could not open file: '" << inputFileName << "'" << endl;
    cerr << "Terminating simulation." << endl;
    abort();
  }
  
  int hotspot;
  configFile >> hotspot >> numObjects >> msgDen >> distributionString >> seed
             >> genType;
  
  // configure object 0 to be a hotspot? (see ronngren paper 1994)
  if(hotspot != 1 && hotspot != 0) {
    cerr << "ERROR: First phold parameter must be 1 for hotspot node or 0 "
         << "for no hotspot node.\nAborting simulation." << endl;
    abort();
  }

  vector<SimulationObject *> *retval = new vector<SimulationObject *>;

  // Convert the distributionString to the proper distribution.
  if (distributionString == "UNIFORM") {
    dist = UNIFORM;
  } 
  else if (distributionString == "POISSON") {
    dist = POISSON;
  }
  else if (distributionString == "EXPONENTIAL") {
    dist = EXPONENTIAL;
  }
  else if (distributionString == "NORMAL") {
    dist = NORMAL;
  }
  else if (distributionString == "BINOMIAL") {
    dist = BINOMIAL;
  }
  else if (distributionString == "FIXED") {
    dist = FIXED;
  }
  else{
    cerr << "ERROR: Improper distribution type entered. Possibilities are: \nUNIFORM"
         << "\nPOISSON\nEXPONENTIAL\nNORMAL\nBINOMIAL\nFIXED\n"
         << "Aborting simulation.\n";
    abort();
  }

  if(genType == "GENERATE"){
    generateObjs = true;
  }
  else if(genType == "LIST"){
    generateObjs = false;
  }
  else{
    cerr << "ERROR: Generation type must be GENERATE or LIST.\nAborting Simulation.\n";
    abort();
  }

  utils::debug << "\nNumber of Objects: " << numObjects << " Message Density: " 
                 << msgDen << " Distribution: " << distributionString << " Seed: " << seed << endl;

  // When generating all the objects, all information about the objects will be the same.
  if(generateObjs){
    configFile >> stateSize >> grain >> numOutputs;
    
    if(numOutputs >= numObjects){
      cerr << "ERROR: The number of outputs per object must be less than the "
           << "total number of objects.\nAborting Simulation.\n";
      abort();
    }

    // Generate a list of the object names.
    vector<string> objNames;
    string prefix = "PHOLDObject.";
    for( int n = 1; n <= numObjects; n++ ){
      stringstream out;
      out << n;
      string curObj = out.str();
      objNames.push_back(prefix + curObj);
    }

    for( int i = 0; i < numObjects; i++){

      // The outputs will be the next 'numOutputs' names that follow the current
      // object name. This ensures that all objects have the same number of outputs.
      int hotspotnum = 0;
      vector<string> outputNames;
      for( int j = 0; j < numOutputs; j++){
        int objnum = (j + i + 1) % numObjects;
        if(objnum == 0)
          hotspotnum = j;
         outputNames.push_back(objNames[(j+i+1)%numObjects]);
      }

      retval->push_back( new Process( i, objNames[i], numOutputs, outputNames,
                                      stateSize, msgDen, dist, grain, seed, hotspot, hotspotnum ) );
    }
  }
  else{
    if(hotspot)
      cout << "Warning; to use hotspot node you must use generated objects."
           << endl << "Ignoring hotspot." << endl;

    for( int i = 0; i < numObjects; i++){
     
       configFile >> name >> procNum >> stateSize >> grain >> numOutputs;
 
       vector<string> outputNames;
       for( int j = 0; j < numOutputs; j++){
          configFile >> temp;
          outputNames.push_back(temp);
       }

       retval->push_back( new Process( procNum, name, numOutputs, outputNames,
                                       stateSize, msgDen, dist, grain, seed ) ); 
    }
  }
  configFile.close();
  
  return retval;
}

const PartitionInfo *
PHOLDApplication::getPartitionInfo( unsigned int numberOfProcessorsAvailable ){
  const PartitionInfo *retval = 0;

  Partitioner *myPartitioner = new RoundRobinPartitioner();
  // Now we'll create some simulation objects...
  vector<SimulationObject *> *objects = getSimulationObjects();
  retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
  delete objects;

  return retval;
}

int 
PHOLDApplication::finalize(){ 
  return 0; 
}

void 
PHOLDApplication::registerDeserializers(){
  DeserializerManager::instance()->registerDeserializer( PHOLDEvent::getPHOLDEventDataType(),
							 &PHOLDEvent::deserialize );
}

ArgumentParser &
PHOLDApplication::getArgumentParser(){
  static ArgumentParser::ArgRecord args[] = {
    { "-simulate", "input file name", &inputFileName, ArgumentParser::STRING, false },

    { "", "", 0 }
  };

  static ArgumentParser *myArgParser = new ArgumentParser( args );
  return *myArgParser;
}
