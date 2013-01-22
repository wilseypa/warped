#ifndef DEFAULT_SCHEDULING_MANAGER_TEST_H
#define DEFAULT_SCHEDULING_MANAGER_TEST_H

// Copyright (c) 2003 Clifton Labs, Inc.  All rights reserved.

// CLIFTON LABS MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
// SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  CLIFTON LABS SHALL NOT BE
// LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, RESULT
// OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.

// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.

// Authors: Dale E. Martin              dmartin@cliftonlabs.com

#include "SchedulingManagerTest.h"

class DefaultSchedulingManager;

class DefaultSchedulingManagerTest : public SchedulingManagerTest {
  CPPUNIT_TEST_SUITE( DefaultSchedulingManagerTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testGetNextEvent );
  CPPUNIT_TEST_SUITE_END();

public:
  DefaultSchedulingManagerTest();

  void setUp();
  void teardown();

  void testConstructor();
  void testGetNextEvent();

protected:  

private:
  SchedulingManager *toTest;

};

#endif
