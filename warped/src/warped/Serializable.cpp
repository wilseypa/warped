// See copyright notice in file Copyright in the root directory of this archive.

#include "Serializable.h"
#include "SerializedInstance.h"

SerializedInstance *
Serializable::serialize() const {
  SerializedInstance *retval = new SerializedInstance( getDataType() );
  serialize( retval );
  return retval;
}
  
