
#include <stdlib.h>                     // for NULL, abort
#include <iostream>                     // for cerr
#include <set>                          // for _Rb_tree_const_iterator, etc
#include <vector>                       // for vector

#include "FileData.h"                   // for FileData
#include "FileQueue.h"                  // for FileQueue, string, ios, etc
#include "InFileData.h"                 // for InFileData
#include "InFileQueue.h"                // for InFileQueue
#include "Serializable.h"               // for Serializable
#include "SerializedInstance.h"         // for SerializedInstance
#include "SimulationManager.h"          // for SimulationManager
#include "SimulationObject.h"           // for SimulationObject
#include "TimeWarpSimulationManager.h"  // for TimeWarpSimulationManager
#include "TimeWarpSimulationStream.h"
#include "VTime.h"                      // for VTime

using std::stringstream;

TimeWarpSimulationStream::TimeWarpSimulationStream(const string& fileName,
                                                   ios::openmode mode,
                                                   SimulationObject* simObj) {
    mySimulationObject = simObj;
    mySimulationManager = dynamic_cast<TimeWarpSimulationManager*>(simObj->getSimulationManager());
    if (mode == ios::in) {
        inFileQueue = new InFileQueue(fileName);
        outFileQueue = NULL;
    } else if ((mode == ios::out) || (mode == ios::app)) {
        inFileQueue = NULL;
        outFileQueue = new FileQueue(fileName);
    }
}

TimeWarpSimulationStream::~TimeWarpSimulationStream() {
    delete inFileQueue;
    delete outFileQueue;
}

void
TimeWarpSimulationStream::flush() {
    ASSERT(outFileQueue != NULL);

    // Do not store the line if the buffer was empty.
    string* line = new string(myOutputBuffer.str());
    if (*line != "") {
        myOutputBuffer.str("");
        outFileQueue->storeLine(mySimulationObject->getSimulationTime(), line);
        seekp(ios::beg);
    }
}

void
TimeWarpSimulationStream::insert(ostringstream& ost) {
    // Do not insert the data during the coast forward phase.
//  if(mySimulationManager->getCoastForwardTime(
//     mySimulationObject->getObjectID()->getSimulationObjectID()) == 0){
    myOutputBuffer << ost.str();
//  }
}

fstream*
TimeWarpSimulationStream::getInputStream() {
    if (inFileQueue != NULL) {
        return inFileQueue->access();
    } else {
        return NULL;
    }
}

ostringstream&
TimeWarpSimulationStream::readLine(ostringstream& ost) {
    // Do not read anything when coasting forward.
    inFileQueue->storePos(mySimulationObject->getSimulationTime());
    char buff[8192];
    inFileQueue->access()->getline(buff,8192);
    ost << buff;

    return ost;
}

void
TimeWarpSimulationStream::fossilCollect(const VTime& fossilCollectTime) {
    if (inFileQueue != NULL) {
        inFileQueue->fossilCollect(fossilCollectTime);
    }
    if (outFileQueue != NULL) {
        outFileQueue->fossilCollect(fossilCollectTime);
    }
}

void
TimeWarpSimulationStream::fossilCollect(int fossilCollectTime) {
    if (inFileQueue != NULL) {
        inFileQueue->fossilCollect(fossilCollectTime);
    }
    if (outFileQueue != NULL) {
        outFileQueue->fossilCollect(fossilCollectTime);
    }
}

void
TimeWarpSimulationStream::rollbackTo(const VTime& rollbackToTime) {
    if (inFileQueue != NULL) {
        inFileQueue->rollbackTo(rollbackToTime);
    }
    if (outFileQueue != NULL) {
        outFileQueue->rollbackTo(rollbackToTime);
    }
}

