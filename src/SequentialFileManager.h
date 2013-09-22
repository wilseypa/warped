#ifndef SEQUENTIAL_FILE_MANAGER_H
#define SEQUENTIAL_FILE_MANAGER_H


#include "warped.h"
#include <unordered_map>
#include <string>
#include "warped/FileManager.h"

using std::string;

namespace warped{

class SequentialFileManager : public FileManager {
  friend class FileManager;
public:
  fstream &open( const string &fileName, ios::openmode mode );

protected:
  SequentialFileManager();
  ~SequentialFileManager();

private:
  std::unordered_map<string, fstream *> files;
};

}
#endif
