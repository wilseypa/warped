
// See copyright notice in file Copyright in the root directory of this archive.

#include "FieldIteratorTest.h"
#include "utils/FieldIterator.h"
#include "utils/Debug.h"
#include <iostream>
using std::endl;

FieldIteratorTest *
FieldIteratorTest::instance(){
  return new FieldIteratorTest();
}

FieldIteratorTest::FieldIteratorTest(){}

FieldIteratorTest::~FieldIteratorTest(){}

int
FieldIteratorTest::fail(){
  utils::debug << "failed!" << endl;
  return -1;
}


int
FieldIteratorTest::regressionTest(){
  if( normalCase() ){
    return -1;
  }
  else if ( emptyString() ){
    return -1;
  }
  else if( noDelimiter() ){
    return -1;
  }
  else{
    return 0;
  }
}



int
FieldIteratorTest::normalCase(){
  int retval = 0;
  const string testString = "this, that, and the other";
  FieldIterator t( testString, ", " );
  if( !t.hasNext() ){
    return fail();
  }
  if( !(t.next() == "this" ) ){
    return fail();
  }
  if( !t.hasNext() ){
    return fail();
  }
  if( !(t.next() == "that" ) ){
    return fail();
  }
  if( !t.hasNext() ){
    return fail();
  }
  if( !(t.next() == "and the other" ) ){
    return fail();
  }
  if( t.hasNext() ){
    return fail();
  }

  utils::debug << "passed." << endl;

  return retval;
}


int
FieldIteratorTest::emptyString(){
  const string testString;
  FieldIterator t1( testString, "" );
  if( t1.hasNext() ){
    return fail();
  }
  FieldIterator t2( testString, "long delimiter" );
  if( t2.hasNext() ){
    return fail();
  }
  return 0;
}

int
FieldIteratorTest::noDelimiter(){
  const string testString("There is no delimiter in this string");
  FieldIterator t1( testString, "foo" );
  if( !t1.hasNext() ){
    return fail();
  }
  if( !( t1.next() == testString ) ){
    return fail();
  }
  if( t1.hasNext() ){
    return fail();
  }
  return 0;
}
