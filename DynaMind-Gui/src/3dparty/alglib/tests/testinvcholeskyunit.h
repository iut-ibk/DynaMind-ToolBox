
#ifndef _testinvcholeskyunit_h
#define _testinvcholeskyunit_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"
#include "spdinverse.h"


bool testinvcholesky(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testinvcholeskyunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testinvcholeskyunit_test();


#endif

