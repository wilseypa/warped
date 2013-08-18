#ifndef FILE_QUEUE_H
#define FILE_QUEUE_H


#include "warped.h"
#include <fstream>
#include <cstdlib>
#include <set>
#include "FileData.h"

using std::multiset;
using std::ios;
using std::cerr;
using std::cout;
using std::endl;

/** The FileQueue class.

    The FileQueue class represents a queue of output strings that will
    eventually be written into a file. Due to the TimeWarp semantics,
    we can't commit an irreversible transaction (sucn as a write to a
    file) until the time of the transaction is less than GVT. Till
    such time, we have to store the output (and possibly rollback to
    correct erroneous output as well).

    The FileQueue class is derived from the STL multiset data
    structure since the multiset supports the operations and methods
    commonly used in queues.
*/
class FileQueue : public multiset< FileData > {
public:
   
   /**@name Public Class Methods of FileQueue. */
   //@{

   /// Default Constructor that takes a fileName
   FileQueue(const string &fileName);

   /** Constructor that takes a flag.

       Use of this constructor implies that output is sent to the screen.

   */
   FileQueue(int standardOutFlag);

   /// Default destructor
   ~FileQueue();

   /// open a particular file
   void open(const string &fileName);

   /// open a particular file with a given mode
   void open(const string &fileName, ios::openmode);

   /// gets the current position of the end of the output file.
   std::streampos getEOFPosition();

   /// sets the put position and clears all data following the position.
   void setPosAndClear(std::streampos pos);

   /// store a line in the file queue
   void storeLine(const VTime &currentTime, string *currentLine);
   
   /// fossil collect all entries with timestamps less than fossilCollectTime
   void fossilCollect(const VTime& fossilCollectTime);
   
   /// fossil collect all entries with timestamps less than fossilCollectTime
   /// only called with optimistic fossil collection
   void fossilCollect(int fossilCollectTime);

   /// rollback to rollback time
   void rollbackTo(const VTime& rollbackTime);

   /// close this file queue
   void close();
   
   //@} // End of Public Class Methods of FileQueue.

private:

   /**@name Private Class Attributes of FileQueue. */
   //@{

   /** flag for writing to file or to the screen.

       We instantiate a out file queue for writing output to stdout.  This
       is done as follows: FileQueue is newed with the constructor having
       the argument FileQueue::standardOut.  This tells the file queue to
       output all the data that it receives to stdout and not to a file.
       The file id is also accessed as FileQueue::standardOut.

   */
   static int standardOut;

   /// name of the file to write to
   string outFileName;

   /// the output file stream
   std::ostream *outFile;
   
   /// to determine if it is a normal output file or pipe to stdout
   int statusBit;

   //@} // End of Private Class Attributes of FileQueue.
   
};
#endif
