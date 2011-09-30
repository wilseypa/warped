#ifndef CONFIG_VALUE_H
#define CONFIG_VALUE_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "StringUtilities.h"
#include <vector>
#include <string.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using std::vector;
using std::ostream;
using std::cerr;
using std::endl;

class ConfigurationValue {
public:
  enum ValueType { STRING, INT, DOUBLE, VECTOR };
  virtual const ValueType getType() const = 0;

  virtual const string &getStringValue() const { die("string"); return *new string(""); }
  virtual int getIntValue() const { die("int"); return -1; }
  virtual double getDoubleValue() const { die("double"); return -1.0; }
  virtual const vector<const ConfigurationValue *> *getVectorValue() const { die("vector"); return 0; }

  virtual ~ConfigurationValue() = 0;

private:
  void die( const string &requested ) const;
};

inline ConfigurationValue::~ConfigurationValue(){}

inline
ostream &
operator<<( ostream &os, const ConfigurationValue::ValueType input ){
  switch( input ){
  case ConfigurationValue::STRING:
    os << "string";
    break;
  case ConfigurationValue::INT:
    os << "int";
    break;
  case ConfigurationValue::DOUBLE:
    os << "double";
    break;
  case ConfigurationValue::VECTOR:
    os << "double";
    break;

  default:
    os << "invalid";
  }
  return os;
}

inline 
void 
ConfigurationValue::die( const string &requested ) const {
  cerr << "Asked for a value of type " << requested << " but I'm a " <<
    getType() << " config option." << endl;
  abort(); 
}

class StringConfigurationValue : public ConfigurationValue {
public:
  StringConfigurationValue( const string &value ) : myString( value ){}
  StringConfigurationValue( const char *value ) : myString( *new string( strdup(value)) ){}

  virtual ~StringConfigurationValue(){}

  const ValueType getType() const {
    return STRING;
  }

  const string &getStringValue() const { return myString; }

private:

  StringConfigurationValue( const StringConfigurationValue & ) : ConfigurationValue(),
								 myString(""){ abort(); }
  StringConfigurationValue operator=( const StringConfigurationValue & ){ abort(); }

  const string &myString;
};

class IntConfigurationValue : public ConfigurationValue {
public:
  IntConfigurationValue( const string &value ){
    int endPos;
    myInt = (int)stringToLong( value, endPos );
    if( endPos == 0 ){
      abort();
    }
  }

  virtual ~IntConfigurationValue(){}

  const ValueType getType() const {
    return INT;
  }

  int getIntValue() const { return myInt; }

private:
  IntConfigurationValue( const IntConfigurationValue & ) : ConfigurationValue(),
							   myInt( 0 ){ abort(); }
  IntConfigurationValue operator=( const IntConfigurationValue & ){ abort(); }

  int myInt;
};

class DoubleConfigurationValue : public ConfigurationValue {
public:
  DoubleConfigurationValue( const string &value ){
    int endPos;
    myDouble = stringToDouble( value, endPos );
    if( endPos == 0 ){
      abort();
    }
  }
  virtual ~DoubleConfigurationValue(){}
  
  const ValueType getType() const {
    return DOUBLE;
  }

  double getDoubleValue() const { return myDouble; }

private:
  DoubleConfigurationValue( const DoubleConfigurationValue & ) : ConfigurationValue(),
								 myDouble(0.0){ abort(); }
  DoubleConfigurationValue operator=( const DoubleConfigurationValue & ){ abort(); }

  double myDouble;
};

class VectorConfigurationValue : public ConfigurationValue {
public:
  VectorConfigurationValue( const vector<const ConfigurationValue *> *initVector ) : myVector(initVector){}
  virtual ~VectorConfigurationValue(){ delete myVector; }

  const ValueType getType() const {
    return VECTOR;
  }

  const vector<const ConfigurationValue *> *getVectorValue() const { return myVector; }

private:
  VectorConfigurationValue( const VectorConfigurationValue & ) : ConfigurationValue(),
								 myVector(0){ abort(); }

  VectorConfigurationValue operator=( const VectorConfigurationValue & ){ abort(); }

  const vector<const ConfigurationValue *> *myVector;
};

#endif
