#include <sstream>
#include "eclmplConfigFileTable.h"

using std::ostringstream;
using std::istringstream;

void
eclmplConfigFileTable::serialize(char *buf, unsigned int &bufSize) const {
  ostringstream serializedBuf;

  for (unsigned int i = 0; i < configTable.size(); i++) {
    serializedBuf << configTable[i].size() << DELIMITER;
    for (unsigned int j = 0; j < configTable[i].size(); j++) {
      serializedBuf << configTable[i][j] << DELIMITER;
    }
  }

  string tmpBuf = serializedBuf.str();
  bufSize = tmpBuf.size() +1;
  memcpy(buf, tmpBuf.c_str(), bufSize);
} // End of serialize(...).

void
eclmplConfigFileTable::deserialize(const char * const buf) {
  if(buf == NULL){
    return;
  }
  
  // receive the message as a character stream
  istringstream inputStream(buf);

  unsigned int stringsInEntry;
  vector<string> entry;
  string tmpString;
  while (inputStream >> stringsInEntry) {
    entry.clear();
    for (unsigned int i = 0; i < stringsInEntry; i++) {
      inputStream >> tmpString;
      entry.push_back(tmpString);
    }
    configTable.push_back(entry);
  }
} // End of deserialize(...).
