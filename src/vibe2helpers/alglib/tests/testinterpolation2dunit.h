
#ifndef _testinterpolation2dunit_h
#define _testinterpolation2dunit_h

#include "ap.h"
#include "ialglib.h"

#include "spline3.h"
#include "blas.h"
#include "trinverse.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "lbfgs.h"
#include "minlm.h"
#include "reflections.h"
#include "bidiagonal.h"
#include "qr.h"
#include "lq.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "lu.h"
#include "trlinsolve.h"
#include "rcond.h"
#include "leastsquares.h"
#include "lsfit.h"
#include "spline1d.h"
#include "spline2d.h"


bool test2dinterpolation(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testinterpolation2dunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testinterpolation2dunit_test();


#endif

