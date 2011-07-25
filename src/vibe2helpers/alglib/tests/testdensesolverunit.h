
#ifndef _testdensesolverunit_h
#define _testdensesolverunit_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "bidiagonal.h"
#include "qr.h"
#include "lq.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "lu.h"
#include "trlinsolve.h"
#include "rcond.h"
#include "tsort.h"
#include "xblas.h"
#include "densesolver.h"


/*************************************************************************
Test
*************************************************************************/
bool testdensesolver(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testdensesolverunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testdensesolverunit_test();


#endif

