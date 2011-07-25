
#ifndef _testlm_h
#define _testlm_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "trinverse.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "lbfgs.h"
#include "minlm.h"


bool testminlm(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlm_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testlm_test();


#endif

