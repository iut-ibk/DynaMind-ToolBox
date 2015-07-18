
#ifndef _testrcondunit_h
#define _testrcondunit_h

#include "ap.h"
#include "ialglib.h"

#include "lu.h"
#include "trlinsolve.h"
#include "rcond.h"


bool testrcond(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testrcondunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testrcondunit_test();


#endif

