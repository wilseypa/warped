//-*-c++-*-
#ifndef RAIDPROCESSSTATE_H
#define RAIDPROCESSSTATE_H

#include "State.h"
#include "SerializedInstance.h"
#include "../../../rnd/MLCG.h"

/** The class RAIDProcessState.
*/
class RAIDProcessState : public State {
public:
  RAIDProcessState();
  ~RAIDProcessState();

  RAIDProcessState(const RAIDProcessState &state);

  // this is needed to copy User State
  RAIDProcessState& operator=(RAIDProcessState&);
  void copyState(const State* state);

  unsigned int getStateSize() const;
  const SerializedInstance* serialize();
  State *deserialize(const SerializedInstance  instance);

  MLCG *getGen() const {return gen;}
  int getNumRequests() const {return numRequests;}
  bool getDiskOperationPending() const {return diskOperationPending;}
  bool getRead() const {return read;}
  bool getStopProcessing() const {return stopProcessing;}
  int get_size() const {return size;}
  int getParitySize() const {return paritySize;}
  int getNumReads() const {return numReads;}
  int getNumWrites() const {return numWrites;}
  int getBeginningOfStrype() const {return beginningOfStrype;}
  int getStrypeSize() const {return strypeSize;}
  int getNumDisks() const {return numDisks;}
  int getFirstDiskId() const {return firstDiskId;}

  void setGen(MLCG *generate) {gen = generate;}
  void setNumRequests(int requests) {numRequests = requests;}
  void setDiskOperationPending(bool pending) {diskOperationPending = pending;}
  void setRead(bool isRead) {read = isRead;}
  void setStopProcessing(bool stop) {stopProcessing = stop;}
  void set_size(int newSize) {size = newSize;}
  void setParitySize(int newSize) {paritySize = newSize;}
  void setNumReads(int num) {numReads = num;}
  void setNumWrites(int num) {numWrites = num;}
  void setBeginningOfStrype(int beginning) {beginningOfStrype = beginning;}
  void setStrypeSize(int newSize) {strypeSize = newSize;}
  void setNumDisks(int num) {numDisks = num;}
  void setFirstDiskId(int id) {firstDiskId = id;}

  friend ostream& operator<< (ostream& os, const RAIDProcessState& rps);
  ostream& printState(ostream& os);

  int  *diskRequests;
private:
  MLCG *gen;
  int numRequests;
  bool diskOperationPending, read, stopProcessing;
  int size, paritySize, numReads, numWrites;
  int beginningOfStrype, strypeSize;
  int numDisks, firstDiskId;
};

#endif
