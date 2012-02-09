
#ifndef _testhevdbiunit_h
#define _testhevdbiunit_h

#include "ap.h"
#include "ialglib.h"

#include "cblas.h"
#include "creflections.h"
#include "hblas.h"
#include "htridiagonal.h"
#include "blas.h"
#include "tdbisinv.h"
#include "hbisinv.h"


/*************************************************************************
Testing hermitian EVD, BI
*************************************************************************/
bool testhevdbi(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhevdbiunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testhevdbiunit_test();


#endif

