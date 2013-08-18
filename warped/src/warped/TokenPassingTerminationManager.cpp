
#include "TokenPassingTerminationManager.h"
#include "TerminateToken.h"
#include "TimeWarpSimulationManager.h"
#include "CommunicationManager.h"
#include <Debug/Debug.h>
using debug::debugout;

#include "TerminateToken.h"
#include "CommunicationManager.h"

TokenPassingTerminationManager::TokenPassingTerminationManager( TimeWarpSimulationManager *initSimulationManager ) : 
  terminatorStatus( getTerminatorStatus( initSimulationManager ) ),
  mySimulationManager( initSimulationManager ),
  myPeer( figureOutPeer( initSimulationManager ) ),
  simulationCompleteFlag( false ),
  idleSinceLastTokenFlag( true ){

  CommunicationManager *commMgr = mySimulationManager->getCommunicationManager();
  ASSERT( commMgr != 0 );

  commMgr->registerMessageType( TerminateToken::getTerminateTokenType(), this );
}

int 
TokenPassingTerminationManager::figureOutPeer( const TimeWarpSimulationManager *simManager ){
  int myPeer = -1;
  int myId = simManager->getSimulationManagerID();
  debug::debugout << "myId = " << myId << std::endl;
  debug::debugout << "getNumberOfSimulationManagers() = " << 
    (int)simManager->getNumberOfSimulationManagers() << std::endl;
  if( myId == (int)simManager->getNumberOfSimulationManagers() - 1 ){
    myPeer = 0;
  }
  else{
    myPeer = myId + 1;
  }

  ASSERT( myPeer != myId );

  return myPeer;
}

TokenPassingTerminationManager::TERMINATOR_STATUS
TokenPassingTerminationManager::getTerminatorStatus( const TimeWarpSimulationManager *simManager ){
  TERMINATOR_STATUS retval = SLAVE;

  if( simManager->getSimulationManagerID() == 0 ){
    retval = TERMINATOR_IDLE;
  }

  return retval;
}

bool
TokenPassingTerminationManager::terminateSimulation(){
  if( simulationCompleteFlag == true ){
    return true;
  }
  else{
    switch( terminatorStatus ){
      // If we're the slave or we've got a token in process, we'll simply
      // drop out of this loop.
    case SLAVE:
    case TERMINATOR_PROCESSING:
      break;
    case TERMINATOR_IDLE:
      sendTerminateToken();
      break;
    default:
      std::cerr << "Invalid terminatorStatus - " << terminatorStatus << std::endl;
      abort();
    }
    return false;
  }
}

void 
TokenPassingTerminationManager::sendTerminateToken(){
  TerminateToken *newToken = new TerminateToken( getId(),
						 myPeer,
						 getId() );
  ASSERT( terminatorStatus == TERMINATOR_IDLE );
  terminatorStatus = TERMINATOR_PROCESSING;
  circulateToken( newToken );
}

void
TokenPassingTerminationManager::circulateToken( const TerminateToken *token ){
  // So we need to send this token to our peer.
  TerminateToken *newToken = new TerminateToken( getId(),
						 myPeer,
						 token->getTerminator(),
						 token->getState() );
  sendToken( newToken );
  delete token;
}

void
TokenPassingTerminationManager::sendToken( const TerminateToken *token ){
  ASSERT( token->getReceiver() != getId() );
  mySimulationManager->getCommunicationManager()->sendMessage( const_cast<TerminateToken *>(token), token->getReceiver() ); 
}

void 
TokenPassingTerminationManager::taintToken( const TerminateToken *toTaint ){
  // OK, so we're not idle and we need to taint this token and send it back.
  int sendBackTo = toTaint->getTerminator();
  TerminateToken *newToken = new TerminateToken( getId(),
						 sendBackTo,
						 sendBackTo,
						 TerminateToken::CANCEL_CYCLE );
  sendToken( newToken );
  delete toTaint;
}

void 
TokenPassingTerminationManager::cancelCycle( const TerminateToken *cancelToken ){
  delete cancelToken;
  terminatorStatus = TERMINATOR_IDLE;
}
  
void 
TokenPassingTerminationManager::simulationComplete(){
  simulationCompleteFlag = true;
  TerminateToken *token = new TerminateToken( getId(),
					      myPeer,
					      getId(),
					      TerminateToken::SIMULATION_COMPLETE );
  sendToken( token );
}

