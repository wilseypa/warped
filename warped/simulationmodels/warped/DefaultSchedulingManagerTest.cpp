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

#include "DefaultSchedulingManagerTest.h"
#include "warped/DefaultSchedulingManager.h"

DefaultSchedulingManagerTest::DefaultSchedulingManagerTest()
 : toTest(0){}

void 
DefaultSchedulingManagerTest::setUp(){
  SchedulingManagerTest::setUp();
  toTest = new DefaultSchedulingManager( getSimManager() );
}

void
DefaultSchedulingManagerTest::teardown(){
  delete toTest;
  toTest = 0;
  SchedulingManagerTest::tearDown();
}

void
DefaultSchedulingManagerTest::testConstructor(){
  SchedulingManagerTest::testConstructor( toTest );
}

void
DefaultSchedulingManagerTest::testGetNextEvent(){
  SchedulingManagerTest::testGetNextEvent( toTest );
}





