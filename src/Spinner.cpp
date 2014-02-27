#include "Spinner.h"

#include <iostream>

#include "WarpedConfig.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void Spinner::spinForever() {
    std::cerr << "The configuration is set to enter an infinite loop. "
              << "You'll need to break it with the "
              << "debugger.  See Spinner: " << __LINE__
              << " in gdb to break this loop.";
#ifdef HAVE_UNISTD_H
    std::cerr << "\npid = " << getpid();
#endif
    std::cerr << std::endl;

    // This has to be volatile so it doesn't get optimized out.
    volatile bool x = true;
    // Spin until someone uses a debugger and flips x to false.
    while (x) {}
}

