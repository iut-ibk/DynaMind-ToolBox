
#ifndef _testratinterpolation_h
#define _testratinterpolation_h

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


bool testri(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testratinterpolation_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testratinterpolation_test();


#endif

