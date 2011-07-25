
#ifndef _testldaunit_h
#define _testldaunit_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "sblas.h"
#include "reflections.h"
#include "tridiagonal.h"
#include "sevd.h"
#include "cholesky.h"
#include "spdinverse.h"
#include "lda.h"


bool testlda(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testldaunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testldaunit_test();


#endif

