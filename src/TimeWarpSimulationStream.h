#ifndef TIMEWARP_SIMULATION_STREAM_H
#define TIMEWARP_SIMULATION_STREAM_H


#include <fstream>                      // for ostringstream, ios, etc
#include <sstream>
#include <string>                       // for string

#include "FileQueue.h"
#include "InFileQueue.h"
#include "SimulationObject.h"           // for ios, etc
#include "SimulationStream.h"           // for SimulationStream, fstream
#include "TimeWarpSimulationManager.h"
#include "warped.h"                     // for ostringstream

class FileQueue;
class InFileQueue;
class TimeWarpSimulationManager;
class VTime;

using std::ostringstream;
using std::ofstream;
using std::ifstream;
using std::string;

class TimeWarpSimulationStream : public SimulationStream {
    friend class TimeWarpSimulationManager;
public:

    /**@name Public Class Methods of TimeWarpSimulationStream. */
    //@{

    /** Default constructor.

        Constructor for creating a TimeWarp simulation file stream.

        @param fileName name of file to open
        @mode  open file for reading/writing
        @simObj object handle.
    */
    TimeWarpSimulationStream(const string& fileName, ios::openmode mode,
                             SimulationObject* simObj);

    /// Default destructor
    ~TimeWarpSimulationStream();

    /// this function does the job of "endl"
    void flush();

    /// Returns a handle for reading a file for the stream.
    virtual fstream* getInputStream();

    /// Reads one line from the file.
    virtual ostringstream& readLine(ostringstream& ost);

    /// Inserts data from ost into the stream.
    virtual void insert(ostringstream& ost);

    /// call fossil collect on the file queue
    void fossilCollect(const VTime& fossilCollectTime);

    /// call fossil collect on the file queue.
    /// used in optimistic fossil collection only.
    void fossilCollect(int fossilCollectTime);

    /// call rollback on the file queue
    void rollbackTo(const VTime& rollbackToTime);

    /// Used in optimistic fossil collection to save the state of the stream.
    void saveCheckpoint(ofstream* outFile, unsigned int saveTime);

    /// Used in optimistic fossil collection to restore the state of the stream.
    void restoreCheckpoint(ifstream* inFile, unsigned int restoreTime);

    //@} // End of Public Class Methods of TimeWarpSimulationStream.

protected:

    /**@name Protected Class Attributes of TimeWarpSimulationStream. */
    //@{

    /// handle to my simulation object
    SimulationObject* mySimulationObject;

    /// handle to my simulation manager
    TimeWarpSimulationManager* mySimulationManager;

    /// handle to an Output File Queue
    FileQueue* outFileQueue;

    /// handle to an input File Queue
    InFileQueue* inFileQueue;

    /// A output buffer if the file turns out to be an output file...
    ostringstream myOutputBuffer;

    /// A pointer to the original file buffer for performing the actual I/O
    std::streambuf* originalBuffer;

    //@} // End of Protected Class Methods of TimeWarpSimulationStream.

private:
    /**@name Private Class Attributes of TimeWarpSimulationStream. */
    //@{

    /// Internal useful method used to setup buffers
    void setBuffer(ios::openmode mode);

    /** A constructor that takes a fileDescriptor as a parameter. This
        constructor is used by the TimeWarpSimulationManager to initialize wout
        and werr with suitable output and error streams.  This constructor is
        private so that only the TimeWarpSimulationManager can utilize this
        method.
        @param fileHandle handle to the fstream to be used.
        @param mode the mode of the stream being set. */
    TimeWarpSimulationStream(int fileHandle, ios::openmode mode);
    //@} // End of Private class methods of TimeWarpSimulationStream

    /**@name Private class attributes of TimeWarpSimulationManager. */
    //@{

    /// A flag to indicate if fileStream has to be closed by the destructor
    bool closeFlag;

    //@} // End of Private Class attributes of TimeWarpSimulationStream

};

#endif
