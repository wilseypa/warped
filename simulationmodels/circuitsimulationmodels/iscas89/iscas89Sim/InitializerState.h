#ifndef  INITIALIZER_H
#define  INITIALIZER_H

#include <FlatState.h>

class InitializerState : public FlatState {
  public :
    InitializerState(){time = 0;}

    ~InitializerState(){}

    const int getSize() const {
      return sizeof(InitializerState);
    }

    unsigned int getStateSize() const {}

    int time;
};

#endif
