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

#define AND_NUM   0
#define OR_NUM    1
#define NAND_NUM  2
#define XOR_NUM   3
#define NOT_NUM   4
#define NOR_NUM   5
#define DFF_NUM   6
#define FILES_NUM 7
#define GATES_NUM 8
#define LINE_LENGTH 256
#define LINE_NUMBER 8
#define OBJECT_DELAY 1

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
  if("G"==tempstr.substr(0,1)) 
    tempstr.erase(0,1);
  else 
    tempstr.erase(0,2);
  stringstream sstream;
  int nodeId;
  sstream<<tempstr;
  sstream>>nodeId;
  return nodeId;
}

/*--get rid of comments and empty lines--*/
bool isUsefull(char(&singleLine)[LINE_LENGTH]){
  bool usefull = false;
  if('#' == singleLine[0] || '\0'==singleLine[0])
    usefull = true;
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

/*--detect the component type by seraching its ID in the bench file--*/
/*--eg.in s536 bench, the gate with ID G60, its component type is NOR--*/
string searchBenchFile(string ID, ifstream &file){
  char benchLine[LINE_LENGTH];
  string nodeID;
  string nodeType = "noType";
  while(file.getline(benchLine,LINE_LENGTH)){
    if(true == isUsefull(benchLine)) 
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
void addNodeVec(vector<CircuitNode*> *nodes,string thisNodeName,string thisNodeNum,int(&gNum)[9],
                map<int,string> &numNameMap){
  int nId;
  CircuitNode* currentNode = new CircuitNode();

  if(thisNodeName == "INPUT"){ //construct the node with the type INPUT
    gNum[FILES_NUM]++; //numOfFiles++;
    currentNode->nodeName =thisNodeName;
    nId = extractNodeId(thisNodeNum);
    numNameMap[nId] = thisNodeNum;
    currentNode->nodeNum = thisNodeNum;
    currentNode->inputPortId=0;
    currentNode->nextNode = NULL;
    nodes->push_back(currentNode);
    //cout<<"this is the INPUT node !"<<endl;
  }
  else if(thisNodeName == "OUTPUT"){//construct the node with the type OUTPUT
    gNum[FILES_NUM]++; //numOfFiles++;
    currentNode->nodeName =thisNodeName;
    nId = extractNodeId(thisNodeNum);
    numNameMap[nId] = thisNodeNum;
    currentNode->nodeNum = thisNodeNum;
    currentNode->inputPortId=0;
    currentNode->nextNode = NULL;
    nodes->push_back(currentNode);
      //cout<<"this is the OUTPUT node!"<<endl;
  }
  else{ // construct other gates 
    //string curNodeNum = thisNodeName;//this value hold the node number for any gate
    currentNode->nodeNum = thisNodeName;//curNodeNum;
    nId = extractNodeId(thisNodeName);
    string gateName = thisNodeNum;
    stringstream out;
    //cout<<"this is the gate!"<<endl;
    //check the gate name,and count how many gates in the benchmark
    if("AND"==gateName) {
      gNum[GATES_NUM]++;//numOfGates++;
      gNum[AND_NUM]++;  //andNum++;
      out<<gNum[AND_NUM];
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    if("OR"==gateName){
      gNum[GATES_NUM]++;//numOfGates++;
      gNum[OR_NUM]++;   //orNum++;
      out<<gNum[OR_NUM];
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    if("NAND"==gateName){
      gNum[GATES_NUM]++;  //numOfGates++;
      gNum[NAND_NUM]++;   //nandNum++;
      out<<gNum[NAND_NUM];//nandNum;
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    if("NOR"==gateName){
      gNum[GATES_NUM]++; //numOfGates++;
      gNum[NOR_NUM]++;   //norNum++;
      out<<gNum[NOR_NUM];//norNum;
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    if("NOT"==gateName){
      gNum[GATES_NUM]++; //numOfGates++;
      gNum[NOT_NUM]++;   //notNum++;
      out<<gNum[NOT_NUM];//notNum;
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    if("XOR"==gateName){
      gNum[GATES_NUM]++; //numOfGates++;
      gNum[XOR_NUM]++;   //xorNum++;
      out<<gNum[XOR_NUM];//xorNum;
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    /*detect D-flip flop*/
    if("DFF"==gateName){
      gNum[GATES_NUM]++; 
      gNum[DFF_NUM]++;   //numOfDFFs++;
      out<<gNum[DFF_NUM];//numOfDFFs;
      string curNum = out.str();
      gateName = gateName + curNum;
      numNameMap[nId]=gateName;
    }
    currentNode->nodeName = gateName;
    currentNode->inputPortId = 0;//this Id should always be 0 when the node is at the beginning at
                                 //the linklist
    currentNode->nextNode = NULL;
    nodes->push_back(currentNode);
  }
}

/*--attach the node to the end of a certain linked list--*/
void attachNode(vector<CircuitNode*> *nodes,string thisNodeNum, string thisNodeName,int portid){
  int size = nodes->size();
  CircuitNode* inputNode = new CircuitNode();
  inputNode->nodeNum =thisNodeNum;
  inputNode->nodeName =thisNodeName;
  for(int n = size-1; n >= 0 ; n--){
    if((*nodes)[n]->nodeNum == thisNodeNum){
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

/*--generate one configuration line for ISCAS89 circuit simulation model--*/
void genConfigLine(int position,vector<CircuitNode*> *nodesVec,ofstream& outputStream,
                   vector<int>* numInputs,int lineId,string path){
//      int lineNum = 8; // 
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
      cout<<"list length is :"<<listLength<<endl;//}
    }//while
    if("INPUT"==ioType){ // generate one line for the configuration file, the information is for the input file. 
      outputStream<<first->nodeNum<<" ";
      outputStream<<listLength<<" "<<"I"<<" ";
      for(int i = 0; i < listLength;i++){
        outputStream<<inPortId[i]<<" ";}
      for(int i = 0; i < listLength;i++){
        outputStream<<gatesName[i]<<" ";}
        outputStream<<LINE_NUMBER<<endl;
      }
     else{
       bool isToFile=false;
       for(int i = position-1;i >= 0;i--){
         if(first->nodeNum==(*nodesVec)[i]->nodeNum){// find an output gate 
           isToFile = true;
           outputStream<<first->nodeName<<" ";
           outputStream<<(*numInputs)[lineId]<<" ";
           outputStream<<"1"<<" ";
           outputStream<<path+(*nodesVec)[i]->nodeNum<<" "; //output file name
           outputStream<<"0"<<" "<<OBJECT_DELAY<<endl;}
         }
         if(isToFile==false){
           outputStream<<first->nodeName<<" "<<(*numInputs)[lineId]<<" "<<listLength<<" ";
           for(int i = 0; i < listLength;i++){
             outputStream<<gatesName[i]<<" ";}
           for(int i = 0; i < listLength;i++){
             outputStream<<inPortId[i]<<" ";}
             outputStream<<OBJECT_DELAY<<endl;}}
  }//if
  else{
    outputStream<<first->nodeNum<<" "<<0<<" "<<"O"<<" ";
    outputStream<<0<<" "<<"null"<<" "<<LINE_NUMBER<<endl;}
}

/*--generate the simulation configuration file for a ISCAS89 circuit --*/
void conFileGen(vector<CircuitNode*> *nodes,ofstream& conFile,int(&gNum)[9],
                vector<int>* numinputs){
  conFile<<gNum[FILES_NUM]<<endl;
  conFile<<gNum[GATES_NUM]<<endl;
  string filePath = "circuitsimulationmodels/iscas89/iscas89Sim/s536/";

  int size =nodes->size();
  for (int i = 0; i < size; i++){
    genConfigLine(i, nodes,conFile,numinputs,i,filePath );
  }
}

/*--for the debugging purpose : shows the content in linked lists --*/
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
