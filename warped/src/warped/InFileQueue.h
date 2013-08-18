#ifndef IN_FILE_QUEUE_H
#define IN_FILE_QUEUE_H


#include <fstream>
#include "warped.h"
#include "InFileData.h"
#include <set>
#include <string>

using std::string;
using std::multiset;
using std::fstream;

/** The InFileQueue class.

    The InFileQueue class represents a queue of file position records
    at different time instances so that during rollback we can
    correctly seek to the correct position in a file we are currently
    reading.

*/
class InFileQueue : public multiset< InFileData > {
public:

   /**@name Public Class Methods of InFileQueue. */
   //@{

   /// Default Constructor that takes a file name
   InFileQueue(const string &fileName);

   /// Destructor
   ~InFileQueue();

   /// Open a particular file
   void open(const string &fileName);

   /// fossil collect everything earlier than fossil collect time
   void fossilCollect(const VTime &fossilCollectTime);

   /// fossil collect everything earlier than fossil collect time
   /// only used with optimistic fossil collection
   void fossilCollect(int fossilCollectTime);

   /// rollback to the rollback time
   void rollbackTo(const VTime &rollbackTime);

   /// store the current position at the current time
   void storePos(const VTime &time);

   // sets the position of the file during the last fossil
   // collection or the optimistic fossil collection restore.
   void restoreFilePosition(streampos newPos);

   /// close the file queue
   void close();

   /// get a handle to the file stream
   fstream* access() {
      return &inFile;
   };

   //@} // End of Public Class Methods of InFileQueue.
  
private:

   /**@name Private Class Attributes of InFileQueue. */
   //@{

   /// name of the file
   string inFileName;

   /// the file stream
   fstream inFile;

   /// the position in the file at the last rollback.
   streampos lastFossilCollPos;

   //@} // End of Private Class Attributes of InFileQueue.
};
#endif

