
// See copyright notice in file Copyright in the root directory of this archive.

#include "PosixFileManager.h"
#include <stdlib.h>
#include <regex.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <libgen.h>

using std::cerr;
using std::endl;
using std::ifstream;
using std::ios;

static const int COMPARE_BUFFER_SIZE = 2048;

FileManager::FileStatus
PosixFileManager::checkFileStatus( const string &filename, FileType MODE ) const {
  struct stat file_stat;
  if( stat( filename.c_str(), &file_stat ) < 0 ){
    return NOT_FOUND;
  }
  
  if( MODE != DONTCARE ){
    switch( MODE ){
    case DIRECTORY:{
      if( !S_ISDIR( file_stat.st_mode ) ){
	return NOT_TYPE;
      }
      break;
    }
    
    case REGULAR_FILE:{
      if( !S_ISREG( file_stat.st_mode ) ){
	return NOT_TYPE;
      }
      break;
    }

    default:{
      cerr << "Don't know how to check for file type " << (int)MODE << endl;
      abort();
    }
    } 
  }
  
  return OK;
}


FileManager::FileStatus 
PosixFileManager::makeDirectory( const string &directory_name, mode_t mode ) const {
  if( mkdir( directory_name.c_str(), mode ) == 0 ){
    return OK;
  }
  else{
    if( errno == EEXIST ){
      // Then the directory already exists - that's OK.
      return OK;
    }
    else{
      // Something else happened.
      lastError = errno;
      return ERROR;
    }
  }
}

FileManager::FileStatus 
PosixFileManager::removeDirectory( const string &directory_name ) const {
  if( rmdir( directory_name.c_str() ) == 0 ){
    return OK;
  }
  else{
    if( errno == ENOENT ){
      return OK;
    }
    else{
      lastError = errno;
      return ERROR;
    }
  }
}

FileManager::FileStatus 
PosixFileManager::changeDirectory( const string &to_directory ) const {
  if( chdir( to_directory.c_str() ) == 0 ){
    return OK;
  }
  else{
    lastError = errno;
    return ERROR;
  }
}

FileManager::FileStatus
PosixFileManager::unlink( const string &fileName ) const {
  if( ::unlink( fileName.c_str() ) == 0 ) {
    return OK;
  }
  else {
    lastError = errno;
    return ERROR;
  }
}

FileManager::FileStatus
PosixFileManager::rename( const string &from, const string &to ) const {
  if (::rename( from.c_str(), to.c_str() ) == 0) {
    return OK;
  }
  else {
    lastError = errno;
    return ERROR;
  }
}


const string
PosixFileManager::findNewestFile( const string &reg_exp, 
				  const string &directory ) const {
  string retval = "";

  //compile the regular expression
  regex_t preg;
  int status = regcomp( &preg, reg_exp.c_str(), REG_ICASE );
  //make sure regcomp worked
  assert( status == 0 );

  // holds the current file and status.
  // note:  dirent's are reused by readdir.  Because they return null at the end of the 
  // directory listing I do not think it is necessary to call delete on currentEntry.
  dirent* pcurrent_dirent;
  struct stat current_stat;
  
  // holds the newest file and status of that file found so far.
  struct stat newest_stat;
  // set time to 0 to avoid special case
  newest_stat.st_mtime=0;

  //open directory for reading
  DIR *currentDir = openDir( directory );
  if( currentDir != 0 ) {
    pcurrent_dirent = readdir(currentDir);
    while( pcurrent_dirent != NULL) {
      // build the full path to the current file
      if( regexec(&preg, pcurrent_dirent->d_name, 0, NULL, 0) == 0) {
	string fullPath =
	  directory + getDirectorySeparator() + pcurrent_dirent->d_name;
	
	if ( stat( fullPath.c_str(), &current_stat ) != 0) {
	  lastError = errno;
	  break;
	}
	else {
	  if( current_stat.st_mtime > newest_stat.st_mtime ){
	    //memcpy because readdir reuses memory
	    memcpy(&newest_stat, &current_stat, sizeof(current_stat));
	    retval = pcurrent_dirent->d_name;
	  }
	}
      }
      pcurrent_dirent = readdir(currentDir);
    }
    
    closedir( currentDir );
  }

  return retval;
}

