#ifndef DFF_H
#define DFF_H

#include <iostream>
#include <vector>
#include "NotGate.h"
#include "NInputNandGate.h"
#include "FileReaderWriter.h"
#include "DFFInitializer.h"

using namespace std;

class DFF {

public:
  /*--name public class methods of DFF--*/
  //@{
  /// constructor 
  DFF(string objName,int numInputs,int numOutputs,vector<string>* namesObj,
      vector<int> *outputPorts,int deLay);

  /// default deconstructor
  ~DFF(){}
  
  /// get 4 NANDS and 1 NOT simulation objects
  vector<SimulationObject*> * getDFFObjects();

  /// generates 4 NANDS and 1 NOT simulation objects and connects them to form a DFF
  void genDFFObj();
  //@} //end of public class methods of DFF

private:
   /*--name public attributes of DFF class--*/
   
   /// name of DFF
   string DFFname;
   /// the number of input ports of DFF
   int inputsNum;
   /// the number of gates the DFF connects to 
   int outputsNum;
   /// stores gates name of gates conneted to the DFF
   vector<string> *objNames;
   /// record the port ID for each gate connected to DFF 
   vector<int> *ports;
   /// delay value 
   int delay;
   /// sotres 4 NANDS and 1 NOT simulation objects 
   vector<SimulationObject*> *DFFObjects;
};

#endif 
