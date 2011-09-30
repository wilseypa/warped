#ifndef SPINNER_H
#define SPINNER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/SimulationConfiguration.h"
#include <string>
using std::string;

/** This code allows us to insert places where an "infinite" loop is
    entered.  The variable we spin on is marked volatile, so the compiler
    won't optimize it out.  This means we can come along in the debugger and
    flip it.  This is useful with the "attach" option of gdb.
    
    The second argument, key, basically allows us to easily add places we'd
    like to spin.  Current keys are SpingBeforeConfiguration and
    SpinBeforeSimulation.
*/
class Spinner {
public:
  static void spinIfRequested( const string &keyName,
			       SimulationConfiguration &configuration );

};

#endif
