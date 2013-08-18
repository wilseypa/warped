#ifndef FILE_DATA_H
#define FILE_DATA_H


#include <string>

/** The FileData class.

    This class represents a container class that stores the current
    output to a file that is being written into.  Due to the TimeWarp
    semantics, we can't commit an irreversible transaction (sucn as a
    write to a file) until the time of the transaction is less than
    GVT. Till such time, we have to store the output (and possibly
    rollback to correct erroneous output as well).
*/
class FileData {
public:
   
   /**@name Public Class Methods of FileData.  */
   //@{

   /// Default constructor
   FileData(const VTime& currentTime) : time(*(currentTime.clone())), line(NULL){};

   /// Constructor that takes all the arguments
   FileData(const VTime& currentTime, string *lineString)
      : time(*(currentTime.clone())), line(lineString){};

   /// Destructor
   ~FileData() {
   };

   /// return the time of this entry
   const VTime &getTime() const {
      return time;
   }

   /// get the string that is stored in this entry
   const string *getLine() const {
      return line;
   }

   /// Overloaded operator <
   friend bool operator<(const FileData& a, const FileData& b){
      return a.time < b.time;
   }
  
   /// Overloaded operator ==
   friend bool operator==(const FileData& a, const FileData& b){
      return a.time == b.time;
   }
  
   /// Overloaded operator >
   friend bool operator>(const FileData& a, const FileData& b){
      return a.time > b.time;
   }
  
   /// Overloaded operator <<
   friend ostream& operator<<(ostream& os, const FileData& a){
      return os << a.time << " " << *(a.line);
   }
   
   //@} // End of Public Class Methods of FileData.

protected:
   
   /**@name Protected Class Attributes of FileData. */
   //@{

   /// needed to sort the entries in the FileQueue
   const VTime &time;

   /// data for writing
   const string *line; 

   //@} // End of Protected Class Attributes of FileData.
};

#endif
