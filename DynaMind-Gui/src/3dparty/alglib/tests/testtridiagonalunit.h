
#ifndef _testtridiagonalunit_h
#define _testtridiagonalunit_h

#include "ap.h"
#include "ialglib.h"

#include "sblas.h"
#include "reflections.h"
#include "tridiagonal.h"


bool testtridiagonal(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testtridiagonalunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testtridiagonalunit_test();


#endif

