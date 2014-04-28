#include "eclmplConfigFileTable.h"

eclmplConfigFileTable::eclmplConfigFileTable() {
} // End of Default Constructor

eclmplConfigFileTable::~eclmplConfigFileTable() {
} // End of Destructor

void eclmplConfigFileTable::addEntry(const std::vector<std::string>& entry) {
    configTable.push_back(entry);
} // End of addEntry(...).

std::vector<std::string> eclmplConfigFileTable::getEntry(const unsigned int& entryNr) const {
    ASSERT(entryNr <= (unsigned int)(configTable.size()-1));
    return configTable[entryNr];
} // End of getEntry(...).

void eclmplConfigFileTable::addToEntry(const std::string& add, const unsigned int& entryNr) {
    ASSERT(entryNr <= (unsigned int)(configTable.size()-1));
    configTable[entryNr].push_back(add);
} // End of addToEntry(...).

unsigned int eclmplConfigFileTable::getNumberOfEntries() const {
    return configTable.size();
} // End of getNumberOfEntries().

void eclmplConfigFileTable::serialize(char* buf, unsigned int& bufSize) const {
    std::ostringstream serializedBuf;

    for (unsigned int i = 0; i < configTable.size(); i++) {
        serializedBuf << configTable[i].size() << DELIMITER;
        for (unsigned int j = 0; j < configTable[i].size(); j++) {
            serializedBuf << configTable[i][j] << DELIMITER;
        }
    }

    std::string tmpBuf = serializedBuf.str();
    ASSERT(bufSize >= tmpBuf.size()+1);
    bufSize = tmpBuf.size() +1;
    memcpy(buf, tmpBuf.c_str(), bufSize);
} // End of serialize(...).

void eclmplConfigFileTable::deserialize(const char* const buf) {
    if (buf == NULL) {
        return;
    }

    // receive the message as a character stream
    std::istringstream inputStream(buf);

    unsigned int stringsInEntry;
    std::vector<std::string> entry;
    std::string tmpString;
    while (inputStream >> stringsInEntry) {
        entry.clear();
        for (unsigned int i = 0; i < stringsInEntry; i++) {
            inputStream >> tmpString;
            entry.push_back(tmpString);
        }
        configTable.push_back(entry);
    }
} // End of deserialize(...).

std::ostream& operator<< (std::ostream& os, const eclmplConfigFileTable& in) {
    for (unsigned int i = 0; i < in.configTable.size(); i++) {
        std::copy(in.configTable[i].begin(), in.configTable[i].end(), std::ostream_iterator<std::string>(os, " "));
        os << std::endl;
    }
    return os;
}