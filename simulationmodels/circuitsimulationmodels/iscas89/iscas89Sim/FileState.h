#ifndef FILE_STATE_H
#define FILE_STATE_H

#include <FlatState.h>

class FileState : public FlatState {
public:
	/**@name Public Class Methods of FileState*/
	//@{
  FileState():
  maxNumLines(0),
  numLinesProcessed(0){}
  
  ~FileState(){};
  
  const int getSize() const {
    return sizeof(FileState);	
  }
  
  unsigned int getStateSize() const {/*return sizeof(FileState);*/};

  //@} // End of Public Class Methods of FileState.
  //**@name Public Class Attributes of FileState. */
  //@{
  
  /// total lines which should be read or written 
  int maxNumLines;
  /// number of lines which are read or written

  int numLinesProcessed;
  //@} // End of Public Class Attributes of FileState.
};

#endif 
