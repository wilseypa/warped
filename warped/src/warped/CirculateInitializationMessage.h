#ifndef CIRCULATE_INITIALIZATION_MESSAGE_H
#define CIRCULATE_INITIALIZATION_MESSAGE_H

// See copyright notice in file Copyright in the root directory of this archive.

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
   communication managers at circulateInitialization time.
*/

class CirculateInitializationMessage : public KernelMessage {
public:
  CirculateInitializationMessage( unsigned int source,
				  unsigned int dest ) :
    KernelMessage( source, dest ){}
  
  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  static const string &getCirculateInitializationMessageType();

  const string &getDataType() const {
    return getCirculateInitializationMessageType();
  }

  static void registerDeserializer();

private:
  /**
     Default constructor - to be used only by the deserializer.
  */
  CirculateInitializationMessage(){};
};

#endif
