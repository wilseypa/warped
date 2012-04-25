#ifndef SEQUENTIAL_FILE_MANAGER_H
#define SEQUENTIAL_FILE_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#ifdef HAVE_TR1_UNORDERED_MAP
#include<tr1/unordered_map>
using std::tr1::unordered_map;
#else
#include <unordered_map>
using std::unordered_map;
#endif

#include <string>
#include "warped/FileManager.h"

using std::tr1::unordered_map;
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
  unordered_map<string, fstream *> files;
};

}
#endif
