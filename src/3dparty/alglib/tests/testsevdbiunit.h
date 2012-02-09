
#ifndef _testsevdbiunit_h
#define _testsevdbiunit_h

#include "ap.h"
#include "ialglib.h"

#include "sblas.h"
#include "reflections.h"
#include "tridiagonal.h"
#include "blas.h"
#include "tdbisinv.h"
#include "sbisinv.h"


/*************************************************************************
Testing symmetric EVD, BI
*************************************************************************/
bool testsevdbi(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testsevdbiunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testsevdbiunit_test();


#endif

