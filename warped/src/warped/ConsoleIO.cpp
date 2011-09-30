// See copyright notice in file Copyright in the root directory of this archive.

#include "ConsoleIO.h"

void directOutputToConsoleOutputStream(){
   // originalCout = cout;
   // cout = *wout;
}

void resetConsoleOutputStream(){
   // *wout << ends;
   // cout = originalCout;
}

void directErrorToConsoleErrorStream(){
   // originalCerr = cerr;
   // cerr = *werr;
}

void resetConsoleErrorStream(){
   // *werr << ends;
   // cerr = originalCerr;
}


