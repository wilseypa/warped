
// See copyright notice in file Copyright in the root directory of this archive.

#include "utils/FieldIterator.h"

/*
"This, that, and the other", delimiter = ", ".
 0123456789111111111122222
           012345678901234

0) beginPos = 0, endPos = 0
0.5) beginPos = 0, endPos = 4
1) hasNext() -> beginPos != string::npos, so we're not done
2) return (0,4), endPos = 10, beginPos = 4 + 2;
3) hasNext() -> beginPos != string::npos so we're not done
4) return (6, 10), endPos = string::npos <= string.length(), beginPos = 6 + 2
3) hasNext() -> beginPos != string::npos so we're not done
5) return ( 8, 24 ), endPos == string.length(), beginPos = string::npos
6) hasNext() -> complete

*/

FieldIterator::FieldIterator() :
  myString(""),
  myDelimiter( "" ),
  beginPos(0),
  endPos(0){}

FieldIterator::FieldIterator( const string &initString, const string &initDelimiter ) :
  myString( initString ),
  myDelimiter( initDelimiter ),
  beginPos( 0 ),
  endPos( 0 ){
  // Prime the pump...
  next();
}

FieldIterator::FieldIterator( const FieldIterator &that ) :
  Iterator<string>( that ),
  myString( that.myString ),
  myDelimiter( that.myDelimiter ),
  beginPos( that.beginPos ),
  endPos( that.endPos ){}

FieldIterator::~FieldIterator(){}

FieldIterator &
FieldIterator::operator=( const FieldIterator &that ){
  myString = that.myString;
  myDelimiter = that.myDelimiter;
  beginPos = that.beginPos;
  endPos = that.endPos;
  
  return *this;
}

bool
FieldIterator::hasNext() const {
  return beginPos != string::npos;
}

const string
FieldIterator::next() const {
  return const_cast<FieldIterator *>(this)->next();
}

string
FieldIterator::next(){
  string retval;
  if( beginPos != string::npos ){
    retval = myString.substr( beginPos, endPos - beginPos );
    if( endPos > 0 ){
      beginPos = endPos + myDelimiter.size();
    }
    if( beginPos >= myString.size() ){
      beginPos = string::npos;
    }
    else{
      endPos = myString.find( myDelimiter, beginPos );
      if( endPos == string::npos ){
	endPos = myString.size();
      }
    }
  }
  return retval;
}
