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
 
  DFF(){DFFObjects = new vector<SimulationObject *>;}

  ~DFF(){}

  
 void createDFF(){
    string fileD = "circuitsimulationmodels/iscas89/DFF/fileD";//the path of the file.
    string fileClk ="circuitsimulationmodels/iscas89/DFF/fileClk";
    string fileQ =  "circuitsimulationmodels/iscas89/DFF/fileQ";
    string fileQ_invert = "circuitsimulationmodels/iscas89/DFF/fileQ_invert";
     
    vector<int> *fileDPorts = new vector<int>; 
    fileDPorts->push_back(1);
    fileDPorts->push_back(1);
    vector<int> *fileClkPorts = new vector<int>;
    fileClkPorts->push_back(2);
    fileClkPorts->push_back(2);
    
    vector<string>* fileDGates = new vector<string>;
    fileDGates-> push_back("NAND1");
    fileDGates-> push_back("NOT1");
    vector<string>* fileClkGates = new vector<string>;
    fileClkGates-> push_back("NAND1");
    fileClkGates-> push_back("NAND2");

    vector<int>* fileQPorts = new vector<int>;
    fileQPorts->push_back(0);
    vector<string>* fileQGates = new vector<string>;
    fileQGates->push_back("null"); 

    vector<int>* fileQinvertPorts = new vector<int>;
    fileQinvertPorts->push_back(0);
    vector<string>* fileQinvertGates = new vector<string>;
    fileQinvertGates->push_back("null");
    
    string nand1 = "NAND1";
    string nand2 = "NAND2";
    string nand3 = "NAND3";
    string nand4 = "NAND4";
    string not1  = "NOT1";

    vector<string>* nand1Out =new vector<string>;
    nand1Out->push_back(nand3);
    vector<int>* nand1Des = new vector<int>;
    nand1Des->push_back(1);
    
    vector<string>* nand2Out =new vector<string>;
    nand2Out->push_back(nand4);
    vector<int>* nand2Des = new vector<int>;
    nand2Des->push_back(1);

    vector<string>* nand3Out =new vector<string>;
    nand3Out->push_back(fileQ);
    nand3Out->push_back(nand4);
    vector<int>* nand3Des = new vector<int>;
    nand3Des->push_back(0);
    nand3Des->push_back(2); 

    vector<string>* nand4Out =new vector<string>;
    nand4Out->push_back(fileQ_invert);
    nand4Out->push_back(nand3);
    vector<int>* nand4Des = new vector<int>;
    nand4Des->push_back(0);
    nand4Des->push_back(2);

    vector<string>* not1Out = new vector<string>;
    not1Out->push_back(nand2);
    vector<int>* not1Des = new vector<int>;
    not1Des->push_back(1);
  
    SimulationObject *D = new FileReaderWriter(fileD,2,"I",fileDPorts,fileDGates,10); //FileReaderWriter object 
    DFFObjects->push_back(D);
    
    SimulationObject *Clk = new FileReaderWriter(fileClk,2,"I",fileClkPorts,fileClkGates,10);//FileReaderWriter object
    DFFObjects->push_back(Clk);
    
    SimulationObject *Q = new FileReaderWriter(fileQ,0,"O",fileQPorts,fileQGates,10);//FileReaderWriter object
    DFFObjects->push_back(Q);
    
    SimulationObject *Qinvert = new FileReaderWriter(fileQ_invert,0,"O",fileQinvertPorts,fileQinvertGates,10);//FileReaderWriter object
    DFFObjects->push_back(Qinvert);
    
    SimulationObject *nandgate1 = new NInputNandGate(nand1,2,1,nand1Out,nand1Des,1);//DFF1_NAND1 gate object
    DFFObjects->push_back(nandgate1);
   
    SimulationObject *notgate = new NotGate(not1,1,1,not1Out,not1Des,1); //DFF1_NOT1  gate Object
    DFFObjects->push_back(notgate);
    SimulationObject *nandgate2 = new NInputNandGate(nand2,2,1,nand2Out,nand2Des,1);//DFF1_NAND2 gate object
    DFFObjects->push_back(nandgate2);
    SimulationObject *nandgate3 = new NInputNandGate(nand3,2,2,nand3Out,nand3Des,1);//DFF1_NAND3 gate object
    DFFObjects->push_back(nandgate3);
    SimulationObject *nandgate4 = new NInputNandGate(nand4,2,2,nand4Out,nand4Des,1); //DFF1_NAND4 gate object with feedback 
    DFFObjects->push_back(nandgate4);
     
    string objName = "DFFInitial";
    vector <string>* desGates = new vector<string>;
    desGates->push_back(nand4);
    desGates->push_back(nand3);
    vector <int> * desPorts = new vector<int>;
    desPorts-> push_back(2); 
    desPorts-> push_back(2);
    SimulationObject *Initializer = new DFFInitializer(objName,2,desGates,desPorts);
    DFFObjects->push_back(Initializer);
    cout<<"all gates are created !" <<endl; 
    
 }

 vector<SimulationObject*> * getDFFGates(){
   return DFFObjects;
 }

   private:

   vector<SimulationObject*> *DFFObjects;
};
#endif 
