
#ifndef _testcholeskyunit_h
#define _testcholeskyunit_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"


bool testcholesky(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcholeskyunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcholeskyunit_test();


#endif

