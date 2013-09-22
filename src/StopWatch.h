#ifndef STOP_WATCH_H
#define STOP_WATCH_H

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// StopWatch.h                                                              //
//                                                                          //
// Tom Green                                                                //
// February 2, 1995                                                         //
//                                                                          //
// The stop watch is started and stopped by calling start() and stop().     //
// Once the watch has been started, elapsed() returns (as a double) the     //
// amount of real time, in seconds, which elapsed between the start() and   //
// stop() calls.  If the watch has been started but not stopped, the        //
// amount of elapsed time since start() is returned (without stopping       //
// the watch).                                                              //
//                                                                          //
// With the overhead of function calls and a little error checking the      //
// times won't be extremely precise, but it should be good enough.          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <sys/time.h>   // to get the system time

class StopWatch{
private :
  struct timeval start_tv; // has tv_sec and tv_usec elements (from sys/time.h)
  struct timezone start_tz; // needed for gettimeofday() call
  struct timeval stop_tv;
  struct timezone stop_tz;
  struct timeval temp_tv;  // used if want elapsed time without calling stop
  struct timezone temp_tz;
  struct timeval temp2_tv;  // used if want elapsed time without calling stop
  struct timezone temp2_tz;
  struct timeval lap_tv;  // used if want lap time without calling stop
  struct timezone lap_tz;
#if defined(__SVR4) && defined(sun)
  hrtime_t startValue;
  hrtime_t stopValue;
  hrtime_t tempValue;
  hrtime_t tempValue2;
  hrtime_t lapValue;
#endif
  bool started;
  bool stopped;
  bool lap;
  double timeDifference(timeval& start, timeval& stop);

public :
  ////////////////////
  //  Constructors  //
  ////////////////////

  StopWatch();
  ~StopWatch();

  ////////////////////////
  //  Member Functions  //
  ////////////////////////

  void start();      // start the StopWatch
  void stop();       // stop the StopWatch
  void reset();

  // return the elapsed time in seconds since the start.
  double elapsed();

  // return the elapsed time in seconds since the start, or since the last
  // call to lapTime (whichever came last).
  double lapTime();

}; // end of class StopWatch

#endif  // STOP_WATCH_H
