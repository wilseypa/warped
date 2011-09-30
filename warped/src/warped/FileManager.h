#ifndef WARPED_FILE_MANAGER_H
#define WARPED_FILE_MANAGER_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <fstream>
using std::fstream;
using std::ios;
#include <string>
using std::string;

namespace warped{

class FileManager {
public:
  static FileManager &instance();
  virtual fstream &open( const string &fileName, ios::openmode ) = 0;

protected:
  FileManager();
  virtual ~FileManager() = 0;

private:
  static FileManager *initSingleton();
};

}
#endif
