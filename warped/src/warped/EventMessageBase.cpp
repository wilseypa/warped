// See copyright notice in file Copyright in the root directory of this archive.

#include "EventMessageBase.h"
#include "Event.h"

void 
EventMessageBase::serialize( SerializedInstance *addTo ) const {
  KernelMessage::serialize( addTo );
  addTo->addSerializable( event );
  addTo->addString( gVTInfo );
}
