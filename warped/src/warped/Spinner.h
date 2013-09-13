#ifndef SPINNER_H
#define SPINNER_H

// This code allows us to insert places where an "infinite" loop is entered.
// The variable we spin on is marked volatile, so the compiler won't optimize
// it out. This means we can come along in the debugger and flip it. This is
// useful with the "attach" option of gdb.

namespace Spinner {
void spinForever();
} //namespace Spinner

#endif
