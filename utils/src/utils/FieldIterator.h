#ifndef FIELD_ITERATOR_H
#define FIELD_ITERATOR_H

// See copyright notice in file Copyright in the root directory of this archive.

#include "utils/Iterator.h"

#include <string>
using std::string;

class FieldIterator : public Iterator<string> {
public:
  FieldIterator();
  FieldIterator( const string &initString, const string &initDelimiter );
  FieldIterator( const FieldIterator &that );
  ~FieldIterator();

  FieldIterator &operator=( const FieldIterator &that );

  bool hasNext() const;
  const string next() const;
  string next();

private:
  string myString;
  string myDelimiter;
  size_t beginPos, endPos;
};

#endif
