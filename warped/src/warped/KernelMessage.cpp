// See copyright notice in file Copyright in the root directory of this archive.

#include "KernelMessage.h"

void
KernelMessage::serialize( SerializedInstance *addTo ) const {
  addTo->addUnsigned( getSender() );
  addTo->addUnsigned( getReceiver() );
  addTo->addUnsigned( incarnationNumber );
}
