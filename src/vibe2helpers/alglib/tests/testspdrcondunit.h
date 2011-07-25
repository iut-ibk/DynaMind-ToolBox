
#ifndef _testspdrcondunit_h
#define _testspdrcondunit_h

#include "ap.h"
#include "ialglib.h"

#include "trlinsolve.h"
#include "cholesky.h"
#include "estnorm.h"
#include "spdrcond.h"


bool testrcondcholesky(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testspdrcondunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testspdrcondunit_test();


#endif

