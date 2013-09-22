#ifndef EVENT_MESSAGE_BASE_TEST_H
#define EVENT_MESSAGE_BASE_TEST_H

#include "KernelMessageTest.h"
#include "DefaultEvent.h"
#include "IntVTime.h"

class EventMessageBase;

class EventMessageBaseTest : public KernelMessageTest {
  CPPUNIT_TEST_SUITE( EventMessageBaseTest );
  CPPUNIT_TEST_SUITE_END();

public:
  class EventMessageBaseTestEvent : public DefaultEvent {
  public:
    static VTime &getDefaultTime(){
      static IntVTime retval( 12 );
      return retval;
    }
    
    EventMessageBaseTestEvent( ) : DefaultEvent( getDefaultTime(),
						 getDefaultTime(),
						 ObjectID(0,0),
						 ObjectID(0,0),
                                                 0 ){}
    ~EventMessageBaseTestEvent(){}

    unsigned int getEventSize() const { return sizeof(EventMessageBaseTestEvent); }
    
    static const string &getEventMessageBaseTestEventDataType(){
      static string retval = "EventMessageBaseTestEvent";
      return retval;
    }
    const string &getDataType() const { return getEventMessageBaseTestEventDataType(); }
    
    void serialize( SerializedInstance * ) const {  }
    
    static Serializable *deserialize( SerializedInstance * ){
      return new EventMessageBaseTestEvent;
    }
    
    bool eventCompare( const Event * ){
      return true;
    }
    
  };

  void testConstructor( const EventMessageBase * );
  void testSerialization( const EventMessageBase *, const string &messageType );

protected:
  static const string getDefaultGVTInfo();
};

#endif
