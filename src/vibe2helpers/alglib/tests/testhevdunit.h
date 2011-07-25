
#ifndef _testhevdunit_h
#define _testhevdunit_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "cblas.h"
#include "creflections.h"
#include "hblas.h"
#include "htridiagonal.h"
#include "hevd.h"


/*************************************************************************
Testing symmetric EVD subroutine
*************************************************************************/
bool testhevd(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhevdunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testhevdunit_test();


#endif

