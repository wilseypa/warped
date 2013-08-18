#ifndef ECLMPL_CONFIG_FILE_TABLE_H
#define ECLMPL_CONFIG_FILE_TABLE_H

#include "eclmplCommonInclude.h"

//typedef struct eclmplConfigFileTableEntry vector<string>;

class eclmplConfigFileTable {
public:
  eclmplConfigFileTable() {}
  ~eclmplConfigFileTable() {}

  inline void addEntry(const vector<string> &entry) { configTable.push_back(entry); }

  inline vector<string> getEntry(const unsigned int &entryNr) const {
    ASSERT(entryNr <= (configTable.size()-1));
    return configTable[entryNr];
  }

  inline void addToEntry(const string &add, const int &entryNr) {
    configTable[entryNr].push_back(add);
  }

  inline const unsigned int getNumberOfEntries() const { return configTable.size(); }

  void serialize(char *buf, unsigned int &bufSize) const;
  void deserialize(const char * const buf);

  friend std::ostream& operator<< (std::ostream& os, const eclmplConfigFileTable &in) {
    for (unsigned int i = 0; i < in.configTable.size(); i++) {
      copy(in.configTable[i].begin(), in.configTable[i].end(), std::ostream_iterator<string>(os, " "));
      os << std::endl;
    }
    return os;
  }
private:
  vector<vector<string> > configTable; // Note space between > and >.
};

#endif
