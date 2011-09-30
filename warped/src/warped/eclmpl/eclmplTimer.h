#ifndef ECLMPL_TIMER_H
#define ECLMPL_TIMER_H

#include "warped/warped.h"
#include "eclmplCommonInclude.h"

class eclmplTimer {
public: 
  eclmplTimer(const double timeOutVal);
  bool timedOut() const;
  void setTimeOut(const double timeOutVal);
  inline void start() { gettimeofday(&startTime, NULL); }
protected:
  eclmplTimer(){} // Disallow instantiation without a timeOut value.
  timeval startTime;
  double timeOut;
}; 

#endif
