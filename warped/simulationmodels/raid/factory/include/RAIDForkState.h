//-*-c++-*-
#ifndef RAIDFORKSTATE_H
#define RAIDFORKSTATE_H

#include "SerializedInstance.h"
#include "State.h"

/** The class RAIDForkState.
*/
class RAIDForkState : public State {
public:

  RAIDForkState();
  ~RAIDForkState(){};

  void copyState(const State* state);

  unsigned int getStateSize() const;

  const SerializedInstance* serialize();
  State *deserialize(const SerializedInstance  instance);

  int getLastOutput() const {return lastOutput;}
  void setLastOutput(int out) {lastOutput = out;}

  std::ostream& printState(std::ostream& os);

private:
  int lastOutput;
};

#endif
