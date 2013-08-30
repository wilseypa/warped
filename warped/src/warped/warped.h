#ifndef WARPED_H
#define WARPED_H


#include "WarpedConfig.h"

/** This header file defines some of the default data types and
    enumerations used in the system. */
#include <stdlib.h> //Used for abort()
#include <algorithm> //Used for sort()
#include <sys/types.h>
#ifdef HAVE_STDINT_H
// The following is a C-99ism...
#include <stdint.h>
typedef int64_t warped64_t;
typedef int32_t warped32_t;
#elif defined(SIZEOF_LONG_LONG_) // defined in warped-config.h
typedef long long warped64_t;
typedef long warped32_t;
#else
typedef long VTimeMajor_t;
typedef long VTimeMinor_t;
#warning Could not find 64 bit type using 32 bit max - NOT recommended.
#endif

warped64_t getWarped64Max();
warped64_t getWarped64Min();
warped32_t getWarped32Max();
warped32_t getWarped32Min();

#include <iostream>
#include <WarpedDebug.h>

#include <sstream>
using std::ostringstream;

// Console I/O operations - we define this stream interface to trap 
// the I/O produced by different processes and feed the I/O to the 
// central console 

extern std::ostream *wout, *werr;

// include the default definition of VTime
#include "VTime.h"
// include the default definition of OBJECT_ID
#include "DefaultObjectID.h"

// This definition for bool is used to ease portability between different
// compliers (some of them have pre-defined type "bool" while some don't).
// Correspondingly, the definition for the constants "true" and "false"
// used in the system are here.

// Check if NULL is defined. If not define it here
#ifndef NULL
#define NULL   0
#endif

// Define the ASSERT statement that will be turned on when the compiler
// flag DEVELOPER_ASSERTIONS is turned "on"
#ifndef ASSERT
#ifndef  NO_DEVELOPER_ASSERTIONS
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif
#endif

// Define the DEBUG statement that will be turned on when the compiler
// flag DEVELOPER_ASSERTIONS is turned "on"
#ifndef DEBUG
#ifndef NO_DEVELOPER_ASSERTIONS
#define DEBUG(x)  x 
#else
#define DEBUG(x)
#endif
#endif



// Define the type of the tag used to seralize and deSerialize stuff
typedef int SerializationTag;

// the delimiter used in serialization and deserialization
const char DELIMITER = ' ';

// Time Warp Event Message Type - it is either a POSITIVE or NEGATIVE msg.
enum EventMessageType {POSITIVE, NEGATIVE};

// The severity enumerations in the system. The order of enumeration is
// from Good to Bad.
enum SEVERITY {NOTE, WARNING, ERROR, ABORT};

/// Enumeration for message suppression
//  Cases where message can be
//  suppressed: Coastforwarding and during lazy and lazyAggr
//  cancellation
enum MsgSuppression {NO, COASTFORWARD, LAZYCANCEL, ADAPTIVECANCEL};

/// Enumeration for OutputManager type.
//  Used so that dynamic casts are not necessary to check type.
//  Need to know which type in order to handle messages differently.
enum OutputMgrType {AGGRMGR, LAZYMGR, ADAPTIVEMGR};

/// Enumeration for StateManager type. 
//  Used so that dynamic casts are not necessary to check type.
enum StateMgrType {DEFAULTSTATE, STATICSTATE, ADAPTIVESTATE};

/** used as a flag for executing a find operation */
enum findMode {LESS, LESSEQUAL, EQUAL, GREATEREQUAL, GREATER};

// a templatized getMinimum function - returns the object that is the
// least.
template <class type>
inline const type&
MIN_FUNC(const type& x, const type& y) {
  if (x < y) { return x; }
  return y;
}

// a templatized getMaximum function - returns the object that is the
// largest.
template <class type>
inline const type&
MAX_FUNC(const type& x, const type& y) {
  if (x > y) { return x; }
  return y;
}

// the following two classes are passed in as function objects 
// to the hash_map data structure 
class EqualID {
public:
  size_t operator()(const OBJECT_ID *s1, const OBJECT_ID *s2) const {
    return (*s1 == *s2);
  }
};

// hash function for object ids
struct OIDHash{
  size_t operator()(const OBJECT_ID& id) const { return size_t(&id); }
};

// hash function object for object ids
class hashID {
public:
  size_t operator()(const OBJECT_ID *objectID) const {
    OIDHash myHashFunction;
    return (myHashFunction(*objectID));
  }
};

inline
std::ostream &operator<<( std::ostream &os, SEVERITY severity ){
  switch( severity ){
  case NOTE:
    os << "NOTE";
    break;
  case WARNING:
    os << "WARNING";
    break;
  case ERROR:
    os << "ERROR";
    break;
  case ABORT:
    os << "ABORT";
    break;
  default:
   //Converts severity from string to int, output is stream.str()
   ostringstream stream;
   stream << severity;
   os << "<Unknown Severity " + stream.str() +" >";
  }

  return os;
}

#if defined (__i386) || defined (__x86_64)

// from http://en.wikipedia.org/wiki/Time_Stamp_Counter#C.2B.2B
extern "C" {
  __inline__ warped64_t rdtsc(void) {
    warped32_t lo, hi;
    __asm__ __volatile__ (
      #ifdef i386
          "pushl %%ebx;"
      #endif
      "xorl %%eax,%%eax; cpuid; rdtsc;"
      #ifdef i386
          "popl %%ebx;"
      #endif
      :"=a" (lo), "=d" (hi)
          ::"%rcx"
      #ifndef i386
          ,"%rbx"
      #endif
      );
      return (warped64_t)hi << 32 | lo;
  }
}
#endif
#endif
