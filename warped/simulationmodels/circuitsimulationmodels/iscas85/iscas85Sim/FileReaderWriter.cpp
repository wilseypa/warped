#include "FileReaderWriter.h"
#include "LogicEvent.h"
#include "FileState.h"
#include <iostream>

using namespace std;

FileReaderWriter::FileReaderWriter(string &filename,int numofgates, string io, 
                                   vector<int> *desportid,vector<string> *desgatesnames,
                                   int maxnumlines):fileName(filename),numOfGates(numofgates),
                                   IO(io),desPortId(desportid),desGatesNames(desgatesnames),
                                   maxNumLines(maxnumlines),fileIoStream(0){}

FileReaderWriter::~FileReaderWriter(){ }

void
FileReaderWriter::initialize(){
  string objname = getName();
  //cout<<endl;
  //cout<<"this is " << objname<<endl;
  if(numOfGates != 0){
    outputHandles = new SimulationObject *[numOfGates];
    for (int i = 0; i < numOfGates; i++ ){
      outputHandles[i] = getObjectHandle((*desGatesNames)[i]);
     // cout<<"my des gate is "<<((*desGatesNames)[i])<<endl;
    }
  }

  if("I"==IO){
    fileIoStream = openInputFile(fileName);
    ostringstream bitstring;
    string bitS = getLine(fileIoStream,bitstring);
    if (bitS=="")
      cout<<"Fail to read the first line from the input file"<<endl;
      else{
        FileState *state = dynamic_cast<FileState *>(getState());
        int bit = getBitValue(bitS);
        //cout<<"the bit read from "<<fileName<<" is "<<bit<<endl;
        clearOstringstream(bitstring);
        IntVTime sendTime = dynamic_cast<const IntVTime&>(getSimulationTime());
        double ldelay = 1.0;
        IntVTime recvTime = sendTime +(int)ldelay;
        LogicEvent *newEvent = new LogicEvent(sendTime,recvTime,this,this);
        newEvent->bitValue = bit;
        newEvent->sourcePort = 1;
        newEvent->destinationPort =0; // leave this field, will be filled out in the executeProcess()
        state->numLinesProcessed++;
       // cout<<state->numLinesProcessed<<" lines have been read. "<<endl;
        this->receiveEvent(newEvent);
      }
  }
  if("O"==IO){
    fileIoStream = openOutputFile(fileName,ios::out);
    //cout<<"I have no des gate."<<endl;
  }
  //cout<<getName()<<" finishes the initilization."<<endl;
}

void
FileReaderWriter::finalize(){}

void
FileReaderWriter::executeProcess(){
  FileState *state = static_cast<FileState *> (getState());
  //cout<<endl;
  //cout<<"in the executeProcess()"<< getName()<<endl;
  LogicEvent *logicEvent = NULL;
  while(true == haveMoreEvents()){
    logicEvent = (LogicEvent *)getEvent();
    if(logicEvent != NULL){
      IntVTime sendTime = static_cast<const IntVTime&>(getSimulationTime());
      if("I"==IO){
        //send the event to the gate
      for(int i = 0; i < numOfGates; i++){
      //SimulationObject *receiver = getObjectHandle(desGateName);
        LogicEvent *sendToGate = new LogicEvent(sendTime,sendTime+1,this,outputHandles[i]);
        sendToGate->bitValue = logicEvent->bitValue;
        sendToGate->sourcePort = logicEvent->sourcePort;
        sendToGate->destinationPort = (*desPortId)[i];
        
        outputHandles[i]->receiveEvent(sendToGate);
        //cout<<(*desGatesNames)[i]<<" receives the event"<<endl;
        //cout<<"des port is "<<(*desPortId)[i]<<endl;
      }
      //read one line and send an event to itself
      int lineprocessed = state->numLinesProcessed;
      ostringstream bitstring;
      string bitS = getLine(fileIoStream,bitstring);
      if( bitS != "" ){ // Check if we are at the end of the file or not
        int bit = getBitValue(bitS);
        clearOstringstream(bitstring);
        LogicEvent *sendToSelf = new LogicEvent(sendTime,sendTime+1,this,this);
        sendToSelf->setbitValue(bit);
        sendToSelf->setsourcePort(1);
        sendToSelf->setdestinationPort(0);
        state->numLinesProcessed++;
        this->receiveEvent(sendToSelf);
      } else {
					break;
			}
    }	
    if("O"==IO){
      ostringstream outstream;
      outstream<<logicEvent->bitValue;
      fileIoStream->insert(outstream);
      fileIoStream->flush();
    }
   }
  }
}


State*
FileReaderWriter::allocateState() {
  return (State*) new FileState();
}

void
FileReaderWriter::deallocateState(const State *state){
  delete (FileState*) state;
}
		 
void
FileReaderWriter::reclaimEvent(const Event *event){
  delete (LogicEvent *)event;
}

void
FileReaderWriter::reportError(const string &msg, SEVERITY level){}

SimulationStream*
FileReaderWriter::openInputFile(string& filename){
  return getIFStream(filename);
}

SimulationStream*
FileReaderWriter::openOutputFile(string& filename,ios::openmode mode){
  return getOFStream(filename, mode);
}

bool
FileReaderWriter::haveMoreLines(SimulationStream* simPt){
  return !(simPt->eof());
} 
		
string
FileReaderWriter::getLine(SimulationStream* simPt, ostringstream& ost){
  return (simPt->readLine(ost)).str();
}
	 
void
FileReaderWriter::clearOstringstream(ostringstream& ost){
  ost.str("");
}
				 
int
FileReaderWriter::getBitValue(string logicBit){
  stringstream sstream;
  int bitValue;
  sstream << logicBit;
  sstream >> bitValue;
  return bitValue;
}
