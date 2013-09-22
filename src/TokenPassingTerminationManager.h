#ifndef TOKEN_PASSING_TERMINATION_MANAGER_H
#define TOKEN_PASSING_TERMINATION_MANAGER_H


#include <TerminationManager.h>
class TerminateToken;
class TimeWarpSimulationManager;
class KernelMessage;

/**
   Defines a termination manager that uses token passing to determine if a
   simulation has completed or not.

   The basic alogorithm follows.  One simulation manager is the
   "terminator", the rest are slaves.  When the simulation manager that is
   terminator becomes idle, it starts a TerminateToken in circulation.  The
   token starts out colored white.  The token passes from SimulationManager
   to SimulationManager until it finds an idle one or it has been returned
   to the initiator.  If it makes it back to the initiator and it is still
   white, then it gets turned red and goes through another circulation.
*/

class TokenPassingTerminationManager : public TerminationManager {
public:
  TokenPassingTerminationManager( TimeWarpSimulationManager *initSimulationManager );

  /**
     The simulation manager queries this method to see if we need to
     terminate the simulation or not.
  */
  bool terminateSimulation();

  /**
     Our simulation manager calls this to set our status as active.
  */
  void setStatusActive();

  /**
     Our simulation manager calls this to set our status as passive.
  */
  void setStatusPassive();

  /**
     Reset the termination manager. Used for optimistic fossil collection rollbacks.
  */
  void ofcReset();

  void receiveKernelMessage( KernelMessage *msg );

private:
  /**
     Our "terminator status.
  */
  enum TERMINATOR_STATUS { SLAVE = 0x0, /// I am a slave
	 TERMINATOR_IDLE = 0x1, /// I'm the terminator and currently no token
	 /// is circulating
	 TERMINATOR_PROCESSING = 0x3 /// I'm the terminator and I'm waiting for a token
  } terminatorStatus;
  
  /**
     Returns true if I'm the terminator, else false.
  */
  bool amTerminator(){ return terminatorStatus && 0x1; }

  /**
     When we receive a termination token, this is how we handle it.
  */
  void handleTerminateToken( const TerminateToken *token );

  /**
     This method handles the sending a termination token.
  */
  void sendTerminateToken();

  void circulateToken( const TerminateToken *toCirulate );
  
  void sendToken( const TerminateToken *toSend );
		      
  /**
     If we've been active since the last token we saw, we'll cancel the
     cycle via this method.  It turns the token black and sends it to the
     terminator.  In turn, they'll retire and make us terminator.
  */
  void taintToken( const TerminateToken *toCirulate );

  /**
     If we're master and we became active in this cycle, we use this method
     to reset.
  */
  void cancelCycle( const TerminateToken *cancelToken );
  
  void simulationComplete();

  /**
     This gets called when we're master and someone else reported back that
     they aren't idle.  We relegate them to terminator by bouncing them a
     black token back with their id set as terminator.
  */
  void retireTerminator( const TerminateToken *canceledToken );

  void becomeTerminator( const TerminateToken *canceledToken );
  
  static int figureOutPeer( const TimeWarpSimulationManager *simManager );

  static TERMINATOR_STATUS getTerminatorStatus( const TimeWarpSimulationManager *simManager );

  /**
     A shortcut to get our id.
  */
  unsigned int getId() const;

  /**
     Our simulation manager.
  */
  TimeWarpSimulationManager *mySimulationManager;

  /**
     This is the peer that we pass tokens to.
  */
  const unsigned int myPeer;

  /**
     This flag gets set when the simulation has completed.
  */
  bool simulationCompleteFlag;

  /**
     Where we note our simulation manager's status.
  */
  SIM_MGR_IDLE_STATUS mySimManagerStatus;
  
  /**
     Have we been idle since the last terminate token that we saw?
  */
  bool idleSinceLastToken() const { return idleSinceLastTokenFlag; }

  bool idleSinceLastTokenFlag;
};

#endif
