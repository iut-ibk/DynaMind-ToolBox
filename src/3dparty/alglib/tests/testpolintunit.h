
#ifndef _testpolintunit_h
#define _testpolintunit_h

#include "ap.h"
#include "ialglib.h"

#include "tsort.h"
#include "ratinterpolation.h"
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
#include "spline3.h"
#include "leastsquares.h"
#include "lsfit.h"
#include "ratint.h"
#include "taskgen.h"
#include "polint.h"


/*************************************************************************
Unit test
*************************************************************************/
bool testpolint(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testpolintunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testpolintunit_test();


#endif

