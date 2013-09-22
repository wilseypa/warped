#ifndef WARPED_UNIT_TEST_H
#define WARPED_UNIT_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SimulationObjectAdapter.h"
#include "TimeWarpSimulationManagerAdapter.h"
#include "DefaultEvent.h"
#include "DefaultTimeWarpEventSet.h"
#include "State.h"

/**
   An abstract class that other tests can derive from.  Tries to set up a
   minimal but "sane enough" environment for unit testing chunks of the
   warped kernel.
*/

class WarpedUnitTest : public CppUnit::TestFixture {
public:
  WarpedUnitTest() : simObj1(0), simMgr(0) {}

  /**
     Set up our minimal environment.  Automagically called prior to each
     test case by CPPUnit.
  */
  void setUp(){
    simObj1 = allocateSimObject( getSimObj1Name() );
    simMgr =  allocateTimeWarpSimulationManager( simObj1 );
  }
  
  /**
     Destructs our minimal environment.  Automagically called after each
     test case by CPPUnit.
  */
  void tearDown(){
    // Note that simMgr kills the object(s) for us.
   // delete simMgr; RYAN
    simMgr = 0;
  }

  /**
     Factory method that test cases can use to override the construction of
     the SimulationObjects...
  */
  virtual SimulationObject *allocateSimObject( const string &name ){
    return new UnitTestSimulationObject( name );
  }

  /**
     Factory method that test cases can use to override the construction of
     TimeWarpSimulationManagers.
  */
  virtual TimeWarpSimulationManager *
  allocateTimeWarpSimulationManager( SimulationObject *initObject ){
    return new UnitTestSimulationManager( initObject );
  }

  SimulationObject *getSimObj1(){
    return simObj1;
  }

  TimeWarpSimulationManager *getSimManager(){
    return simMgr;
  }
  
  static const ObjectID &getDefaultSender(){
    static ObjectID retval(0,0);
    return retval;
  }
  
  class UnitTestEvent : public DefaultEvent {
  public:
    UnitTestEvent( const VTime &recvTime,
		   const ObjectID &receiver ) :
      DefaultEvent( IntVTime::getIntVTimeZero(),
		    recvTime,
		    getDefaultSender(),
                    receiver,
                    getNextId() ){}

    UnitTestEvent( const VTime &recvTime,
		   const ObjectID &receiver,
                   const VTime &sendTime ) :
      DefaultEvent( sendTime,
		    recvTime,
		    getDefaultSender(),
		    receiver,
                    getNextId() ){}

    unsigned int getEventSize() const {return sizeof(UnitTestEvent); }

    static const string &getUnitTestEventType(){
      static string testEventType = "UnitTestEvent";
      return testEventType;
    }

    const string &getDataType() const { return getUnitTestEventType(); }

    bool eventCompare( const Event *compareTo ){ return DefaultEvent::compareEvents( this, compareTo ); }

    void serialize( SerializedInstance *addTo ) const {
      Event::serialize( addTo );
    }

    static Serializable *deserialize( SerializedInstance *instance ) {
      VTime *sendTime = dynamic_cast<VTime *>(instance->getSerializable());
      VTime *receiveTime = dynamic_cast<VTime *>(instance->getSerializable());
      unsigned int senderSimManID = instance->getUnsigned();
      unsigned int senderSimObjID = instance->getUnsigned();
      unsigned int receiverSimManID = instance->getUnsigned();
      unsigned int receiverSimObjID = instance->getUnsigned();
      unsigned int eventId = instance->getUnsigned();
 
      ObjectID receiver(receiverSimObjID, receiverSimManID);

      UnitTestEvent *event = new UnitTestEvent( *receiveTime, receiver, *sendTime );

      delete sendTime;
      delete receiveTime;

      return event;
    }
    
    static int getNextId(){
      static int idVal1 = 0;
      return idVal1++;
    }
  };

  class UnitTestState : public State {
  public:
     UnitTestState() : strMember( "UnitTestState" ), intMember(1) {}
     ~UnitTestState() {}

     void copyState( const State *toCopy ){
        ASSERT( toCopy != 0 );
        const UnitTestState *testState = dynamic_cast<const UnitTestState *>(toCopy);
        strMember = testState->strMember;
        intMember = testState->intMember;
     }

     unsigned int getStateSize() const { return sizeof(UnitTestState); }

     string strMember;
     int intMember;
  };

  class UnitTestSimulationManager : public TimeWarpSimulationManagerAdapter {
  public:
    ~UnitTestSimulationManager(){
      delete obj1;
      obj1 = 0;
      delete eventSet;
    }

    UnitTestSimulationManager( SimulationObject *initObj1 ) {
      obj1 = initObj1;
      setNumberOfObjects(1);
      eventSet = new DefaultTimeWarpEventSet(this,false);
      obj1->setObjectID(new OBJECT_ID( 0, getSimulationManagerID()));
      obj1->setSimulationManager( this );
    }

    SimulationObject *getObjectHandle( const string &objectName ) const {
      SimulationObject *retval = 0;
      if( obj1->getName() == objectName ){
	retval = obj1;
      }
      else{
	std::cerr << "SimulationObject " << objectName << " unknown, aborting!" << std::endl;
	abort();
      }
      return retval;
    }

    SimulationObject *getObjectHandle( const ObjectID &objectID ) const {
      SimulationObject *retval = 0;
      if( *(obj1->getObjectID()) == objectID ){
        retval = obj1;
      }
      else{
        std::cerr << "SimulationObject ID " << objectID << " unknown, aborting!" << std::endl;
        abort();
      }
      return retval;
    }

    TimeWarpEventSet *getEventSetManager(){ return eventSet; }

    vector<string> *getSimulationObjectNames(){
      vector<string> *retval = new vector<string>;
      retval->push_back( obj1->getName() );
      return retval;
    }

    bool contains( const string &object ) const {
      bool retval = true;
      if(object != obj1->getName()){
        retval = false;
      }
      return retval;
    }

  private:
    SimulationObject *obj1;
    TimeWarpEventSet *eventSet;
  };

  class UnitTestSimulationObject : public SimulationObjectAdapter {
  public:
    UnitTestSimulationObject( const string &initName ) :
      myName( initName ),
      theState( new UnitTestState() ) {}
    ~UnitTestSimulationObject(){
      delete theState;
    }

    const string &getName() const { return myName; }
    State *allocateState() { return new UnitTestState(); }
    State *getState() { return theState; }

  private:
    const string myName;
    State *theState;
  };

  static const string &getSimObj1Name(){ static string retval = "SimObj1"; return retval; }
  static const ObjectID &getSimObj1ID(){ static ObjectID retval(0,0); return retval; }

  /**
     An event with zero receive time.
  */
  static Event *getEvent1();
  /**
     An event with zero receive time.
  */
  static Event *getEvent2();
  /**
     An event with 1 receive time.
  */
  static Event *getEvent3();
  /**
     An event with zero receive time.
  */
  static Event *getEvent4();

  static vector<Event *>  &getDefaultEvents();
  static void checkDefaultEvents( const Event *event1,
				  const Event *event2,
				  const Event *event3,
				  const Event *event4 );

  static void              doDefaultInsert( TimeWarpEventSet *toTest );

private:
  SimulationObject *simObj1;
  TimeWarpSimulationManager *simMgr;
  
};

#endif
