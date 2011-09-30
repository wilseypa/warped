// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/FileManager.h"
#include "warped/SequentialFileManager.h"

namespace warped {

FileManager &
FileManager::instance(){
  static FileManager *singleton = initSingleton();
  return *singleton;
}

FileManager::FileManager(){}

FileManager::~FileManager(){}

FileManager *
FileManager::initSingleton(){
  // Ultimately we need to check and see if we're running sequential or
  // parallel and so forth.
  FileManager *retval = 0;
  if( true ){
    retval = new SequentialFileManager();
  }
  return retval;
}

}
