#ifndef VARARGS_HH
#define VARARGS_HH

// See copyright notice in file Copyright in the root directory of this archive.

#include <utils/VectorIterator.h>
#include <vector>
using std::vector;

template <typename Type>
class VarArgs {
public:
  VarArgs &add( Type item ){
    myData.push_back( item );
    return *this;
  }

  const VectorIterator<Type> iterator() const {
    return VectorIterator<Type>(myData);
  }

  VectorIterator<Type> iterator(){
    return VectorIterator<Type>(myData);
  }

  int length() const {
    return myData.size();
  }

  VarArgs &operator<<( Type item ){
    myData.push_back(item);
    return *this;
  }

  virtual ~VarArgs();

private:
  vector<Type> myData;
};

template<typename Type>
inline
VarArgs<Type>::~VarArgs(){}

#endif
