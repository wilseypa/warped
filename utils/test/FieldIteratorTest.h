#ifndef FIELD_ITERATOR_TEST_H
#define FIELD_ITERATOR_TEST_H

// See copyright notice in file Copyright in the root directory of this archive.

class FieldIteratorTest {
public:
  static FieldIteratorTest *instance();

  int regressionTest();

private:
  int fail();
  int normalCase();
  int emptyString();
  int noDelimiter();

  FieldIteratorTest();
  ~FieldIteratorTest();
};

#endif
