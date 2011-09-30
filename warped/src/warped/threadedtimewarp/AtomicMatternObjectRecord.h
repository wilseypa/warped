#ifndef ATOMIC_MATTERN_OBJECT_RECORD_H
#define ATOMIC_MATTERN_OBJECT_RECORD_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "warped.h"
#include "MatternObjectRecord.h"
#include <pthread.h>
/** The AtomicMatterObjectRecord class.
	This is an atomic version of the MatternObjectRecord class
*/
class AtomicMatternObjectRecord : public MatternObjectRecord {

private:
  /**@name Private Class Attributes of MatternObjectRecord. */
  //@{
  pthread_rwlock_t tMinLock;
  //@} // End of Private Class Attributes of MatternObjectRecord.

public:

  /**@name Public Class Methods of MatternObjectRecord. */
  //@{

  /// Constructor.
	AtomicMatternObjectRecord() : MatternObjectRecord() {
		pthread_rwlock_init(&tMinLock, NULL);
	}

  /// Destructor.
  ~AtomicMatternObjectRecord(){};

  /** Reset the minimal timestamp of red messages
  @param newTime the new time to reset it to
  */
  void resetMinTimeStamp(const VTime& newTime){
	  pthread_rwlock_wrlock(&tMinLock);
	  delete tMin;
	  tMin = newTime.clone();
	  pthread_rwlock_unlock(&tMinLock);
  }

  /** Set the minimal timestamp of red messages
  @param newTime the new time to set it to
  */
  void setMinTimeStamp(const VTime& newTime){
	  pthread_rwlock_wrlock(&tMinLock);
	  if (newTime < *tMin) {
		  delete tMin;
		  tMin = newTime.clone();
	  }
	  pthread_rwlock_unlock(&tMinLock);
  }

  /** Get the minimal timestamp of red messages.
  @return tMin the minimal timestamp of red messages
  */
  const VTime *getMinTimeStamp(){
	 // pthread_rwlock_rdlock(&tMinLock);
	//  VTime *returnCopy = tMin->clone();
	//  pthread_rwlock_unlock(&tMinLock);
	  return tMin;
  }

  /** Set the number of white messages.
  @param newValue the new value for number of white messages
  */
  void setNumberOfWhiteMessages(int newValue){
    numberOfWhiteMessages = newValue;
  }

  /// increment the token iteration number
  void incrementTokenIterationNumber(){
	  __sync_fetch_and_add(&tokenIterationNumber, 1);
  }

  /// increment the number of white messages
  void incrementNumberOfWhiteMessages(){
	  __sync_fetch_and_add(&numberOfWhiteMessages,1);
  }

  /// decrement the number of white messages
  void decrementNumberOfWhiteMessages(){
	  __sync_fetch_and_sub(&numberOfWhiteMessages,1);
  }
   
  //@} // End of Public Class Methods of MatternObjectRecord.
};
#endif
   
