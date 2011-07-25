
#ifndef _testtdevdbiunit_h
#define _testtdevdbiunit_h

#include "ap.h"
#include "ialglib.h"

#include "blas.h"
#include "tdbisinv.h"


/*************************************************************************
Testing EVD, BI
*************************************************************************/
bool testtdevdbi(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testtdevdbiunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testtdevdbiunit_test();


#endif

