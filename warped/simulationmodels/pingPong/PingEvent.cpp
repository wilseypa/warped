// Copyright (c) The University of Cincinnati.  
// All rights reserved.

// UC MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF 
// THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  UC SHALL NOT BE LIABLE
// FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
// RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
// DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Malolan Chetlur             mal@ececs.uc.edu
//          Jorgen Dahl                 dahlj@ececs.uc.edu
//          Dale E. Martin              dmartin@ececs.uc.edu
//          Radharamanan Radhakrishnan  ramanan@ececs.uc.edu
//          Dhananjai Madhava Rao       dmadhava@ececs.uc.edu
//          Philip A. Wilsey            phil.wilsey@uc.edu

//---------------------------------------------------------------------------
// 
// $Id: PingEvent.cpp
// 
//---------------------------------------------------------------------------

#include "PingEvent.h"
#include <cstdlib>
#include <warped/SerializedInstance.h>
#include <warped/ObjectID.h>

void 
PingEvent::serialize( SerializedInstance *addTo ) const {
  Event::serialize( addTo );
  addTo->addString( myOwner );
}

Serializable *
PingEvent::deserialize( SerializedInstance *instance ){
  VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
  VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
  unsigned int senderSimManID = instance->getUnsigned();
  unsigned int senderSimObjID = instance->getUnsigned();
  unsigned int receiverSimManID = instance->getUnsigned();
  unsigned int receiverSimObjID = instance->getUnsigned();
  unsigned int eventId = instance->getUnsigned();
  string owner = instance->getString();

  ObjectID sender(senderSimObjID, senderSimManID);
  ObjectID receiver(receiverSimObjID, receiverSimManID);

  PingEvent *event = new PingEvent( *sendTime,
				    *receiveTime,
				    sender,
				    receiver,
				    eventId,
				    owner );
  delete sendTime;
  delete receiveTime;

  return event;
}

bool
PingEvent::eventCompare( const Event* event ){
  if( compareEvents( this, event ) == true ){
    if( myOwner == dynamic_cast<const PingEvent *>(event)->getOwner() ){
      return true;
    }
  }
  return false;
}
