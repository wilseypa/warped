#ifndef OBJECT_ID_H
#define OBJECT_ID_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <iostream>

/** The class ObjectID.

    In defining this revision of {\tt WARPED}, the developers had long
    discussions about generating unique simulation object ids.  {\tt
    ObjectID} is an interesting part of the system, as it is fundamental
    to the discrete "event" concept and can have a profound impact on both
    ease of use and performance.  Many tradeoffs can be made with respect
    to {\tt ObjectID}. Some of the discussion and tradeoffs have been
    summarized in here.  The definition that the developers decided on for
    {\tt ObjectID} is a compromise of flexibility and performance. 

    A simple integer definition for simulation object ids would suffice
    for many non-distributed applications, and would provide excellent
    performance. If we were running a uniprocessor simulation, it would be
    trivial to ensure that unique object ids are issued to every object
    that registers itself with the Simulation Manager.  However, if we
    were running a distributed simulation, it is no longer possible to
    ensure that a unique integer id is generated throughout the
    distributed simulation (due to the distributed nature of the problem).
    In order to address this problem, a two tuple of integers is used as
    the simulation object id. A tuple can still provide good performance
    and offers a higher degree of flexibility than a simple integer. The
    first member of the two tuple denotes the simulation object's id while
    the second member denotes the Simulation Manager's id. While the
    simulation object id by itself may not be unique across the
    simulation, the two tuple is guaranteed to be unique.

    In previous versions of {\tt WARPED}, the job of ensuring that unique
    ids were assigned to simulation objects was left to the application
    writer/user.  In this version of {\tt WARPED}, we are changing this
    rule.  Unique simulation object id generation will be handled by the
    simulation kernel. The application writer/user assigns names to
    simulation objects and uses these names to refer to objects.  The
    mapping from object names to their unique object ids is handled by the
    kernel.  In addition, depending upon the type of the Simulation
    Manager that is instantiated at run-time ({\tt i.e.},
    SequentialSimulationManager, DecentralizedSimulationManager, or
    SharedMemorySimulationManager), either unsigned integers will be used
    as simulation object ids or the two tuple version of simulation object
    ids will be used.

    This class provides the two tuple definition of ObjectID.  
*/
class ObjectID {
public:
   /**@name Public Class Methods of ObjectID. */
   //@{

   /** Constructor.
      
       @param simObjID Id of the simulation object.
       @param simMgrID Id of the simulation manager that contains this object.
   */
   inline ObjectID( unsigned int simObjID, unsigned int simMgrID ) : 
      mySimulationObjectID( simObjID ), mySimulationManagerID( simMgrID ){}

   /** Constructor.
      
       Simulation Manager id is set to 0 by default.
      
       @param simObjID Simulation object id.
   */
   inline ObjectID( unsigned int simObjID ) : mySimulationObjectID(simObjID), 
      mySimulationManagerID(0){}

   /** Default Constructor.
      
       Simulation Manager id is set to 0 by default.
      
       @param simObjID Simulation object id.
   */
   inline ObjectID(): mySimulationObjectID(0), mySimulationManagerID(0){}

   /** Copy Constructor.
	
       @param init ObjectID object that is copied.
   */
   inline ObjectID( const ObjectID &init ) : 
      mySimulationObjectID( init.getSimulationObjectID() ), 
      mySimulationManagerID( init.getSimulationManagerID() ){}

   /** Get simulation object id.
	
       @return Simulation object id.
   */
   inline unsigned int getSimulationObjectID() const { return mySimulationObjectID; }

   /** Get simulation manager id.
	
       @return Simulation manager id.
   */
   inline unsigned int getSimulationManagerID() const { return mySimulationManagerID; }

   /// Overloaded operator
   inline const ObjectID& operator=( const ObjectID &from  ){
      mySimulationObjectID = from.getSimulationObjectID();
      mySimulationManagerID = from.getSimulationManagerID();
      return *this;
   }

   /// Overloaded operator
   inline const ObjectID operator+( const ObjectID &right ) const {
      return ObjectID(mySimulationObjectID + right.getSimulationObjectID(),
                       mySimulationManagerID + right.getSimulationManagerID());
   }

   /// Overloaded operator
   inline const ObjectID operator-( const ObjectID &right ) const {
      return ObjectID(mySimulationObjectID - right.getSimulationObjectID(),
                      mySimulationManagerID - right.getSimulationManagerID());
   }

   /// Overloaded operator
   inline bool operator<( const ObjectID &right ) const {
      if(mySimulationObjectID < right.getSimulationObjectID()) {
         return true;
      }
      else if(mySimulationObjectID == right.getSimulationObjectID() &&
              mySimulationManagerID < right.getSimulationManagerID()){
         return true;
      }
      else{
         return false;
      }
   }

   /// Overloaded operator
   inline bool operator>( const ObjectID &right ) const {
      if(mySimulationObjectID > right.getSimulationObjectID()){
         return true;
      }
      else if(mySimulationObjectID == right.getSimulationObjectID() &&
              mySimulationManagerID > right.getSimulationManagerID()){
         return true;
      }
      else{
         return false;
      }
   }

   /// Overloaded operator
   inline bool operator==( const ObjectID &right ) const {
      if( mySimulationObjectID == right.getSimulationObjectID() && 
          mySimulationManagerID == right.getSimulationManagerID() ){
         return true;
      }
      else{
         return false;
      }
   }

   /// Overloaded operator
   inline bool operator!=( const ObjectID &right ) const {
      return !operator==( right );
   }

   /// Overloaded operator
   inline bool operator<=( const ObjectID &right ) const {
      if( *this < right || *this == right ){
         return true;
      }
      else{
         return false;
      }
   }

   /// Overloaded operator
   inline bool operator>=( const ObjectID &right ) const {
      if( *this > right || *this == right ){
         return true;
      }
      else{
         return false;
      }
   }

   /// Destructor.
   inline ~ObjectID(){}

   //@} // End of Public Class Methods of ObjectID. 

private:

   /**@name Private Class Attributes of ObjectID. */
   //@{

   /// Simulation object id.
   unsigned int mySimulationObjectID;

   /// Simulation manager id.
   unsigned int mySimulationManagerID;

   //@} // End of Private Class Attributes of ObjectID.
};

inline 
std::ostream &
operator<<( std::ostream &os, const ObjectID &vt ){
  os << "(" << vt.getSimulationObjectID() << ", " 
     << vt.getSimulationManagerID() << ")";
   return os;
}

// hash function object for ObjectID
//This function object can be useful as a hash function while using stl
class hashObjectID {
public:
  size_t operator()(const ObjectID &objId) const {
    return size_t(objId.getSimulationManagerID()*10000 + objId.getSimulationObjectID());
  }
};

//Equal function object for ObjectID
//This is binary predicate, function object that can be useful
//while using stl
class EqualObjectID {
public:
  bool operator()(const ObjectID &lhs, const ObjectID& rhs) const {
    return (lhs == rhs);
  }
};

#endif
