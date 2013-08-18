#include "DFF.h"

using namespace std;

DFF::DFF(string objName, int numInputs, int numOutputs, vector<string>* namesObj,
         vector<int> *outputPorts, int deLay):DFFname(objName),inputsNum(numInputs),
         outputsNum(numOutputs),objNames(namesObj),ports(outputPorts),delay(deLay)
        { DFFObjects = new vector<SimulationObject* >; }

vector<SimulationObject* >*
DFF::getDFFObjects(){ return DFFObjects;}

void 
DFF::genDFFObj(){

  string dffNand_1 = "NAND1";
  string dffNot    = "NOT";
  string dffNand_2 = "NAND2";
  string dffNand_3 = "NAND3";
  string dffNand_4 = "NAND4";
  
  dffNand_1 = DFFname + dffNand_1;
  dffNot    = DFFname + dffNot;
  dffNand_2 = DFFname + dffNand_2;
  dffNand_3 = DFFname + dffNand_3;
  dffNand_4 = DFFname + dffNand_4;

  vector<string>* nand1Out = new vector<string>;
  vector<string>* notOut   = new vector<string>;
  vector<string>* nand2Out = new vector<string>;
  vector<string>* nand3Out = new vector<string>;
  vector<string>* nand4Out = new vector<string>;

  vector<int>* nand1Des =new vector<int>;
  vector<int>* notDes   =new vector<int>;
  vector<int>* nand2Des =new vector<int>;
  vector<int>* nand3Des =new vector<int>;
  vector<int>* nand4Des =new vector<int>;

  nand1Out->push_back(dffNand_3);
  notOut  ->push_back(dffNand_2);
  nand2Out->push_back(dffNand_4);
  nand3Out =objNames;
  nand3Out->push_back(dffNand_4);
  outputsNum++;
  nand4Out->push_back(dffNand_3);
  
  nand1Des->push_back(1);
  notDes  ->push_back(1);
  nand2Des->push_back(1);
  nand3Des =ports;
  ports->push_back(2);
  nand4Des->push_back(2); 
  
  SimulationObject *nandgate1= new NInputNandGate(dffNand_1,2,1,nand1Out,nand1Des,1);
  SimulationObject *notgate  = new NotGate(dffNot,1,1,notOut,notDes,1);
  SimulationObject *nandgate2= new NInputNandGate(dffNand_2,2,1,nand2Out,nand2Des,1);
  SimulationObject *nandgate3= new NInputNandGate(dffNand_3,2,outputsNum,nand3Out,nand3Des,1);
  SimulationObject *nandgate4= new NInputNandGate(dffNand_4,2,1,nand4Out,nand4Des,1);
  
  DFFObjects->push_back(nandgate1);
  DFFObjects->push_back(notgate);
  DFFObjects->push_back(nandgate2);
  DFFObjects->push_back(nandgate3);
  DFFObjects->push_back(nandgate4);

  string dffInitial = "DFFInitial";
  dffInitial = DFFname + dffInitial;
  vector<string>* desGates  = new vector<string>;
  vector<int>* desPorts = new vector<int>;
  desGates->push_back(dffNand_4);
  desGates->push_back(dffNand_3);
  desPorts->push_back(2);
  desPorts->push_back(2); 
  SimulationObject *Initializer = new DFFInitializer(dffInitial,2,desGates,desPorts);

  DFFObjects->push_back(Initializer);
}
