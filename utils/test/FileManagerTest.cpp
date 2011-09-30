
// See copyright notice in file Copyright in the root directory of this archive.

#include "FileManagerTest.h"
#include <utils/Debug.h>
#include <utils/FileManager.h>
#include <utils/StringUtilities.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using std::endl;
using std::ofstream;
using std::cerr;

FileManagerTest *
FileManagerTest::instance(){
  static FileManagerTest *myFileManagerTest = new FileManagerTest();
  
  return myFileManagerTest;
}

int 
FileManagerTest::instanceTest() const {
  int retval = 0;

  utils::debug << "Starting instance test - ";

  const FileManager *fm1 = FileManager::instance();
  const FileManager *fm2 = FileManager::instance();

  if( fm1 == 0 || fm2 == 0 ){
    cerr << "Got a null instance for a FileManager!" << endl;
    retval = -1;
    goto end;
  }

  if( fm1 != fm2 ){
    cerr << "Got a two separate instances for FileManagers!" << endl;
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }

  return retval;
}

int 
FileManagerTest::buildDirectory( const string &dirName ) const {
  int retval = 0;

  utils::debug << "Starting directory test - ";

  // First we want to make sure that the dir doesn't exist.
  const FileManager *fm = FileManager::instance();
  if( fm->checkFileStatus( dirName ) == FileManager::OK ){
    // Uh oh, the test dir exists.  For the moment we'll complain and exit.
    cerr << "The directory " << dirName << " exists, currently I can't handle this." << endl;
    exit( -1 );
  }

  // Good, it didn't exist.  We need to create it.
  if( fm->makeDirectory( dirName ) != FileManager::OK ){
    cerr << "Got an error making directory " << dirName << endl;
    perror( "Here's why" );
    
    retval = -1;
    goto end;
  }

  if( fm->checkFileStatus( dirName, FileManager::DIRECTORY ) != FileManager::OK ){
    cerr << "Thought I built a directory, but can't find it!" << endl;
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }
  return retval;
}

const string
FileManagerTest::buildFileName( const string &dirName, 
				const string &fileSuffix,
				const int fileNum ) const {
  const FileManager *fm = FileManager::instance();

  const string fileName = dirName + fm->getDirectorySeparator() + 
    "testFile" + intToString(fileNum) + fileSuffix;

  return fileName;
}

int
FileManagerTest::buildFiles( const string &dirName, 
			     const string &fileSuffix,
			     const int numFiles ) const {

  utils::debug << "Starting file creation tests -";

  int retval = 0;

  // Let's build some files.
  for( int i = 0; i < numFiles; i++ ){
    const string fileName = buildFileName( dirName, fileSuffix, i );
    ofstream outfile( fileName.c_str() );
    if( !outfile.good() ){
      cerr << "Couldn't open file " << fileName << endl;
      perror( "Here's why" );
      goto end;
    }
    outfile << "Here's some data!" << endl;
    outfile.close();
  }
  
 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }
  return retval;
}

int
FileManagerTest::checkFiles( const string &dirName, 
			     const string &fileSuffix,
			     const int numFiles ) const {

  utils::debug << "Starting file counting test - ";

  int retval = 0;

  const FileManager *fm = FileManager::instance();
  
  // We expect that there are numCount files in the directory
  const vector<string> *fileList = fm->getAllFiles( fileSuffix + "$", dirName );
  if( fileList == 0 ){
    cerr << "Got a null file list after creating files!" << endl;
    retval = -1;
    goto end;
  }

  if( (int)fileList->size() != numFiles ){
    cerr << "Only found " << fileList->size() << " files in " << dirName
	 << " and was expecting " << numFiles << endl;
    retval = -1;
    goto end;
  }

  // Now we'll create a new file with a different suffix and try again:
  { // New scope to keep compiler happy
    const string fileName = dirName + 
      fm->getDirectorySeparator() + "extrafile" + fileSuffix + "1";

    ofstream newFile( fileName.c_str() );
    newFile << "Here's something to put in here" << endl;
    newFile.close();
    const vector<string> *newFileList = fm->getAllFiles( fileSuffix + "$", dirName );
    if( (int)newFileList->size() != numFiles ){
      cerr << "Oops, a file with a different suffix got counted!" << endl;
      retval = -1;
      goto end;
    }
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }
  return retval;
}