void 
TokenPassingTerminationManager::retireTerminator( const TerminateToken *canceledToken ){
  ASSERT( getId() == canceledToken->getTerminator() );

  terminatorStatus = SLAVE;

  unsigned int newTerminator = canceledToken->getSender();
  TerminateToken *reply = new TerminateToken( getId(),
					      newTerminator,
					      newTerminator,
					      TerminateToken::BECOME_TERMINATOR );
  sendToken( reply );
  delete canceledToken;
}

void 
TokenPassingTerminationManager::becomeTerminator( const TerminateToken *canceledToken ){
  terminatorStatus = TERMINATOR_IDLE;
  delete canceledToken;
}


void
TokenPassingTerminationManager::handleTerminateToken( const TerminateToken *token ){
  if( amTerminator() ){
    switch( token->getState() ){
    case TerminateToken::FIRST_CYCLE:{
      // We're on the first circulation and it made it back.  So far,
      // everyone is idle.
      if( idleSinceLastToken() ){
	TerminateToken *newToken = new TerminateToken( getId(),
						       myPeer,
						       getId(),
						       TerminateToken::SECOND_CYCLE );
	circulateToken( newToken );
	delete token;
      }
      else{
	// Oops, we've been active since we started this token.
	cancelCycle( token );
      }
      break;
    }
    case TerminateToken::SECOND_CYCLE:{
      // We're the master, the token came back the second time.
      if( idleSinceLastToken() ){
	// OK, the simulation is complete.
	simulationComplete();
      }
      else{
	// We became active while that token was circulating.  Let's cancel
	// the cycle again.
	cancelCycle( token );
      }
      break;
    }
    case TerminateToken::CANCEL_CYCLE:{
      // Someone else nixed the termination cycle.  We're passing the duty
      // on to them.
      retireTerminator( token );
      break;
    }
    case TerminateToken::SIMULATION_COMPLETE:{
      // Do nothing;
      break;
    }
    default:{
//       mySimulationManager->abortSimulation( "Invalid terminator token received, simulation corrupt" );
    }
    }
  }
  else{
    // We're not the terminator.
    switch( token->getState() ){
    case TerminateToken::FIRST_CYCLE:
    case TerminateToken::SECOND_CYCLE:{
      if( idleSinceLastToken() ){
	circulateToken( token );
      }
      else{
	// We're not idle so we'll "taint" the token.  This will turn it
	// black and send it back to the master.  He'll then designate us
	// terminator.
	taintToken( token );
      }
      break;
    }
    case TerminateToken::BECOME_TERMINATOR:{
      becomeTerminator( token );
      break;
    }
    case TerminateToken::SIMULATION_COMPLETE:{
      simulationComplete();
      break;
    }
    default:{
       std::cerr << "Invalid terminator token received, simulation corrupt" << std::endl;
       abort();
    }
    }
  }

  if(mySimManagerStatus == ACTIVE){
    idleSinceLastTokenFlag = false;
  }
  else{
    idleSinceLastTokenFlag = true;
  }
}

void 
TokenPassingTerminationManager::setStatusActive(){
  mySimManagerStatus = ACTIVE;
  idleSinceLastTokenFlag = false;
}

void
TokenPassingTerminationManager::setStatusPassive(){
  // The idleSinceLastTokenFlag remains false here to ensure
  // that active status can be detected properly.
  mySimManagerStatus = IDLE;
}

void 
TokenPassingTerminationManager::receiveKernelMessage( KernelMessage *msg ){
  TerminateToken *token = dynamic_cast<TerminateToken *>( msg );
  ASSERT( token != 0 );
  handleTerminateToken( token );
}

unsigned int
TokenPassingTerminationManager::getId() const {
  return mySimulationManager->getSimulationManagerID();
}

void
TokenPassingTerminationManager::ofcReset(){
  mySimManagerStatus = ACTIVE;
  idleSinceLastTokenFlag = false;
  simulationCompleteFlag = false;
  
  if( mySimulationManager->getSimulationManagerID() == 0 ){
    terminatorStatus = TERMINATOR_IDLE;
  }
  else{
    terminatorStatus = SLAVE;
  }
}
