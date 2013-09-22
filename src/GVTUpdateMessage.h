#ifndef GVT_UPDATE_MESSAGE_H
#define GVT_UPDATE_MESSAGE_H


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
   communication managers at gVTUpdate time.
*/

class GVTUpdateMessage : public KernelMessage {
public:
  GVTUpdateMessage( unsigned int source,
		    unsigned int dest,
		    const VTime &initNewGVT ) :
    KernelMessage( source, dest ),
    newGVT( initNewGVT.clone() ){}

  
  void serialize( SerializedInstance * ) const;
  static Serializable *deserialize( SerializedInstance *data );

  static const string &getGVTUpdateMessageType();

  const string &getDataType() const {
    return getGVTUpdateMessageType();
  }

  static void registerDeserializer();

  const VTime &getNewGVT() const { 
    ASSERT( newGVT != 0 );
    return *newGVT; 
  }

  ~GVTUpdateMessage(){
    delete newGVT;
  }

private:
  /**
     The new GVT value.
  */
  const VTime *newGVT;
};

#endif