int
FileManagerTest::removeFile( const string &dirName,
			     const string &fileSuffix ) const {
  int retval = 0;
  utils::debug << "Starting file removal test - ";

  const string fileName = buildFileName( dirName, fileSuffix, 1 );
  const FileManager *fm = FileManager::instance();
  assert( fm->checkFileStatus( fileName, FileManager::REGULAR_FILE ) == FileManager::OK );

  if( fm->unlink( fileName ) != FileManager::OK ){
    perror( "Error" );
    retval = -1;
    goto end;
  }
  
  if( fm->checkFileStatus( fileName ) != FileManager::NOT_FOUND ){
    cerr << "Removed file " << fileName << " but FileManager:checkFileStatus gives "
	 << "erroneous status" << endl;
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }

  return retval;
}

int
FileManagerTest::removeFiles( const string &dirName,
			      const string &fileSuffix ) const {
  int retval = 0;
  utils::debug << "Starting file(s) removal test - ";

  const string fileName = buildFileName( dirName, fileSuffix, 1 );
  const FileManager *fm = FileManager::instance();
  const vector<string> *fileList = fm->getAllFiles( fileSuffix + ".*$", dirName );
  for( vector<string>::const_iterator i = fileList->begin();
       i != fileList->end();
       i++ ){
    const string fileName = (*i);
    assert( fm->checkFileStatus( fileName, FileManager::REGULAR_FILE ) == FileManager::OK );
    if( fm->unlink( fileName ) != FileManager::OK ){
      perror( "Error" );
      retval = -1;
      goto end;
    }
    if( fm->checkFileStatus( fileName ) != FileManager::NOT_FOUND ){
      cerr << "Removed file " << fileName << " but FileManager:checkFileStatus gives "
	   << "erroneous status" << endl;
      retval = -1;
      goto end;
    }
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed." << endl;
  }

  return retval;
}

int
FileManagerTest::removeDirectory( const string &directoryName ) const {

  utils::debug << "Starting remove directory test -";

  int retval = 0;
  const FileManager *fm = FileManager::instance();
  assert( fm->checkFileStatus( directoryName, FileManager::DIRECTORY ) ==
	  FileManager::OK );
  if( fm->removeDirectory( directoryName ) != FileManager::OK ){
    perror( "Error" );
    retval = -1;
    goto end;
  }

  if( fm->checkFileStatus( directoryName, FileManager::DIRECTORY ) !=
      FileManager::NOT_FOUND ){
    cerr << "Removed directory " << directoryName << " but FileManager:checkFileStatus gives "
	 << "erroneous status" << endl;
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed" << endl;
  }
  
  return retval;
}

int
FileManagerTest::baseName(){
  utils::debug << "Starting basename test - ";

  int retval = 0;
  const FileManager *fm = FileManager::instance();
  const string path1 = "/long/path/to/file.c";
  const string path2 = "secondfile.c";

  if( fm->baseName( path1 ) != "file.c" ){
    retval = -1;
    goto end;
  }

  if( fm->baseName( path2 ) != "secondfile.c" ){
    retval = -1;
    goto end;
  }

 end:
  if( retval == 0 ){
    utils::debug << "passed" << endl;
  }
  else{
    utils::debug << "failed" << endl;
  }
  return retval;
}



int 
FileManagerTest::regressionTest(){
  int retval = 0;
  const string dirName = "testDir";
  int numFiles = 100;
  const string fileSuffix = ".txt";

  utils::enableDebug();

  retval = instanceTest();
  if( retval != 0 ){
    goto end;
  }

  // This will build the test dir.
  retval = buildDirectory( dirName );
  if( retval != 0 ){
    goto end;
  }

  // This will write the files into it.
  retval = buildFiles( dirName, fileSuffix, numFiles );
  if( retval != 0 ){
    goto end;
  }

  // Now we'll count the files 
  retval = checkFiles( dirName, fileSuffix, numFiles );
  if( retval != 0 ){
    goto end;
  }

  // Now we'll remove a file
  retval = removeFile( dirName, fileSuffix );
  if( retval != 0 ){
    goto end;
  }

  // Now we'll recount the directory having removed a file.
  numFiles--;
  retval = checkFiles( dirName, fileSuffix, numFiles );
  if( retval != 0 ){
    goto end;
  }

  // Now we'll kill the rest of the files in that subdir
  retval = removeFiles( dirName, fileSuffix );
  if( retval != 0 ){
    goto end;
  }

  // Now let's kill the directory
  retval = removeDirectory( dirName );
  if( retval != 0 ){
    goto end;
  }

  // Basename test
  retval = baseName();
  if( retval != 0 ){
    goto end;
  }

 end:
  return retval;
}
