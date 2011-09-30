#ifndef SIMULATION_STREAM_H
#define SIMULATION_STREAM_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include <iostream>
#include <fstream>

using std::fstream;

class SimulationStream : public fstream {
public:

   /**@name Public Class Methods of SimulationStream. */
   //@{

   /// Default destructor
   virtual ~SimulationStream(){}

   /// virtual method that does the actual job of "endl".
   virtual void flush() = 0;

   /// Returns a handle for reading a file for the stream.
   virtual fstream *getInputStream() = 0;

   /// Reads one line from the file.
   virtual ostringstream &readLine(ostringstream &ost) = 0;

   /// Inserts data from ost into the stream.
   virtual void insert(ostringstream &ost) = 0;
   
   //@} // End of Public Class Methods of SimulationStream.

protected:
   
   /**@name Public Class Methods of SimulationStream. */
   //@{

   SimulationStream(){}
   
   //@} // End of Public Class Methods of SimulationStream.
};

extern ostream& nl(ostream& os);

#endif
