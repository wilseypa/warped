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

//-----------------------------------------------------------------------
//
// $Id: transiscas89.h
//
//-----------------------------------------------------------------------

#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include <map>

#define FILES_NUM 0
#define GATES_NUM 1
#define LINE_LENGTH 256
#define LINE_NUMBER 8
#define OBJECT_DELAY 1
#define FILE_PATH  "circuitsimulationmodels/iscas89/iscas89Sim/s5378/"

using namespace std;

/*--the struct for storing the circuit node information--*/ 
class CircuitNode{
  public:
    string nodeNum; 
    string nodeName;
    int inputPortId;
    CircuitNode *nextNode;
};

/*--generate the ID for the component (gate or DFF)--*/
int extractNodeId(string nName){
  string tempstr = nName;
  if("II"==tempstr.substr(0,2))
    tempstr.erase(0,2);
  else // begin with "G", "I" , "g" or "n"
    tempstr.erase(0,1);
  stringstream sstream;
  int nodeId;
  sstream<<tempstr;
  sstream>>nodeId;
  return nodeId;
}

/*--produce node id. e.g. the id of n846gat should be 200846--*/
int produceID(string node){
  int nid = extractNodeId(node);
  string temp = node.substr(0,1);
  if("G" == temp)
    nid = nid + 100000;
  else if("n" == temp)
    nid = nid + 200000;
  else if("g" == temp)
    nid = nid + 300000;
  else{ // begin with "II"
    if("II"==node.substr(0,2))
      nid = nid + 400000;
    else
      nid = nid + 500000;
  }
  return nid;
}

/*--get rid of comments and empty lines--*/
bool isUsefull(char(&singleLine)[LINE_LENGTH]){
  bool usefull = true;
  if('#' == singleLine[0] || '\0'==singleLine[0])
    usefull = false;
  return usefull;
}

/*--get rid of '(',')','=',','from the bench file line--*/
void stripTokens(char(&singleLine)[LINE_LENGTH]){
  int lineLength = strlen(singleLine); 
  for(int i =0; i<lineLength;i++){
    if (singleLine[i] =='(')
      singleLine[i]=' ';
    if (singleLine[i] ==')')
        singleLine[i]=' ';
    if (singleLine[i] =='=')
      singleLine[i]=' ';
    if (singleLine[i] == ',')
      singleLine[i]=' ';
    }
}

/*--produce a name for a node--*/
string genNodeName(int id, string type){
  string thisNodeName;
  stringstream out;
  out<<id;
  string curNum = out.str();
  thisNodeName = type + curNum;
  return thisNodeName; 
}

/*--detect the component type by seraching its ID in the bench file--*/
/*--eg.in s536 bench, the gate with ID G60, its component type is NOR--*/
string searchBenchFile(string ID, ifstream& file){
  char benchLine[LINE_LENGTH];
  string nodeID;
  string nodeType = "noType";
  while(file.getline(benchLine,LINE_LENGTH)){
    if(!isUsefull(benchLine)) 
      continue;
    stripTokens(benchLine);
    istringstream curLine(benchLine,istringstream::in);
    curLine>>nodeID;
    if(nodeID == ID){
      curLine>>nodeType;
      return nodeType;
    }
  }   
}	

/*--push new node to the nodes vector--*/
void addNodeVec(vector<CircuitNode*> *nodes,string thisNodeName,string thisNodeNum,map<int,string> &numNameMap){
  CircuitNode* currentNode = new CircuitNode();
  int nId;
  if(thisNodeName == "INPUT" || thisNodeName == "OUTPUT"){ //construct the node with the type INPUT
    currentNode->nodeName =thisNodeName;
    currentNode->nodeNum = thisNodeNum;
    currentNode->inputPortId=0;
    currentNode->nextNode = NULL;
    nodes->push_back(currentNode);
    if(thisNodeName == "INPUT"){
      nId = produceID(thisNodeNum);
      numNameMap[nId] = thisNodeNum;
    }
  }
  else{ // construct other gates 
    currentNode->nodeNum = thisNodeName;
    nId = extractNodeId(thisNodeName);
    string gateName = genNodeName(nId,thisNodeNum);
    string temp = thisNodeName.substr(0,1);
    if("G"==temp){
      nId = produceID(thisNodeName);
      gateName = gateName + "G";
      numNameMap[nId] = gateName;
      
    }
    else if ("n" == temp){
      nId = produceID(thisNodeName);
      gateName = gateName + "n";
      numNameMap[nId] = gateName;
    }
    else if ("g" == temp){
      nId = produceID(thisNodeName);
      gateName = gateName + "g";
      numNameMap[nId] = gateName;
    }
    else {// "I" == temp 
      if("II"==thisNodeName.substr(0,2)){
        nId = produceID(thisNodeName);
        gateName = gateName + "II";
        numNameMap[nId] = gateName;
      }
      else{
        nId = produceID(thisNodeName);
        gateName = gateName + "I";
        numNameMap[nId] = gateName;
      }
    }

    currentNode->nodeName = gateName;
    currentNode->inputPortId = 0;//this Id should always be 0 when the node is at the beginning at
                                 //the linklist
    currentNode->nextNode = NULL;
    nodes->push_back(currentNode);

  }
}

