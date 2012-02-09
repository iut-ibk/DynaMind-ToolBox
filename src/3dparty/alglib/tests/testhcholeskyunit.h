
#ifndef _testhcholeskyunit_h
#define _testhcholeskyunit_h

#include "ap.h"
#include "ialglib.h"

#include "cblas.h"
#include "hcholesky.h"


bool testhcholesky(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhcholeskyunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testhcholeskyunit_test();


#endif

