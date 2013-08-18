#ifndef DESERIALIZER_MANAGER_H
#define DESERIALIZER_MANAGER_H

#include "warped.h"
#include<string>
using std::string;

#ifdef HAVE_TR1_UNORDERED_MAP
#include<tr1/unordered_map>
using std::tr1::unordered_map;
#else
#include <unordered_map>
using std::unordered_map;
#endif

class Deserializer;
class Serializable;
class SerializedInstance;

typedef Serializable *(*deserializeFunc)(SerializedInstance * );

class DeserializerManager {
public:
  /**
     Each serializable type should register with the DeserializerManager
     singleton before attempting to deserialize.  Typically a static code
     
  */
  void registerDeserializer( const string &dataType, deserializeFunc );

  deserializeFunc findDeserializer( const string &dataType );

  static DeserializerManager *instance();

protected:
  ~DeserializerManager(){}


private:
  unordered_map<string, deserializeFunc> deserializerMap;

  DeserializerManager(){}
};

#endif
