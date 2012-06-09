#ifndef MATTERN_OBJECT_RECORD_H
#define MATTERN_OBJECT_RECORD_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"

// a token can be white, red, or uncolored
enum gVTTokenColor {WHITE, RED};

// are we sending or receiving
enum messageStatus {SEND, RECEIVED};

/** The MatterObjectRecord class.

The MatternObjectRecord class is the data structure that the
MatternGVTManager uses to keep track of sent/received messages of
each simulation manager in the distributed simulation. Currently
an array of these data structure is allocated by the
MatternGVTManager. The size of the array is equal to the number of
simulation managers in the simulation. Each entry in this array
contains the color of the simulation manager, the number of white
messages the simulation manager has sent, the number of iterations
the gvt token has taken (currently this is used only by the
initiator - simulation manager 0), and the minimal timestamp of
the red messages.

*/
class MatternObjectRecord {
public:

  /**@name Public Class Methods of MatternObjectRecord. */
  //@{

  /// Constructor.
  MatternObjectRecord():
    color( WHITE ),
    tokenIterationNumber(0),
    tMin( NULL ),
    numberOfWhiteMessages(0){}

  /// Destructor.
  ~MatternObjectRecord(){};

  /** Set the color

  @param newColor the new color type
  */
  virtual void setColor(gVTTokenColor newColor){
    color = newColor;
  }

  /** Reset the minimal timestamp of red messages

  @param newTime the new time to reset it to
  */
  virtual void resetMinTimeStamp(const VTime& newTime){
	  delete tMin;
	  tMin = newTime.clone();
  }

  /** Set the minimal timestamp of red messages

  @param newTime the new time to set it to
  */
  virtual void setMinTimeStamp(const VTime& newTime){
	  if (newTime < *tMin) {
		 // delete tMin;
		  tMin = newTime.clone();
	  }
  }

  /** Set the number of white messages.

  @param newValue the new value for number of white messages
  */
  virtual void setNumberOfWhiteMessages(int newValue){
    numberOfWhiteMessages = newValue;
  }

  /// increment the token iteration number
  virtual void incrementTokenIterationNumber(){
    tokenIterationNumber++;
  }

  /** return the token iteration number

  @return tokenIterationNumber the token iteration number
  */
  virtual unsigned int getTokenIterationNumber(){
    return tokenIterationNumber;
  }

  /** Set the token iteration number

  @param value the new value for the token iteration number
  */
  virtual void setTokenIterationNumber(unsigned int value){
    tokenIterationNumber = value;
  }

  /// increment the number of white messages
  virtual void incrementNumberOfWhiteMessages(){
    numberOfWhiteMessages++;
  }

  /// decrement the number of white messages
  virtual void decrementNumberOfWhiteMessages(){
    numberOfWhiteMessages--;
  }
   
  /** Get the color of this simulation manager.

  @return color the color type of this simulation manager
  */
  virtual gVTTokenColor getColor(){
    return color;
  }

  /** Get the number of white messages.

  @return numberOfWhiteMesssages the number of white messages
  */
  virtual int getNumberOfWhiteMessages(){
    return numberOfWhiteMessages;
  }

  /** Get the minimal timestamp of red messages.
  @return tMin the minimal timestamp of red messages
  */
  virtual const VTime *getMinTimeStamp(){
    return tMin;
  }
  //@} // End of Public Class Methods of MatternObjectRecord.
   
protected:

  /**@name Private Class Attributes of MatternObjectRecord. */
  //@{

  /// color of this simulation manager
  gVTTokenColor color;

  /// the number of white messages it has sent out
  int numberOfWhiteMessages;

  /// the number of iterations the gvttoken has taken
  unsigned int tokenIterationNumber;

  /// the minimal timestamp of red messages
  VTime *tMin;

  //@} // End of Private Class Attributes of MatternObjectRecord.
};
#endif
   
