// See copyright notice in file Copyright in the root directory of this archive.

#include "warped/SequentialFileManager.h"
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::cerr;
using std::vector;

namespace warped {

    SequentialFileManager::SequentialFileManager(){}

    SequentialFileManager::~SequentialFileManager(){

      vector<fstream *> *_files= new vector<fstream *>;
      //Obtains all the objects from files
      for(unordered_map<string, fstream *>::iterator i = files.begin(); i != files.end(); i++) {
          _files->push_back(i->second);
      }


      for( vector<fstream *>::iterator i = _files->begin();
           i < _files->end();
           i++ ){
        if( (*i) ){
          (*i)->close();
        }
      }
      delete _files;
    }

    fstream &
    SequentialFileManager::open( const string &fileName,
                                 ios::openmode mode ) {
        fstream *retval = 0;
        unordered_map<string, fstream *>::iterator it = files.find( fileName );
        //If objectName is not found the end iterator is returned
        if ( it == files.end()) {
            retval = new fstream( fileName.c_str(), mode );
            if( fileName == "stdout" ){
              dynamic_cast<ios *>(retval)->rdbuf(cout.rdbuf());
            }
            else if ( fileName == "stderr" ){
              dynamic_cast<ios *>(retval)->rdbuf(cerr.rdbuf());
            }
            else if ( fileName == "stdin" ){
                  dynamic_cast<ios *>(retval)->rdbuf(cin.rdbuf());
            }
            files.insert( make_pair(fileName, retval) );
        }
        else {
            retval = it->second;
        }

        return *retval;
    }

}
