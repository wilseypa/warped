#include "Iscas85Application.h"
#include "LogicEvent.h"
#include "FileReaderWriter.h"
#include "NInputAndGate.h"
#include "NInputXorGate.h"
#include "NInputOrGate.h"
#include "NInputNandGate.h"
#include "NInputNorGate.h"
#include "NotGate.h"
#include "PartitionInfo.h"
#include "RoundRobinPartitioner.h"
#include "DeserializerManager.h"

#include <vector>
#include <iostream>
#include <fstream>
#include "stdlib.h"

using namespace std;


Iscas85Application::Iscas85Application(string inputFileName, string testCaseFileName,
                                       int numObjects)
    : inputFileName(inputFileName),
      testCaseFileName(testCaseFileName),
      numObjects(numObjects) {}

int 
Iscas85Application::finalize(){
  return 0;
}

int
Iscas85Application::getNumberOfSimulationObjects(int mgrId) const {
  return numObjects;
}

const PartitionInfo*
Iscas85Application::getPartitionInfo(unsigned int numberOfProcessorsAvailable){
  const PartitionInfo *retval = 0;
  Partitioner *myPartitioner = new RoundRobinPartitioner();

  int numFileObjects;
  string fileName;
  int numOfGates;  
  string type;
  int desPort;  
  string desGateName;
  int maxLines;
  
  ifstream configfile;

  configfile.open(inputFileName.c_str());

  if(configfile.fail()){
    cerr<<"Could not open file!:'"<<inputFileName<<"'"<<endl;
    cerr<<"Terminating simulation."<<endl;
    abort();
  }

  vector<SimulationObject *> *objects = new vector<SimulationObject *>;

  string filePath;//
  int totalGates; // total gates number in the circuit
  configfile>>filePath;//
  configfile>>numFileObjects;
  configfile>>totalGates;
	

  for(int i=0; i < numFileObjects; i++){
    configfile>>fileName>>numOfGates>>type;	
    vector<int> *desPortId = new vector<int>;
    vector<string> *desGatesNames = new vector<string>;
		 
    if(0==numOfGates){
      configfile>>desPort;
      //cout<<"numOfGates is:"<<numOfGates<<endl;
      //cout<<"desPort Value is:"<<desPort<<endl;
      desPortId->push_back(desPort);
      configfile>>desGateName;
      //cout<<"numOfGates is:"<<numOfGates<<endl;
      //cout<<"desPort Value is:"<<desPort<<endl;
      desGatesNames->push_back(desGateName);
    }
  else{
    for(int j= 0;j<numOfGates;j++){
    configfile>>desPort;
    //cout<<"numOfGates is:"<<numOfGates<<endl;
    //cout<<"desPort Value is:"<<desPort<<endl;
    desPortId->push_back(desPort);
   }
    for(int j= 0;j<numOfGates;j++){
      configfile>>desGateName;
      desGatesNames->push_back(desGateName);
    }
  }
    configfile>>maxLines;
    fileName = filePath+fileName;//the absolute path of the file 
    SimulationObject *newObject = new FileReaderWriter(fileName,numOfGates,type,desPortId,desGatesNames,maxLines); 
    objects->push_back(newObject);
    //cout<<"construct FileReaderWriter object!"<<endl;
  }
	 
   // int totalGates; // total gates number in the circuit 

    string myObjName;    // object name
    int numberOfInputs;  // the number of inputs of the object
    int numberOfOutputs; // the number of objects connected to the object
    string destObjName;  // the name of the gate which receives the object output
    int desInputPort;    // pin id of the gate which receives the object output
    int delay;           // the delay between event receiving and event sending 

  // configfile >> totalGates;
	 
    for(int i = 0; i < totalGates; i++){
      configfile >> myObjName >> numberOfInputs >> numberOfOutputs;// >> destObjName >> desInputPort; 
      vector<string> *outputObjectNames = new vector<string>;
      vector<int> *destinationPorts = new vector<int>;

      for(int i = 0; i < numberOfOutputs; i++){
        configfile >> destObjName;
        //cout<<"object name is "<< destObjName<<endl;
        outputObjectNames -> push_back(destObjName);	
      } 
									
      for(int i =0; i < numberOfOutputs; i++){
        configfile >> desInputPort;
        destinationPorts->push_back(desInputPort);
      }

      configfile >> delay;
      string gate= myObjName.substr(0,1);
      string gate1 = myObjName.substr(0,3);
 
      if("A"==gate){
        SimulationObject *newObject = new NInputAndGate(myObjName,
                                                        numberOfInputs,
                                                        numberOfOutputs,
                                                        outputObjectNames,
                                                        destinationPorts,
                                                        delay);

        objects->push_back(newObject);
        // cout << "building object with R !"<<endl;
        // cout << "the address of the object is " << newObject<<endl;
      } 
      else if ("X"==gate){
        SimulationObject *newObject = new NInputXorGate(myObjName,
                                                        numberOfInputs,
                                                        numberOfOutputs,
                                                        outputObjectNames,
                                                        destinationPorts,
                                                        delay);

        objects->push_back(newObject);
        // cout << "building object with R !"<<endl;
        // cout << "the address of the object is " << newObject<<endl;
      }
      else if ("O"==gate){
        SimulationObject *newObject = new NInputOrGate(myObjName,
                                                       numberOfInputs,
                                                       numberOfOutputs,
                                                       outputObjectNames,
                                                       destinationPorts,
                                                       delay);
        objects->push_back(newObject);
        // cout << "building object with R !"<<endl;
        // cout << "the address of the object is " << newObject<<endl;
      }
      else {
        if("NOT"==gate1){
          SimulationObject *newObject = new NotGate(myObjName,
                                                    numberOfInputs,numberOfOutputs,outputObjectNames,
                                                    destinationPorts,delay);
          objects->push_back(newObject);
        }
        if("NAN"==gate1){
          SimulationObject *newObject = new NotGate(myObjName,
                                                     numberOfInputs,numberOfOutputs,outputObjectNames,
                                                     destinationPorts,delay);
	  objects->push_back(newObject);
	}

       if("NOR"==gate1){
         SimulationObject *newObject = new NotGate(myObjName,
                                                   numberOfInputs,numberOfOutputs,outputObjectNames,
                                                   destinationPorts,delay);	
         objects->push_back(newObject);
        }
     }
   }
     // cout<<"objects size is"<<objects->size()<<endl;
      retval = myPartitioner->partition( objects, numberOfProcessorsAvailable );
      delete objects;
      return retval;
}

void
Iscas85Application::registerDeserializers(){
	DeserializerManager::instance()->registerDeserializer(LogicEvent::getLogicEventDataType(),&LogicEvent::deserialize);
}
