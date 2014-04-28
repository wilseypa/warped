#ifndef ECLMPL_COMMON_INCLUDE_H
#define ECLMPL_COMMON_INCLUDE_H

#include "warped.h"
#include "SimulationConfiguration.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // TCP_NODELAY defined here.
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

#include <iterator>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>

// No messages with greater size may be received or sent.
// This number itself can be changed of course...
#define ECLMPL_MTU 50000  // This is equal to the maximum message aggregation size.
#define MAXBUF 65535

#define __local__stringize(y) #y
#define __local__add_quotes(x) __local__stringize (x)

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

typedef unsigned long SequenceNumber;

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

#endif
