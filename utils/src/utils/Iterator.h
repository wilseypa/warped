#ifndef ITERATOR_H
#define ITERATOR_H

// See copyright notice in file Copyright in the root directory of this archive.

template <class Type>
class Iterator {
protected:
  Iterator(){}

public:
  virtual bool hasNext() const = 0;
  virtual const Type next() const = 0;
  virtual Type next() = 0;

  virtual ~Iterator(){}
};

#endif
