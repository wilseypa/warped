#ifndef __STACKOFPTR_HH__
#define __STACKOFPTR_HH__


#include "warped.h"
#include <string.h>

template <class Element>
class StackOfPtr {

private:
  Element** ptrArray;
  int ptrArrayIndex;
  int size;

public:

  inline StackOfPtr( unsigned int initSize = 1000 ) : 
    ptrArray( new Element*[initSize] ), ptrArrayIndex( 0 ), size( initSize ) {}

  inline ~StackOfPtr() {
    delete [] ptrArray;
  }

  inline void insert(Element* ptr) {
    if(ptrArrayIndex == size) {
      int newSize = size*2;
      Element** tmpPtrArray = new Element* [newSize];
      memcpy( tmpPtrArray, ptrArray, size );
      size = newSize;
      delete [] ptrArray;
      ptrArray = tmpPtrArray;
    }
    ptrArray[ptrArrayIndex] = ptr;
    ptrArrayIndex++;
  }

  inline Element* remove() {
    Element* ptr = NULL;
    if (ptrArrayIndex > 0) {
      ptrArrayIndex--;
      ptr = ptrArray[ptrArrayIndex];
      // I commented this out because we don't bother to zero the
      // array when we construct it.  Why do it here?
      //      ptrArray[ptrArrayIndex] = NULL;
    }
    // Else we'll return ptr which was initialize to NULL.

    return ptr;
  }

  inline Element* top() const {
    return ptrArray[ptrArrayIndex-1];
  }
};

#endif //__STACKOFPTR_HH__
