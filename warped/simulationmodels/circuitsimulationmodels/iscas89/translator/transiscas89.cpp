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

// Authors : Xinyu Guo                    guox2@mail.uc.edu
//           Philip A. Wilsey             phil.wilsey@uc.edu  

//---------------------------------------------------------------------
//
// $Id: transiscas89.cpp 
// this program can generate the configuration files of ISCAS89 circuits
// simulation models: s526,s5378 and s9234. 
//
//--------------------------------------------------------------------- 

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "transiscas89.h"

using namespace std;

int main (int argc, char* argv[]){
  char oneLine[LINE_LENGTH];  
  ifstream iscasBenchMark;
  ofstream simModelConfig;
  int componentNum [2] = {0,0}; 
  map<int,string> numnameMap; 
  map<string,int> gateInputsNum; 

  iscasBenchMark.open(argv[1]);
  simModelConfig.open(argv[2]);
  if(!iscasBenchMark.is_open()){
    cout <<"Fail to open the input file !" <<endl; 
    exit (-1);
  }
  if(!simModelConfig.is_open()){
    cout<<"Fail to open the output file !"<<endl;	 
    exit(-1);
  }

  vector <CircuitNode* > *nodesVec = new vector<CircuitNode* >;
  vector <int> *numOfInputsVec = new vector<int>;//record the input number of each object. 
	                                         //gates, input and output files are objects.
	                                         //each input file has 0 input.
  
  while(iscasBenchMark.getline(oneLine,LINE_LENGTH)){ //for each line in the isacs85 benchmark
    cout<<"deal new line!"<<endl;
    if(!isUsefull(oneLine))
      continue;
    
    stripTokens(oneLine);
    string NodeName;   
    string NodeNum;
    istringstream thisLine(oneLine,istringstream::in);
    thisLine>>NodeName;
    thisLine>>NodeNum; 
     
    if("INPUT"==NodeName || "OUTPUT"==NodeName){
      addNodeVec(nodesVec,NodeName,NodeNum,numnameMap);
      componentNum[FILES_NUM]++;
    } 
    else{
      int identity;
      identity = produceID(NodeName);
      if(numnameMap.find(identity)==numnameMap.end()){
          addNodeVec(nodesVec,NodeName,NodeNum,numnameMap);
          componentNum[GATES_NUM]++;
      }
    }

    string inputGateNum;
    int portId = 0;
			
    while(thisLine>>inputGateNum){//attach the information of the current gate to its input gates
      int id = produceID(inputGateNum);
      int ID = produceID(NodeName);

      if(numnameMap.find(id)!=numnameMap.end()){// this node is in the vector
        attachNode(nodesVec,NodeName,numnameMap.find(ID)->second,inputGateNum,portId);  
     } 
      else{ // this node is not in the vector
        int filePos = iscasBenchMark.tellg();
        string gateType = searchBenchFile(inputGateNum,iscasBenchMark);
        iscasBenchMark.seekg(filePos);    
        addNodeVec(nodesVec,inputGateNum,gateType,numnameMap);
        componentNum[GATES_NUM]++; 
        attachNode(nodesVec,NodeName,numnameMap.find(ID)->second,inputGateNum,portId);
      }   
      portId++; 
    }
      gateInputsNum[NodeName]=portId;
  }

  showContent(nodesVec);//show the content in the vector

  conFileGen(nodesVec,simModelConfig,componentNum,gateInputsNum);// genertate configuration file
   
  iscasBenchMark.close();
  simModelConfig.close();
} 
