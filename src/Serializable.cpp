
#include "Serializable.h"
#include "SerializedInstance.h"

SerializedInstance*
Serializable::serialize() const {
    SerializedInstance* retval = new SerializedInstance(getDataType());
    serialize(retval);
    return retval;
}

