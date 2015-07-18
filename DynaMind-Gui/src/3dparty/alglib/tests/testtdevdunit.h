
#ifndef _testtdevdunit_h
#define _testtdevdunit_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "rotations.h"
#include "tdevd.h"


/*************************************************************************
Testing bidiagonal SVD decomposition subroutine
*************************************************************************/
bool testtdevd(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testtdevdunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testtdevdunit_test();


#endif

