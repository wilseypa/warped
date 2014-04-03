
#ifndef SEQUENTIAL_SIMULATION_STREAM_H
#define SEQUENTIAL_SIMULATION_STREAM_H


#include <fstream>                      // for ios, ostringstream, fstream, etc
#include <string>                       // for string

#include "SimulationStream.h"           // for fstream, SimulationStream

using std::ios;

class SequentialSimulationStream : public SimulationStream {
    friend class SequentialSimulationManager;
public:

    /**@name Public Class Methods of SequentialSimulationStream. */
    //@{

    /** Default constructor.

    Constructor for creating a sequential simulation file stream.

    @param fileName name of file to open
    @mode  open file for reading/writing
    @flag  output to file or console (true or false)
    */
    SequentialSimulationStream(const std::string& fileName, ios::openmode mode);

    /// Default destructor
    ~SequentialSimulationStream();

    /// virtual method that does the actual job of "endl".
    virtual void flush() { std::fstream::flush(); }

    /// Returns a handle for reading a file for the stream.
    virtual std::fstream* getInputStream();

    /// Reads one line from the file.
    virtual std::ostringstream& readLine(std::ostringstream& ost);

    /// Inserts data from ost into the stream.
    virtual void insert(std::ostringstream& ost);


    //@} // End of Public Class Methods of SequentialSimulationStream.

protected:

private:
    /** A constructor that takes a fileDescriptor as a parameter. This
        constructor is used by the SequentialSimulationManager to initialize
        wout and werr with suitable output and error streams.  This
        constructor is private so that only the SequentialSimulationManager
        can utilize this method.  @param fileHandle handle to the fstream to
        be used.

        @param mode the mode of the stream being set. */
    SequentialSimulationStream(std::streambuf*, ios::openmode mode);

    /**
       If we were opened with a filename, then we need to close the file at
       destruction.  If we were opened with a pointer to someone elses rdbuf,
       then we don't want to call "close".
    */
    bool closeflag;
};

#endif