DIR *
PosixFileManager::openDir( const string &dirName ) const {
  DIR *retval = opendir( dirName.c_str() );
  if( retval == 0 ){
    lastError = errno;
  }
  
  return retval;
}

vector<string> *
PosixFileManager::getAllFiles( const string &reg_ex, const string &dir ) const {
  vector<string> *retval = new vector<string>;

  //compile the regular expression
  regex_t preg;
  int status = regcomp( &preg, reg_ex.c_str() , REG_ICASE );
  //make sure regcomp worked
  assert( status == 0 );

  //note:  dirent's are reused by readdir.  Because they return null at the end of the 
  //directory listing I do not think it is necessary to call delete on currentEntry.
  dirent* pcurrent_dirent;
  
  //open directory for reading
  DIR* currentDir = openDir( dir );
  if( currentDir != 0 ){
    while((pcurrent_dirent = readdir(currentDir)) != NULL) {
      //build the full path to the current file
      if( regexec(&preg, pcurrent_dirent->d_name, 0, NULL, 0) == 0) {
	string fullPath =  dir + getDirectorySeparator() + pcurrent_dirent->d_name;  
	retval->push_back( fullPath );
      }
    }
  }
  
  status = closedir( currentDir );
  if( status != 0 ){
    lastError = errno;
  }
  
  return retval;
}



int
PosixFileManager::fileCompare( const string &fileName1, const string &fileName2 )const {
  ifstream file1;
  ifstream file2;
  int      difference = 0;
  int      fileSize1, fileSize2;
  int      bytesToRead;
  char *compare_buffer1;
  char *compare_buffer2;
  
  file1.open( fileName1.c_str(), ios::in );
  file2.open( fileName2.c_str(), ios::in );

  if (file2.good() == 0) {
    // Okay. The original file does not exists. Can't compare
    file1.close();
    return 256;
  }
  
  file1.seekg(0, ios::end);
  fileSize1 = file1.tellg();
  file1.seekg(0, ios::beg);
  
  file2.seekg(0, ios::end);
  fileSize2 = file2.tellg();
  file2.seekg(0, ios::beg);
  
  if (fileSize1 == fileSize2) {
    // Compare the files only if they are of the same size.
    
    compare_buffer1 = new char[COMPARE_BUFFER_SIZE];
    compare_buffer2 = new char[COMPARE_BUFFER_SIZE];
    
    while (fileSize1 > 0)  {
      bytesToRead = ((fileSize1 > COMPARE_BUFFER_SIZE) ? COMPARE_BUFFER_SIZE
		     : fileSize1);

      file1.read( compare_buffer1, bytesToRead );
      file2.read( compare_buffer2, bytesToRead );
      
      fileSize1 -= bytesToRead;
      
      if ((difference = memcmp(compare_buffer1, compare_buffer2, bytesToRead)) != 0) {
	break;
      }
    }
    
    delete [] compare_buffer1;
    delete [] compare_buffer2;
  }
  else {
    difference = 256;
  }
  
  file1.close();
  file2.close();
  
  return difference;
}

const string
PosixFileManager::getDirectorySeparator() const {
  return "/";
}

const vector<string> &
PosixFileManager::staticGetLibraryDirectories(){
  static vector<string> *libDirs = new vector<string>;
  if( libDirs->size() == 0 ){
    libDirs->push_back( "." );  
    libDirs->push_back( "/usr/lib" );
    libDirs->push_back(  "/usr/local/lib" );
  }

  return *libDirs;
}

const vector<string> &
PosixFileManager::getLibraryDirectories() const {
  return staticGetLibraryDirectories();
}

const string
PosixFileManager::getRealPath( const string &file_name ) const {
  static char buf[PATH_MAX];
  char *full_path = realpath( file_name.c_str(), buf );
  if( !full_path ){
    lastError = errno;
  }
  return full_path;
}


const string
PosixFileManager::baseName( const string &pathToFile ) const {
  return ::basename( const_cast<char *>(pathToFile.c_str()) );
}  

const string
PosixFileManager::getLastError() const {
  if( !(lastError < 0 ) ){
    return string(strerror(lastError));
  }
  else{
    return "";
  }
}


FileManager *
PosixFileManager::instance(){
  static FileManager *my_instance = new PosixFileManager();
  return my_instance;
}


const FileManager *
FileManager::instance(){
  return PosixFileManager::instance();
}
