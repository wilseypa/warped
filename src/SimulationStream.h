#ifndef SIMULATION_STREAM_H
#define SIMULATION_STREAM_H


#include <fstream>                      // for fstream
#include <sstream>
#include <iostream>

class SimulationStream : public std::fstream {
public:

    /**@name Public Class Methods of SimulationStream. */
    //@{

    /// Default destructor
    virtual ~SimulationStream() {}

    /// virtual method that does the actual job of "endl".
    virtual void flush() = 0;

    /// Returns a handle for reading a file for the stream.
    virtual std::fstream* getInputStream() = 0;

    /// Reads one line from the file.
    virtual std::ostringstream& readLine(std::ostringstream& ost) = 0;

    /// Inserts data from ost into the stream.
    virtual void insert(std::ostringstream& ost) = 0;

    //@} // End of Public Class Methods of SimulationStream.

protected:

    /**@name Public Class Methods of SimulationStream. */
    //@{

    SimulationStream() {}

    //@} // End of Public Class Methods of SimulationStream.
};

extern std::ostream& nl(std::ostream& os);

#endif
