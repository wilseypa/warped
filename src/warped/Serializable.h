#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H


class SerializedInstance;
#include <string>
using std::string;

/**
   Serialization interface.  Serializable classes should derive from this
   class.  They need to define a unique type name and register with the
   DeserializerManager.
*/
class Serializable {
public:
  /**
     This class' unique data type string.
  */
  virtual const string &getDataType() const = 0;

  /**
     Serialize this object.  Serialization happens in two phases, following
     the "gang of four" template method pattern.  First, we insert the data
     type, and then Serializable#serialize( const SerializedInstance * )
     gets called.  Derived classes overload that method to insert their
     derived data into the SerializedInstance.
  */
  SerializedInstance *serialize() const;

  /** Serialize this object.
      
  If the kernel needs a serialized instance of a Serializable object, it
  will call this method to do so.
  
  This is a pure virtual function and has to be overridden.
  
  @return Pointer to a SerializedInstance.  
  */
  virtual void serialize( SerializedInstance * ) const = 0;

  virtual ~Serializable() = 0;
};

inline Serializable::~Serializable(){}

#endif
