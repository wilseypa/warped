#ifndef TERMINATE_TOKEN_H
#define TERMINATE_TOKEN_H


/**
   This is the class defining the termination token that gets passed
   around.  Essentially, it has a color and a terminator.  

   The color can be WHITE, which means it's on the first cycle,
   RED, which means it's on it's second cycle, or BLACK.
*/

class SerializedInstance;

#include "KernelMessage.h"

/**
   This class represents the token that gets passed between
   TokenPassingTerminationManager instances.

   @see TokenPassingTerminationManager
*/

class TerminateToken : public KernelMessage {
public:
  /**
     Type defining states of a token.
  */
  enum TokenState { FIRST_CYCLE, 
		    SECOND_CYCLE,
		    CANCEL_CYCLE,
		    BECOME_TERMINATOR,
		    SIMULATION_COMPLETE };

  TerminateToken( unsigned int source,
		  unsigned int dest,
		  unsigned int terminator,
		  TokenState state = TerminateToken::FIRST_CYCLE );

  TokenState getState() const { return myState; }
  unsigned int getTerminator() const { return myTerminator; }

  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  static const string &getTerminateTokenType();

  const string &getDataType() const {
    return getTerminateTokenType();
  }

  static void registerDeserializer();

private:
  /**
     Default constructor - to be used only by the deserializer.
  */
  TerminateToken(){};

  TokenState myState;
  unsigned int myTerminator;
};

#endif
