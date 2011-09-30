// See copyright notice in file Copyright in the root directory of this archive.

#include "SingleLinkedList.h"
#include <algorithm>
#include <functional>

using std::for_each;
using std::unary_function;

SingleLinkedList::SingleLinkedList(){
  processedEvents = new vector<const Event *>;
}

SingleLinkedList::~SingleLinkedList(){
  cleanUp();
  delete processedEvents;
}

void
SingleLinkedList::insert( const Event *event ){
  push(event); // insert this guy into the queue
}

const Event *
SingleLinkedList::getEvent(){
  const Event *retval = top(); // have a look at the top element
  processedEvents->push_back(retval); // store this guy for deletion
  pop(); // remove this guy from the queue
  return retval;
}

const Event *
SingleLinkedList::peekEvent(){
  if(!empty()){
    return top(); // have a look at the top element
  }
  else {
    return 0;
  }
}

class DeleteEvent {
public:
  void operator()( const Event *toDelete ){
    delete toDelete;
  }
};

void
SingleLinkedList::cleanUp(){
  for_each( processedEvents->begin(),
	    processedEvents->end(),
	    DeleteEvent() );

  processedEvents->resize(0);
}

const string &
SingleLinkedList::getType() {
  static const string singleLinkedListType = "SingleLinkedList";
  return singleLinkedListType;
}
