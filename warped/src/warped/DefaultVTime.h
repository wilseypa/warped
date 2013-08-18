#ifndef DEFAULT_VTIME_H
#define DEFAULT_VTIME_H


#ifndef USE_USER_VTIME

// default definition of VTIME
#include "VTime.h"
#define VTIME VTime

#else
  // define user defined VTIME here

  #include "UserDefinedVTime.h"
#endif

#endif
