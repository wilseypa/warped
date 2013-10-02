
#include "SequentialSimulationStream.h"
using std::cerr;
using std::endl;

SequentialSimulationStream::SequentialSimulationStream(const string& fileName,
                                                       ios::openmode mode)
    : closeflag(true) {
    // Open the file the user has specified
    fstream::open(fileName.c_str(), mode);

    // check if the open operation was successful...
    if (!good()) {
        cerr << "SequentialSimulationStream:: Error Opening File - "
             << fileName << endl;
        exit(-41);
    }

    //   setBuffer(mode);
}

SequentialSimulationStream::SequentialSimulationStream(streambuf* buf,
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

fstream*
SequentialSimulationStream::getInputStream() {
    return this;
}

ostringstream&
SequentialSimulationStream::readLine(ostringstream& ost) {
    char buff[8192];
    getline(buff,8192);
    ost << buff;
    return ost;
}

void
SequentialSimulationStream::insert(ostringstream& ost) {
    *this << ost.str();
}
