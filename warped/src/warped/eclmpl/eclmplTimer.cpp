#include "eclmplTimer.h"

eclmplTimer::eclmplTimer(const double timeOutVal) : timeOut(timeOutVal) {
  start();
} // End of constructor.

bool 
eclmplTimer::timedOut() const {
  bool retVal;
  timeval currTime;
  gettimeofday(&currTime, NULL);
  double timeDiff = (double)((currTime.tv_sec*1.0+currTime.tv_usec/1.0e+6)-
			     (startTime.tv_sec*1.0+startTime.tv_usec/1.0e+6));
  if (timeDiff >= timeOut) {
    retVal = true;
  }
  else {
    retVal = false;
  }
  return retVal;
} // End of timedOut().

inline void
eclmplTimer::setTimeOut(const double timeOutVal) {
  timeOut = timeOutVal;
} // End of setTimeOut(...).