/// Used in optimistic fossil collection to save the state of the stream.
void
TimeWarpSimulationStream::saveCheckpoint(ofstream* outFile, unsigned int saveTime) {
    char del = '_';

    // If there is an input file, save the position of the file at the checkpoint time.
    if (inFileQueue != NULL) {
        outFile->write(&del, sizeof(del));

        multiset< InFileData >::iterator it = inFileQueue->begin();
        while (it != inFileQueue->end() && it->getTime().getApproximateIntTime() < saveTime) {
            it++;
        }

        // If there are no position saves after the checkpoint time, use the current
        // position in the file.
        streampos curPos;
        if (it != inFileQueue->end()) {
            curPos = it->getPosition();
        } else {
            curPos = inFileQueue->access()->tellg();
        }

        outFile->write((char*)(&curPos), sizeof(curPos));
    }

    // If there is an output file, save all data that is less than the checkpoint time
    // and save the current end of file position.
    if (outFileQueue != NULL) {
        const VTime* time;
        unsigned int timeSize;
        const char* timePtr;
        SerializedInstance* toWrite;
        const string* line;
        unsigned int lineSize;

        multiset< FileData >::iterator it = outFileQueue->begin();
        while (it != outFileQueue->end() && it->getTime().getApproximateIntTime() < saveTime) {
            time = &it->getTime();
            toWrite = new SerializedInstance(time->getDataType());
            time->serialize(toWrite);

            timePtr = &toWrite->getData()[0];
            timeSize = toWrite->getSize();

            line = it->getLine();
            lineSize = line->size();

            outFile->write((char*)(&timeSize), sizeof(timeSize));
            outFile->write(&del, sizeof(del));
            outFile->write(timePtr, timeSize);
            outFile->write(&del, sizeof(del));
            outFile->write((char*)(&lineSize), sizeof(lineSize));
            outFile->write(line->c_str(), lineSize);

            it++;
        }
        outFile->write(&del, sizeof(del));

        streampos end = outFileQueue->getEOFPosition();
        outFile->write((char*)(&end), sizeof(end));
    }
}

/// Used in optimistic fossil collection to restore the state of the stream.
void
TimeWarpSimulationStream::restoreCheckpoint(ifstream* inFile, unsigned int restoreTime) {
    char del = '_';
    char delIn;

    // First, empty out what is already in the queues.
    if (inFileQueue != NULL) {
        inFileQueue->fossilCollect(mySimulationManager->getPositiveInfinity());
    }

    if (outFileQueue != NULL) {
        outFileQueue->fossilCollect(mySimulationManager->getPositiveInfinity());
    }

    // If there is an input file, read the file position, and set the file position.
    if (inFileQueue != NULL) {
        if (inFile->peek() == del) {
            inFile->read(&delIn, sizeof(delIn));
        }

        streampos curPos;
        inFile->read((char*)(&curPos), sizeof(curPos));
        inFileQueue->restoreFilePosition(curPos);
    }

    // If there is an output file, restore the data set.
    if (outFileQueue != NULL) {
        string* line;
        unsigned int lineSize;
        unsigned int timeSize;

        while (inFile->peek() != del) {
            inFile->read((char*)(&timeSize), sizeof(timeSize));
            inFile->read(&delIn, sizeof(delIn));

            if (delIn != del) {
                cerr << mySimulationManager->getSimulationManagerID() <<  " - ALIGNMENT ERROR in "
                     << "OutFileQueue restoreCheckpoint. Got: " << delIn << endl;
                abort();
            }

            // Read the time in.
            char* timeBuf = new char[timeSize];
            inFile->read(timeBuf, timeSize);
            SerializedInstance* serVTime = new SerializedInstance(timeBuf, timeSize);
            VTime* restoredTime = dynamic_cast<VTime*>(serVTime->deserialize());

            inFile->read(&delIn, sizeof(delIn));

            if (delIn != del) {
                cerr << mySimulationManager->getSimulationManagerID() <<  " - ALIGNMENT ERROR in "
                     << "outFileQueue restoreCheckpoint. Got: " << delIn << endl;
                abort();
            }

            // Read the file line data.
            inFile->read((char*)(&lineSize), sizeof(lineSize));
            char* lineBuf = new char[lineSize];
            inFile->read(lineBuf, lineSize);

            line = new string(lineBuf, lineSize);

            // Restore the file queue.
            outFileQueue->storeLine(*restoredTime, line);
        }

        if (inFile->peek() == del) {
            inFile->read(&delIn, sizeof(delIn));
        }

        // Set the file to the position of the last rollback and delete any file
        // commits that occurred after the rollback.
        streampos end;
        inFile->read((char*)(&end), sizeof(end));
        outFileQueue->setPosAndClear(end);
    }
}
