
#ifndef _testlbfgs_h
#define _testlbfgs_h

#include "ap.h"
#include "ialglib.h"

#include "lbfgs.h"


bool testminlbfgs(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testlbfgs_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testlbfgs_test();


#endif

