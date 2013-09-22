#ifndef INTVTIME_H
#define INTVTIME_H


#include <warped.h>
#include <iostream>
#include <string>
#include <Serializable.h>
#include <VTime.h>

class IntVTime : public VTime {

public:

  /**@name Public Class Methods of IntVTime. */
  //@{

  /** Constructor.
      
      @param major Major IntVTime.
      @param minor Minor IntVTime.
  */
  inline IntVTime ( int initTime ) : myTime( initTime ){}

  inline IntVTime (const IntVTime &init)
    : VTime(), myTime(init.getTime()){}

  /// Destructor.
  inline ~IntVTime(){}

  /** Get Major IntVTime.
	
      @return Major IntVTime.
  */
  inline int getTime() const { return myTime; }

  inline VTime *clone () const { return new IntVTime( myTime ); }

  // Overloaded operators.
  // These are called when IntVTime objects are used through VTime&
  // references.
  const VTime& operator= (const VTime &from);
  bool operator< (const VTime &right) const;
  bool operator> (const VTime &right) const;
  bool operator== (const VTime &right) const;
  bool operator!= (const VTime &right) const;
  bool operator<= (const VTime &right) const;
  bool operator>= (const VTime &right) const;
  
  // FIXME: operator+ is impossible for abstract classes!!!
  //const VTime operator+ (const VTime &right) const;
  //const VTime operator- (const VTime &right) const;

  // Overloaded operators.
  // These are used when IntVTime objects are accessed directly or
  // through IntVTime& references
  const IntVTime& operator= (const IntVTime&);
  bool operator< (const IntVTime&) const;
  bool operator> (const IntVTime&) const;
  bool operator== (const IntVTime&) const;
  bool operator!= (const IntVTime&) const;
  bool operator<= (const IntVTime&) const;
  bool operator>= (const IntVTime&) const;

  const IntVTime operator+ (const IntVTime &right) const;
  const IntVTime operator- (const IntVTime &right) const;
  const IntVTime operator+ (int) const;
  const IntVTime operator- (int) const;

  static const VTime &getIntVTimeZero();
  static const VTime &getIntVTimePositiveInfinity();

  /** Get definition of zero.

      @return IntVTime definition of Zero.
  */
  const VTime &getZero() const { return getIntVTimeZero(); }

  /** Get definition of Positive Infinity.
	
      @return IntVTime definition of Positive Infinity.
  */
  const VTime &getPositiveInfinity() const { return getIntVTimePositiveInfinity(); }

  /**
     Unhide Serializable#serialize
  */
  const SerializedInstance *serialize() const {
    return Serializable::serialize();
  }

  void serialize( SerializedInstance * ) const;

  static Serializable *deserialize( SerializedInstance * );

  static void registerDeserializer();

  static const string &getIntVTimeDataType() {
    static const string dataType = "IntVTimeDataType";
    return dataType;
  }

  /**
     @see Serializable#getDataType
  */
  const std::string &getDataType() const {
    return getIntVTimeDataType();
  }

  const string toString() const ;
  
  const warped64_t getApproximateIntTime() const {
	  return myTime;
  }

  //@} // End of Public Class Methods of IntVTime. 

private:

  /**@name Private Class Methods of IntVTime. */
  //@{
  
  /// Default Constructor.
  IntVTime() : myTime(0){}

  // Prevent IntVTimes from being allocated or deleted on stack.

  //@} // End of Private Class Methods of IntVTime.

  /**@name Private Class Attributes of IntVTime. */
  //@{
  int myTime;

  //@} // End of Private Class Attributes of IntVTime.

};


#endif
