
#ifndef _testspdgevdunit_h
#define _testspdgevdunit_h

#include "ap.h"
#include "ialglib.h"

#include "cholesky.h"
#include "sblas.h"
#include "blas.h"
#include "trinverse.h"
#include "rotations.h"
#include "tdevd.h"
#include "reflections.h"
#include "tridiagonal.h"
#include "sevd.h"
#include "spdgevd.h"


/*************************************************************************
Testing bidiagonal SVD decomposition subroutine
*************************************************************************/
bool testspdgevd(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testspdgevdunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testspdgevdunit_test();


#endif

