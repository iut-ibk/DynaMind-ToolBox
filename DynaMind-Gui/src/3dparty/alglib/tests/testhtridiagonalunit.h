
#ifndef _testhtridiagonalunit_h
#define _testhtridiagonalunit_h

#include "ap.h"
#include "ialglib.h"

#include "cblas.h"
#include "creflections.h"
#include "hblas.h"
#include "htridiagonal.h"


bool testhtridiagonal(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhtridiagonalunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testhtridiagonalunit_test();


#endif

