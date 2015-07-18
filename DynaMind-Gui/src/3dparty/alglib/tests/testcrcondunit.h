
#ifndef _testcrcondunit_h
#define _testcrcondunit_h

#include "ap.h"
#include "ialglib.h"

#include "clu.h"
#include "ctrlinsolve.h"
#include "crcond.h"


bool testcrcond(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcrcondunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcrcondunit_test();


#endif

