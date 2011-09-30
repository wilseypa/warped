//-*-c++-*-
#ifndef RAIDDISKSTATE_H
#define RAIDDISKSTATE_H

#include "warped.h"
#include "SerializedInstance.h"
#include "State.h"

/** The class RAIDDiskState.
*/
class RAIDDiskState: public State {
public:
  RAIDDiskState();
  ~RAIDDiskState();

  RAIDDiskState& operator=(RAIDDiskState&);
  void copyState(const State* state);

  unsigned int getStateSize() const;

  const SerializedInstance* serialize();
  State *deserialize(const SerializedInstance  instance);

  int getCurrentCylinder() const;
  void setCurrentCylinder(int cylinder);

  ostream& printState(ostream& os);

private:
  int currentCylinder;
};

#endif

