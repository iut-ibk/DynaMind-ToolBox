
#ifndef _testsevdunit_h
#define _testsevdunit_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "sblas.h"
#include "reflections.h"
#include "tridiagonal.h"
#include "sevd.h"


/*************************************************************************
Testing symmetric EVD subroutine
*************************************************************************/
bool testsevd(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testsevdunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testsevdunit_test();


#endif

