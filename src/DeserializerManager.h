#ifndef DESERIALIZER_MANAGER_H
#define DESERIALIZER_MANAGER_H

#include <string>                       // for string, hash
#include <unordered_map>                // for unordered_map

#include "Serializable.h"               // for string, etc
#include "warped.h"

using std::string;


class Deserializer;
class Serializable;
class SerializedInstance;

typedef Serializable* (*deserializeFunc)(SerializedInstance*);

class DeserializerManager {
public:
    /**
       Each serializable type should register with the DeserializerManager
       singleton before attempting to deserialize.  Typically a static code

    */
    void registerDeserializer(const string& dataType, deserializeFunc);

    deserializeFunc findDeserializer(const string& dataType);

    static DeserializerManager* instance();

protected:
    ~DeserializerManager() {}


private:
    std::unordered_map<string, deserializeFunc> deserializerMap;

    DeserializerManager() {}
};

#endif
