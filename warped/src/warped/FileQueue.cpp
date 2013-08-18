
#include "FileQueue.h"
#include <cstdio>
#include <fstream>
using std::ofstream;
using std::ifstream;

int FileQueue::standardOut = -1;

FileQueue::FileQueue(const string &fileName) {
  open( fileName );
}

FileQueue::FileQueue(int standardOutFlag){
  // We instantiate a out file queue for writing output to stdout.  This
  // is done as follows: FileQueue is newed with the constructor having
  // the argument FileQueue::standardOut.  This tells the file queue to
  // output all the data that it recieves to stdout and not to a file.
  // The file id is also accessed as FileQueue::standardOut.
  if(standardOutFlag != 1){
    cerr << "FileQueue: Error in use of constructor" << endl;
    cerr << "Integer Parameter to constructor != 1" << endl;
    abort();
  }
  statusBit = standardOutFlag;
  
  // outFile is set to cout
  outFile = &cout;
  if (!outFile->good()) {
    cout << "FileQueue: error opening standard out " << "\n";
    abort();
  }
}


FileQueue::~FileQueue() {
  if(statusBit == 0){
     (static_cast<ofstream *>(outFile))->close();
  }
  delete outFile;
}

void
FileQueue::close() {

   if (statusBit == 0) {
      ((ofstream *)outFile)->close();
   }
}

void
FileQueue::open(const string &fileName) {
  open( fileName, std::ios_base::out );
}

void
FileQueue::open(const string &fileName, ios::openmode mode) {
  outFileName = fileName;
  statusBit = 0;
  
  outFile = new ofstream(fileName.c_str(), mode);
  if (!outFile->good()) {
    perror( ("FileQueue: Error opening file " + fileName ).c_str() );
    abort();
  }
}

std::streampos
FileQueue::getEOFPosition(){
  outFile->seekp(0, ios::end);
  return outFile->tellp();
}

void
FileQueue::setPosAndClear(std::streampos pos){
  outFile->seekp(0, ios::end);
  std::streampos curEnd = outFile->tellp();

  if(pos < curEnd){
    ((ofstream *)outFile)->close();
    string tempFileName = "temp_" + outFileName;

    ifstream inFile(outFileName.c_str());
    ofstream tempOutFile(tempFileName.c_str());

    // Read in the contents of the file and write to the new file
    // until the position has been reached.
    char transfer;
    while(inFile.tellg() < pos){
      inFile.get(transfer);
      tempOutFile.put(transfer);
    }

    inFile.close();
    tempOutFile.close();

    // Remove the original file and rename the new file to the old
    // one.
    remove(outFileName.c_str());
    rename(tempFileName.c_str(), outFileName.c_str());
    remove(tempFileName.c_str());

    outFile = new ofstream(outFileName.c_str(), ios::app);
  }
}

void 
FileQueue::fossilCollect(const VTime& fossilCollectTime){

   multiset< FileData >::iterator iter_begin = begin();
   multiset< FileData >::iterator iter_end = end();
   
   while(iter_begin != iter_end){
      if (iter_begin->getTime() < fossilCollectTime){
         outFile->seekp(0, ios::end);
         const string *line = iter_begin->getLine();
         
         *outFile << *line;
         // this is a little trick using iterators.
         // the old value of iter_begin is saved and passed to erase
         // AFTER the iterator has been incremented.
         delete line;
         delete &(iter_begin->getTime());
         erase(iter_begin++);
      }
      else {
         break;
      }
   }
}

void
FileQueue::fossilCollect(int fossilCollectTime){

   multiset< FileData >::iterator iter_begin = begin();
   multiset< FileData >::iterator iter_end = end();

   while(iter_begin != iter_end){
      if (iter_begin->getTime().getApproximateIntTime() < fossilCollectTime){
         outFile->seekp(0, ios::end);
         const string *line = iter_begin->getLine();

         *outFile << *line;
         // this is a little trick using iterators.
         // the old value of iter_begin is saved and passed to erase
         // AFTER the iterator has been incremented.
         delete line;
         delete &(iter_begin->getTime());
         erase(iter_begin++);
      }
      else {
         break;
      }
   }
}

void
FileQueue::rollbackTo(const VTime& rollbackToTime){

   FileData findElement(rollbackToTime);
   multiset< FileData >::iterator iter_find = lower_bound(findElement);
   multiset< FileData >::iterator iter_end = end();
   // delete every thing after (and including) the found element
   while(iter_find != iter_end){
      delete iter_find->getLine();
      delete &(iter_find->getTime());
      erase(iter_find++);
   }
}

void
FileQueue::storeLine(const VTime &time, string *line){
   FileData input(time, line);
   insert(input);
}
