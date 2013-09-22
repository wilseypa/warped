#ifndef EVENT_TEST_H
#define EVENT_TEST_H

#include "DefaultEvent.h"
#include "SerializableTest.h"
#include "SerializedInstance.h"
#include "IntVTime.h"

class EventTest : public SerializableTest {
  class EventTestTestEvent : public DefaultEvent {
  public:
    const ObjectID &getDefaultSender(){
      static ObjectID defaultSender(0,0);
      return defaultSender;
    }
    const ObjectID &getDefaultReceiver(){
      static ObjectID defaultReceiver(0,1);
      return defaultReceiver;
    }
    
    static const string &getEventTestTestEventType(){
      static string testEventType = "EventTestTestEvent";
      return testEventType;
    }

    unsigned int getEventSize() const { return sizeof(EventTestTestEvent); }
    
    const string &getDataType() const { return getEventTestTestEventType(); }
    
    bool eventCompare( const Event *other ){
      return Event::compareEvents( this, other );
    }
    
    EventTestTestEvent() : DefaultEvent( IntVTime::getIntVTimeZero(),
					 IntVTime::getIntVTimePositiveInfinity(),
					 getDefaultSender(),
					 getDefaultReceiver(),
                                         getNextId() ){}

    

    static Serializable *deserialize( SerializedInstance *instance ){
      Serializable *retval = 0;

      const VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
      const VTime *recvTime = dynamic_cast<VTime *>(instance->getSerializable());
      unsigned int senderSimManID = instance->getUnsigned();
      unsigned int senderSimObjID = instance->getUnsigned();
      unsigned int receiverSimManID = instance->getUnsigned();
      unsigned int receiverSimObjID = instance->getUnsigned();
      unsigned int eventId = instance->getUnsigned();

      ObjectID receiver(receiverSimObjID, receiverSimManID);
      ObjectID sender(senderSimObjID, senderSimManID);

      retval = new EventTestTestEvent( *sendTime, 
				       *recvTime, 
				       sender, 
				       receiver, 
				       eventId, 
				       "" );

      return retval;
    }

    static int getNextId(){
      static int idVal1 = 0;
      return idVal1++;
    }

  private:
    EventTestTestEvent( const VTime &sendTime,
			const VTime &recvTime,
			const ObjectID &sender,
			const ObjectID &receiver,
			const unsigned int eventId,
			const string &myString ) : 
      DefaultEvent( sendTime, recvTime, sender, receiver, eventId ),
      privateString(myString){}    
    
    const string privateString;
};


  CPPUNIT_TEST_SUITE( EventTest );
  CPPUNIT_TEST( testSerialization );
  CPPUNIT_TEST( testEventIds );
  CPPUNIT_TEST_SUITE_END();

public:
  // Test interface
  void setUp();
  void tearDown();

  void testConstructor();
  void testSerialization();
  void testEventIds();
  void testSerialization( const Event *toTest, const string &dataType );

};

#endif
