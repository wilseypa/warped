
#include <stdlib.h>                     // for exit
#include <fstream>                      // for ostringstream, etc
#include <iostream>                     // for cerr

#include "SequentialSimulationStream.h"

using std::cerr;
using std::endl;

SequentialSimulationStream::SequentialSimulationStream(const std::string& fileName,
                                                       ios::openmode mode)
    : closeflag(true) {
    // Open the file the user has specified
    std::fstream::open(fileName.c_str(), mode);

    // check if the open operation was successful...
    if (!good()) {
        cerr << "SequentialSimulationStream:: Error Opening File - "
             << fileName << endl;
        exit(-41);
    }

    //   setBuffer(mode);
}

SequentialSimulationStream::SequentialSimulationStream(std::streambuf* buf,
                                                       ios::openmode mode)
    : closeflag(false) {
    ios::rdbuf(buf);
}


SequentialSimulationStream::~SequentialSimulationStream() {
    flush();
    if (closeflag == true) {
        close();
    }
}

std::fstream*
SequentialSimulationStream::getInputStream() {
    return this;
}

std::ostringstream&
SequentialSimulationStream::readLine(std::ostringstream& ost) {
    char buff[8192];
    getline(buff, 8192);
    ost << buff;
    return ost;
}

void
SequentialSimulationStream::insert(std::ostringstream& ost) {
    *this << ost.str();
}
