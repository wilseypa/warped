#ifndef VECTOR_ITERATOR_H
#define VECTOR_ITERATOR_H

// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/Iterator.h>
#include <vector>
using std::vector;

template <class Type>
class VectorIterator : public Iterator<Type> {
public:
  VectorIterator( const vector<Type> &initList ) :
    myIterator( initList.begin() ),
    myList( initList ){}
  
  bool hasNext() const { return myIterator < myList.end(); }

  const Type next() const { 
    return const_cast<VectorIterator<Type> *>(this)->next();
  }
  
  Type next(){ 
    Type retval = *myIterator;
    myIterator++;
    return retval;
  }

private:
  typename vector<Type>::const_iterator myIterator;
  const vector<Type> &myList;
};

#endif
