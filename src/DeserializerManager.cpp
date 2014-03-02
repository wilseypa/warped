
#include <stddef.h>                     // for NULL
#include <utility>                      // for pair, make_pair

#include "DeserializerManager.h"

void
DeserializerManager::registerDeserializer(const string& dataType,
                                          deserializeFunc functionPtr) {
    deserializerMap.insert(std::make_pair(dataType, functionPtr));
}

deserializeFunc
DeserializerManager::findDeserializer(const string& dataType) {
    std::unordered_map<string, deserializeFunc>::iterator it = deserializerMap.find(dataType);
    if (it != deserializerMap.end()) {
        return (*it).second;
    } else {
        return NULL;
    }
    //return (*(deserializerMap.find( dataType ))).second;
}


DeserializerManager*
DeserializerManager::instance() {
    static DeserializerManager* instance = new DeserializerManager();

    return instance;
}