/*--attach the node to the end of a certain linked list--*/
void attachNode(vector<CircuitNode*> *nodes,string thisNodeNum, string thisNodeName,string attachTo, int portid){
  int size = nodes->size();
  CircuitNode* inputNode = new CircuitNode();
  inputNode->nodeNum =thisNodeNum;
  inputNode->nodeName =thisNodeName;
  for(int n = size-1; n >= 0 ; n--){
    if((*nodes)[n]->nodeNum == attachTo){
      CircuitNode* nextNodePt = (*nodes)[n]->nextNode;
      CircuitNode* currentPt =nextNodePt;
      if(nextNodePt==NULL){
        (*nodes)[n]->nextNode = inputNode;
        inputNode->inputPortId = portid+1;
        inputNode->nextNode = NULL;
      }
      else{
        while(nextNodePt != NULL){//go to the end of the linklist which begines with node[n] 
          currentPt = nextNodePt;
          nextNodePt = nextNodePt->nextNode;
        }
        currentPt->nextNode=inputNode;
        inputNode->inputPortId =portid+1;
        inputNode->nextNode=NULL;
      }
    }
  }
}

/*--given the name of a gate, judge whether it's DFF or not--*/
bool isDFF(string gateName){
  bool is = false;
  string temp = gateName;
  string DFF = temp.substr(0,3);
  if("DFF"==DFF){
    is = true;
  }
  return is;
}

/*--generate one configuration line for ISCAS89 circuit simulation model--*/
void genConfigLine(int position,vector<CircuitNode*> *nodesVec,ofstream& outputStream,
                   map<string,int>& gatesInputs,string path){ 
  vector<int> inPortId;
  vector<string> gatesName;
  CircuitNode* first = (*nodesVec)[position];
  string ioType = first->nodeName;

  if(ioType!="OUTPUT"){
    int listLength = 0;
    CircuitNode* next = first->nextNode;
    CircuitNode* current = new CircuitNode();
    while(next!=NULL){
      listLength++;
      current=next;
      gatesName.push_back(current->nodeName);
      inPortId.push_back(current->inputPortId);
      next=next->nextNode;
      //cout<<"list length is :"<<listLength<<endl;
    }
    if("INPUT"==ioType){ // generate one line for the configuration file, the information is for the input file. 
      outputStream<<first->nodeNum<<" ";
      outputStream<<listLength<<" "<<"I"<<" ";
      for(int i = 0; i < listLength;i++){
        outputStream<<inPortId[i]<<" ";}
      for(int i = 0; i < listLength;i++){
        if(isDFF(gatesName[i])){
          gatesName[i] = gatesName[i]+"NAND1";
        }
        outputStream<<gatesName[i]<<" ";}
        outputStream<<LINE_NUMBER<<endl;
      }
     else{
       bool isToFile=false;
       for(int i = position-1;i >= 0;i--){
         if(first->nodeNum==(*nodesVec)[i]->nodeNum){// find an output gate 
           isToFile = true;
           outputStream<<first->nodeName<<" ";
           outputStream<<gatesInputs.find(first->nodeNum)->second<<" ";//outputStream<<(*numInputs)[lineId]<<" ";
           outputStream<<"1"<<" ";
           outputStream<<path+(*nodesVec)[i]->nodeNum<<" "; //output file name
           outputStream<<"0"<<" "<<OBJECT_DELAY<<endl;}
         }
         if(isToFile==false){
           outputStream<<first->nodeName<<" "<<gatesInputs.find(first->nodeNum)->second<<" "<<listLength<<" ";
           for(int i = 0; i < listLength;i++){
             if(isDFF(gatesName[i])){
               gatesName[i]=gatesName[i]+"NAND1";
             }
             outputStream<<gatesName[i]<<" ";}
           for(int i = 0; i < listLength;i++){
             outputStream<<inPortId[i]<<" ";}
             outputStream<<OBJECT_DELAY<<endl;}}
  }
  else{
    outputStream<<first->nodeNum<<" "<<0<<" "<<"O"<<" ";
    outputStream<<0<<" "<<"null"<<" "<<LINE_NUMBER<<endl;}
}

/*--generate the simulation configuration file for a ISCAS89 circuit --*/
void conFileGen(vector<CircuitNode*> *nodes,ofstream& conFile,int (&gNum) [2],
                map<string,int>& gateinputNumber){
  string PATH = FILE_PATH;
  conFile<<PATH<<endl;
  conFile<<gNum[FILES_NUM]<<endl;
  conFile<<gNum[GATES_NUM]<<endl;
  string filePath = FILE_PATH;
  int size =nodes->size();
  for (int i = 0; i < size; i++){
    genConfigLine(i, nodes,conFile,gateinputNumber,filePath );
  }
}

/*--for the debugging purpose : display the content in linked lists --*/
void showContent(vector<CircuitNode*> *nodes){
  int size = nodes->size();
  for(int i = 0; i < size; i++){
    cout<<(*nodes)[i]->nodeNum<<" ";
    CircuitNode* nextGate = (*nodes)[i]->nextNode;
    CircuitNode* currentGate = new CircuitNode();
    while(nextGate != NULL){
      currentGate = nextGate;
      cout<< currentGate->nodeNum<<" ";
      cout<< currentGate->nodeName<<" ";
      cout<< currentGate->inputPortId<<" ";
      nextGate = nextGate->nextNode;
    }
    cout<<endl;
  } 
}
