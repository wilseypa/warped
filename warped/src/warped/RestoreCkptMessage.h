#ifndef RESTORECKPTMESSAGE_H
#define RESTORECKPTMESSAGE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "KernelMessage.h"

class SerializedInstance;

/** The RestoreCkptMessage class.

    This class is used in the OptFossilCollManager to coordinate
    recovery from catastrophic rollbacks.
*/
class RestoreCkptMessage : public KernelMessage {
public:
  /**@name Public Class Methods of RestoreCkptMessage. */
  //@{

  /// Indicates the cycle that the recovery process is in.
  enum RestoreState{SEND_TO_MASTER,
                    FIRST_CYCLE,
		    SECOND_CYCLE,
		    THIRD_CYCLE};

  /** Constructor

      @param source The source simulation manager id.
      @param dest The destination simulation manager id.
      @param initCkptTime The first time to checkpoint.
      @param initState Initializes the cycle.
      @param initConsensus Initializes if consensus has been reached.
  */
  inline RestoreCkptMessage(unsigned int source,
                            unsigned int dest,
                            int initCkptTime = 0,
                            RestoreState initState = SEND_TO_MASTER,
                            bool initConsensus = false) :
    KernelMessage( source, dest ),
    checkpointTime(initCkptTime),
    tokenState(initState),
    checkpointConsensus(initConsensus){
  }
  
  /// Destructor.
  ~RestoreCkptMessage(){}

  /** Sets the checkpoint time.
      @param time The checkpoint time.
  */
  void setCheckpointTime(int time){ checkpointTime = time; }

  /** Set the current cycle.
      @param setState The cycle the recovery process is in.
  */
  void setTokenState(RestoreState setState){ tokenState = setState; }

  /** Set the restore time.
      @param ckptCon The time to restore.
  */
  void setCheckpointConsensus(bool ckptCon){ checkpointConsensus = ckptCon; }

  /** Returns the checkpoint time.
      @return int The checkpoint time.
  */
  int getCheckpointTime() const { return checkpointTime; }

  /** Returns the current cycle.
      @return the current cycle.
  */
  RestoreState getTokenState()const { return tokenState; }

  /** Returns the restore time.
      @return The restore time.
  */
  bool getCheckpointConsensus() const { return checkpointConsensus; }

  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  static const string &getRestoreCkptMessageType();

  const string &getDataType() const {
    return getRestoreCkptMessageType();
  }

  static void registerDeserializer();

  //@} // End of Public Class Methods of RestoreCkptMessage.

private:

  /**@name Private Class Methods of RestoreCkptMessage. */
  //@{

  ///  The estimate of that the LP is sending out
  int checkpointTime;

  /// The current cycle of the recovery process.
  RestoreState tokenState;

  /// The time to restore.
  bool checkpointConsensus;

  //@} // End of Private Class Methods of RestoreCkptMessage.
};

#endif
