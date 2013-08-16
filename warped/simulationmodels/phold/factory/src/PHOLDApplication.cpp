// See copyright notice in file Copyright in the root directory of this archive.

#include "../include/PHOLDApplication.h"
#include "../include/Process.h"
#include "../include/PHOLDEvent.h"
#include <warped/PartitionInfo.h>
#include <warped/RoundRobinPartitioner.h>
#include <warped/DeserializerManager.h>

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
using std::string;

PHOLDApplication::PHOLDApplication(string inputFileName, int numObjects)
    : inputFileName(inputFileName),
      numObjects(numObjects) {}

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
  
 /* ------------------------------- HOT SPOTS ------------------------------- *
  *                                                                           *
  * a hotspot is a SimulationObject that has a probability                    *
  * of receiving a message that is N times greater than the other objects.    *
  * it is a way of imposing load imbalance in a way that might occur in a     *
  * realistic model such as a digital logic simulator (see ronngren 1994)     *
  * the first PHOLD configuration parameter is N.  Set to 1 to not use        *
  * hotspots                                                                  *
  *                                                                           *
  * If N > 1, a set of groups of objects to use as hotspots at various points *
  * during the simulation is required.  The next parameter is the number of   *
  * groups M.  After that, the following data points are read in M times,     *
  * one for each group of objects: virtual time, number of objects in the     *
  * set, and the list of objects (by process ID)                              *
  *                                                                           *
  * Example:                                                                  *
  *                                                                           *
  * 5                                                                         *
  * 2                                                                         *
  * 0 2 0 1                                                                   *
  * 500 3 2 3 4                                                               *
  *                                                                           *
  * N is 5.  There will be 2 groups of hotspots.  At virtual time 0,          *
  * processes 0 and 1 will become hotspots.  At virtual time 500, processes   *
  * 2 and 3 and 4 will become hotspots.                                       *
  *                                                                           *
  * ------------------------------------------------------------------------- */
  vector<int> hotspotSwitchTimes(0);
  vector<vector<int> > hotspots;
  int hotspotProb;
  configFile >> hotspotProb;
  if (hotspotProb > 1) {
    int numHotspotSwitches;
    configFile >> numHotspotSwitches;
    hotspots.resize(numHotspotSwitches);
    for(int i = 0; i < numHotspotSwitches; i++) {
      int next, n;
      configFile >> next;
      hotspotSwitchTimes.push_back(next);
      configFile >> n;
      for(int j = 0; j < n; j++) {
        int obj;
        configFile >> obj;
        hotspots[i].push_back(obj);
      }
    }
  }

  configFile >> numObjects >> msgDen >> distributionString >> seed
             >> genType;

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
    cerr << "ERROR: Improper distribution type \"" << distributionString 
         << "\" entered. Possibilities are: \nUNIFORM"
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
    configFile >> stateSize >> grain;

    // if numOutputs isn't given, assume a fully connected network
    if(!(configFile >> numOutputs))
        numOutputs = numObjects - 1;
    
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
      vector<string> outputNames;
      for( int j = 0; j < numOutputs; j++){
        int objnum = (j + i + 1) % numObjects;
        outputNames.push_back(objNames[objnum]);
      } 

      retval->push_back( new Process( i, objNames[i], numOutputs, outputNames,
                                      stateSize, msgDen, dist, grain, seed,
                                      hotspotProb, &hotspotSwitchTimes, &hotspots ) );
    }
  }
  else{
    if(hotspotProb > 1)
      cout << "Warning; to use hotspot node you must use generated objects."
           << endl << "Using hotspot probability multiplier of 1." << endl;

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
  numLPs = numberOfProcessorsAvailable;

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