#ifndef ECLMPL_COMMON_INCLUDE_H
#define ECLMPL_COMMON_INCLUDE_H

#include "warped.h"
#include "SimulationConfiguration.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // TCP_NODELAY defined here.
#include <netdb.h>
#include <string>
#include <string.h>
#include <strings.h>

#include <set>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <queue>

using std::binary_function;
using std::ostream_iterator;

#ifndef ECLMPL_ASSERT
#include <assert.h>
#define ECLMPL_ASSERT(x) assert(x)
#else
#define ECLMPL_ASSERT(x)
#endif

#ifndef ECLMPL_DEBUG
#define ECLMPL_DEBUG(x) x
#else
#define ECLMPL_DEBUG(x)
#endif

// Sequence number arithmetic. Numbers are allowed to wrap around.
typedef unsigned long SequenceNumber;
#define SeqLT(a,b)     ((long)((a)-(b)) < 0)
#define SeqLEQ(a,b)    ((long)((a)-(b)) <= 0)
#define SeqGT(a,b)     ((long)((a)-(b)) > 0)
#define SeqGEQ(a,b)    ((long)((a)-(b)) >= 0)

// When high priority is considered a low number, we have to reverse the operator <.
// I.e. x has higher priority than y if x < y. So, for a less-operator relating to
// this priority, less(x, y) is true if x > y.
template <class _Tp>
struct lessPriority : public binary_function<_Tp,_Tp,bool> {
    bool operator()(const _Tp __x, const _Tp __y) const {
        return *__x > *__y; // checking based on ' class::operator< '
    }
};

/**
   This function does a strdup like the tradition library function, except
   it uses "new" to allocate the memory.  Memory returned needs to be
   deleted with "delete []".

   @param toDup String to duplicate.  Does not have to be null terminated
   if the second parameter is supplied.
   @param len (optional) The length of the string to duplicate.  If this
   parameter is not supplied, strlen will be called on "toDup".  (In that
   case "toDup" should be NULL terminated!
*/
inline char* cppStrDup(const char* toDup, int len = -1) {
    char* retval = 0;
    if (len == -1) {
        len = strlen(toDup) + 1;
        retval = new char[len];
        memcpy(retval, toDup, len);
        retval[ len - 1 ] = '\0';
    } else {
        retval = new char[len];
        memcpy(retval, toDup, len);
    }
    return retval;
}


#define __local__stringize(y) #y
#define __local__add_quotes(x) __local__stringize (x)

// This is a debugging function that lets you print a character string
// that may contain control characters.
#if 0
void __eclmpl__printCharStr(const char* const str, const unsigned int& strLen) {
    cerr << "\"";
    for (unsigned int i = 0; i < strLen; i++) {
        if (str[i] == '\0')
        { cerr << "'\\0'"; }
        else if (str[i] == '\n')
        { cerr << "'\\n'"; }
        else if (str[i] >= 32 && str[i] <= 126)
        { cerr << str[i]; }
        else
        { cerr << "'ascii_" << (int)str[i] << "'"; }
    }
    cerr << "\"";
}
#endif

// No messages with greater size may be received or sent.
// This number itself can be changed of course...
#define ECLMPL_MTU 50000  // This is equal to the maximum message aggregation size.

#endif
