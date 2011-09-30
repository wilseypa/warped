// See copyright notice in file Copyright in the root directory of this archive.

#include "TerminateToken.h"
#include "DeserializerManager.h"

TerminateToken::TerminateToken(	unsigned int source,
				unsigned int dest,
				unsigned int terminator,
				TokenState state )
  : KernelMessage( source,
		   dest ),
    myState( state ),
    myTerminator( terminator ){}

const string &
TerminateToken::getTerminateTokenType(){
  static string typeString( "TerminateToken" );
  return typeString;
}


void
TerminateToken::serialize( SerializedInstance *addTo ) const {
  KernelMessage::serialize( addTo );
  addTo->addUnsigned( myTerminator );
  addTo->addInt( myState );
}


Serializable *
TerminateToken::deserialize( SerializedInstance *data ){
  // Pull the data - have to do this outside of the constructor arguments
  // so that the data comes out ordered.
  unsigned int source = data->getUnsigned();
  unsigned int dest = data->getUnsigned();
  unsigned int incNum = data->getUnsigned();
  unsigned int terminator = data->getUnsigned();
  int state = data->getInt();

  TerminateToken *retval = new TerminateToken( source,
                                               dest,
                                               terminator,
                                               TerminateToken::TokenState( state ) );
  retval->setIncarnationNumber(incNum);
  return retval;
}

void 
TerminateToken::registerDeserializer(){
  DeserializerManager::instance()->registerDeserializer( getTerminateTokenType(),
							 &deserialize );
}
