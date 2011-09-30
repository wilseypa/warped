
// See copyright notice in file Copyright in the root directory of this archive.

#include "PluginManager.h"
#include "PluginBase.h"
#include "FileManager.h"
#include "Debug.h"
#include <assert.h>
#include <iostream>

using std::cerr;
using std::endl;

PluginManager * 
PluginManager::instance(){
  static PluginManager *instance = new PluginManager();
  return instance;
}

PluginManager::PluginManager(){
  int errorCount = lt_dlinit();
  if( errorCount != 0 ){
    lastError = lt_dlerror();
  }
}

PluginManager::~PluginManager(){}

const vector<PluginBase *> *
PluginManager::getPluginList( const string &dir ){
  vector<PluginBase *> *retval = new vector<PluginBase *>;

  //get a file manager
  const FileManager* filemanager = FileManager::instance();

  //pointer to the allocate function in every library
  const vector<string> *fileList = filemanager->getAllFiles( ".*la$", dir );
  if( fileList != 0 ){
    string currentFile;
    for( vector<string>::const_iterator i = fileList->begin();
	 i != fileList->end();
	 i++ ){
      currentFile = *i;
      PluginBase *currentPlugin = loadPlugin( currentFile );
      if( currentPlugin != 0 ){
	retval->push_back( currentPlugin );
      }
      else{
	cerr << "Error loading plugin " << *i << ", error was \"" << lastError << "\"" << endl;
      }
    }
  }

  return retval;
}

PluginBase *
PluginManager::loadPlugin( const string &fileName ){
  PluginBase *retval = 0;

  lt_dlhandle lib_handle;
  //try to dlopen
  utils::debug << "Filename = " << fileName << endl;
  lib_handle = lt_dlopen( fileName.c_str() );
  if( lib_handle != 0 ){
    const string allocatorName = getAllocatorNameFromFileName( fileName );
    //get the allocate funcation
    void* (* allocateFunction)()=
      (void *(*)())lt_dlsym(lib_handle, allocatorName.c_str() );

    if( allocateFunction == 0 ){
      const char *error = lt_dlerror();
      if( error != 0 ){       
	lastError = error;
// 	lt_dlclose(lib_handle);
      }     
    }
    else{
      //cast to savant_PluginBase
      retval = static_cast<PluginBase *>((*allocateFunction)());
      if( retval->getPluginSignature() != PluginBase::pluginSignature() ){
	lastError = "invalid plugin signature";
      }
      //      dlclose(lib_handle);
    }

    loadedPlugins[fileName] = lib_handle;
  }
  else{
    lastError = lt_dlerror();
  }

  return retval;
}

void
PluginManager::unloadPlugin( const string &fileName ){
  if (lt_dlclose((loadedPlugins.find(fileName))->second)) {
    lastError = lt_dlerror();
  } else {
    loadedPlugins.erase(fileName);
  }    
}

const string &
PluginManager::getLastError() const {
  return lastError;
}

int 
PluginManager::addSearchDir(const string &search_dir ) {
  return lt_dladdsearchdir(search_dir.c_str());
}

int 
PluginManager::setSearchPath(const string &search_path ) {
  return lt_dlsetsearchpath(search_path.c_str());
}

const string 
PluginManager::getSearchPath() {
  string temp(lt_dlgetsearchpath());
  return temp;
}

bool 
PluginManager::isValidPlugin( const string &fileName ){
  bool retval = false;
  PluginBase *plugin = loadPlugin( fileName );
  if( plugin != 0 ){
    retval = true;
    delete plugin;
  }
  return retval;
}

const string
PluginManager::getAllocatorNameFromFileName( const string &filePath ) const {
  const string fileName = FileManager::instance()->baseName( filePath );
  // Now strip off the last three chars, either ".la" or ".so".
  
  string pluginName = fileName;

  string::size_type newEnd = pluginName.rfind( ".la" );
  if( newEnd != string::npos ){
    pluginName.erase( newEnd );
  }
  else{
     newEnd = pluginName.rfind( ".so" );
     if( newEnd != string::npos ){
       pluginName.erase( newEnd );
     }
     else{
       pluginName = "";
     }
  }

  return "allocate" + pluginName;
}
