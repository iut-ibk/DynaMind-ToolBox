
#ifndef _llstestunit_h
#define _llstestunit_h

#include "ap.h"
#include "ialglib.h"

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


bool testlls(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool llstestunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool llstestunit_test();


#endif

