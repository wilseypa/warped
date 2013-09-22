#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#define LINE_LENGTH 256
#define LINE_NUMBER 8
#define OBJECT_DELAY 1

using namespace std;

// the struct for storing the circuit node information 
class CircuitNode{
	public:
		string nodeNum; 
		string nodeName;
		int inputPortId;
		CircuitNode *nextNode;
};

int extractNodeId(string nName);

//void genConfigLine(CircuitNode* first,ofstream& outputStream,string& ioType,vector<int>* numInputs,int lineId);
void genConfigLine(int position, vector<CircuitNode* >* nodesVec,ofstream& outputStream,vector<int>* numInputs,int lineId,string path);

int main (int argc, char* argv[]){

 char oneLine[LINE_LENGTH];  
 ifstream isacsBenchMark;
 ofstream simModelConfig;

 // ids for NOT,AND,OR,NAND,NOR and XOR gates
 int andNum =0;
 int orNum  =0;
 int nandNum =0;
 int xorNum =0;
 int notNum =0;
 int norNum =0;
 
 int numOfFiles = 0;
 int numOfGates = 0;

 map<int,string> numNameMap; 
 int nId;

 isacsBenchMark.open(argv[1]);
 simModelConfig.open(argv[2]);

 if(!isacsBenchMark.is_open()){
	 cout <<"Fail to open the input file !" <<endl; 
	 exit (-1);
 }

 if(!simModelConfig.is_open()){
	 cout<<"Fail to open the output file !"<<endl;	 
	 exit(-1);
 }

  vector <CircuitNode* > *nodes = new vector<CircuitNode* >;
	vector <int> *numOfInputsVec = new vector<int>;//record the input number of each object. 
	                                               //gates, input and output files are objects.
	                                               //each input file has 0 input.
  
  while(isacsBenchMark.getline(oneLine,LINE_LENGTH)){ //for each line in the isacs85 benchmark
		int lineCount = 0;
		int lineLength = strlen(oneLine);
		bool havePid = false;
		for(int i =0; i<lineLength;i++){ //strip space and () in isacs85 benchmark
			if (oneLine[i] =='(')
				oneLine[i]=' ';	
			if (oneLine[i] ==')')
				oneLine[i]=' ';
			if (oneLine[i] =='=')
				oneLine[i]=' ';
			if (oneLine[i] == ',')
				oneLine[i]=' ';
		}

    /* check for the striping 
    cout<<"strip space and ( )"<<endl;
  	istringstream thisLine(oneLine,istringstream::in);
		string aa = thisLine.str();
		cout<<aa<<endl;
		*/

	  CircuitNode* currentNode = new CircuitNode();
		string thisNodeName; //for INPUT and the OUTPUT
  	istringstream thisLine(oneLine,istringstream::in);
		thisLine>>thisNodeName;
		
		if(thisNodeName == "INPUT"){ //construct the node with the type INPUT
      numOfFiles++; 
			currentNode->nodeName =thisNodeName;
			string thisNodeNum;
			thisLine>>thisNodeNum;
		  nId = extractNodeId(thisNodeNum);
			numNameMap[nId] = thisNodeNum; 
      currentNode->nodeNum = thisNodeNum;
			currentNode->inputPortId=0;
			currentNode->nextNode = NULL;
			nodes->push_back(currentNode);
			//cout<<"this is the INPUT node !"<<endl;
		}	
		else if(thisNodeName == "OUTPUT"){//construct the node with the type OUTPUT
			numOfFiles++;
			currentNode->nodeName =thisNodeName;
			string thisNodeNum;
			thisLine>>thisNodeNum;
			nId = extractNodeId(thisNodeNum);
			numNameMap[nId] = thisNodeNum; 
			currentNode->nodeNum = thisNodeNum;
			currentNode->inputPortId=0; 
			currentNode->nextNode = NULL;
			nodes->push_back(currentNode);
		 //cout<<"this is the OUTPUT node!"<<endl;
		}	
		else{ // construct other gates 
	  	string thisNodeNum = thisNodeName;//this value hold the node number for any gate
			currentNode->nodeNum = thisNodeNum;
			nId = extractNodeId(thisNodeNum);
			string gateName;
			thisLine>>gateName;
			stringstream out;
			//cout<<"this is the gate!"<<endl;
      //check the gate name,and count how many gates in the benchmark
			if("AND"==gateName) {
				numOfGates++;
				andNum++; 
				out<<andNum;
				string curNum = out.str();
        gateName = gateName + curNum;
				numNameMap[nId]=gateName;
      }
			if("OR"==gateName){
				numOfGates++;
				orNum++;
				out<<orNum;
				string curNum = out.str();
        gateName = gateName + curNum;
			  numNameMap[nId]=gateName;
      }
			if("NAND"==gateName){ 
				numOfGates++;
				nandNum++;
			  out<<nandNum;
			  string curNum = out.str();
				gateName = gateName + curNum;
				numNameMap[nId]=gateName;
      }
			if("NOR"==gateName){
				numOfGates++;
				norNum++;
				out<<norNum;
				string curNum = out.str();
				gateName = gateName + curNum;
				numNameMap[nId]=gateName;
			}
			if("NOT"==gateName){
				numOfGates++;
				notNum++;
				out<<notNum;
				string curNum = out.str();
				gateName = gateName + curNum;
				numNameMap[nId]=gateName;
			}
			if("XOR"==gateName){ 
				numOfGates++;
				xorNum++;
				out<<xorNum;
			  string curNum = out.str();
			  gateName = gateName + curNum;
				numNameMap[nId]=gateName;
      }
      currentNode->nodeName = gateName;
			currentNode->inputPortId = 0;//this Id should always be 0 when the node is at the beginning at
			                             //the linklist
			currentNode->nextNode = NULL;
      nodes->push_back(currentNode);

			string inputGateNum;
			int portId = 0;
			
			while(thisLine>>inputGateNum){ //attach the information of the current gate to its input gates
				//cout<<"the inputGateNum is"<<inputGateNum<<endl;
		    CircuitNode* inputNode = new CircuitNode();
				inputNode->nodeNum =thisNodeNum;
			  nId=extractNodeId(thisNodeNum);
			  inputNode->nodeName = numNameMap[nId];
				
				int n =0 ;
				//cout<<"the first while !"<<endl;
				while(n<nodes->size()){
					if((*nodes)[n]->nodeNum == inputGateNum){
						CircuitNode* nextNodePt	= (*nodes)[n]->nextNode;
						CircuitNode* currentPt =nextNodePt;
            if(nextNodePt==NULL){
							(*nodes)[n]->nextNode = inputNode;
							inputNode->inputPortId = portId+1;
							inputNode->nextNode = NULL;
						}
            else{
							while(nextNodePt != NULL){//go to the end of the linklist which begines with node[n] 
							  currentPt = nextNodePt;
             		nextNodePt = nextNodePt->nextNode;
							}
							currentPt->nextNode=inputNode;
							inputNode->inputPortId =portId+1;
							inputNode->nextNode=NULL;
				  } 
				}
				n++;
			}
			portId++;
		} //while
			//bool havePid = false;
			numOfInputsVec->push_back(portId);//wrong
			havePid = true;
		}//else
		if(false==havePid){//
			numOfInputsVec->push_back(0);//wrong
		}
		else{havePid=false;}
		lineCount++;
}//while

// cout<<"out of the while !"<<endl;
 int j= 0;
   while(j<nodes->size()){
		 cout<<(*nodes)[j]->nodeNum<<" ";
		 CircuitNode* nextGate = (*nodes)[j]->nextNode;
		 CircuitNode* currentGate = new CircuitNode();
		 while(nextGate != NULL){
		 	currentGate = nextGate;
		  cout<< currentGate->nodeNum<<" ";
			cout<< currentGate->nodeName<<" ";
			cout<< currentGate->inputPortId<<" ";
			nextGate = nextGate->nextNode;
		 }
	 	 cout<<endl;
		 j++;
	 }

	simModelConfig<<numOfFiles<<endl;
	simModelConfig<<numOfGates<<endl;
	string filePath = "circuitsimulationmodels/iscas85/iscas85Sim/c3540/";

	int capacity  = 0;
  int linenum = 0;
	while(capacity<nodes->size()){
		//genConfigLine(capacity,(*nodes)[capacity],simModelConfig,(*nodes)[capacity]->nodeName,numOfInputsVec,linenum);
		genConfigLine(capacity,nodes,simModelConfig,numOfInputsVec,linenum,filePath);
		capacity++;
		linenum++;
	}
isacsBenchMark.close();
simModelConfig.close();

}//main

void genConfigLine(int position,vector<CircuitNode*> *nodesVec,ofstream& outputStream,
									 vector<int>* numInputs,int lineId,string path){
//	int lineNum = 8; // 
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


int extractNodeId(string nName){
	string tempstr = nName;
	tempstr.erase(0,1);
	stringstream sstream;
	int nodeId;
	sstream<<tempstr;
  sstream>>nodeId;
  return nodeId;
}
