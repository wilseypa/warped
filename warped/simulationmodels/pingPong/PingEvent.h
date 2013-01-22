#ifndef PINGEVENT_HH
#define PINGEVENT_HH

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

#include <warped/DefaultEvent.h>
#include <new>

class PingEvent : public DefaultEvent {
public:
  /**
     Constructor called by application.
  */
  PingEvent( const VTime &initSendTime,
	     const VTime &initRecvTime,
	     SimulationObject *initSender,
	     SimulationObject *initReceiver,
	     const string &owner ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver ),
    myOwner( owner ){}

  
  ~PingEvent(){}

  unsigned int getEventSize() const { return sizeof(PingEvent); } 
  
  static Serializable *deserialize( SerializedInstance *instance );

  void serialize( SerializedInstance * ) const;
  
  bool eventCompare(const Event* event);

  static const string &getPingEventDataType(){
    static string pingEventDataType = "PingEvent";
    return pingEventDataType;
  }
  
  const string &getDataType() const {
    return getPingEventDataType();
  }

  const string &getOwner() const { return myOwner; }

private:
  /*
     Constructor called by deserializer.
  */
  PingEvent( const VTime &initSendTime,
	     const VTime &initRecvTime,
	     const ObjectID &initSender,
	     const ObjectID &initReceiver,
	     const unsigned int eventIdVal,
	     const string &owner ) : 
    DefaultEvent( initSendTime, initRecvTime, initSender, initReceiver, eventIdVal ),
    myOwner( owner ){}


  const string myOwner;
};

#endif
