
#ifndef _testcqrunit_h
#define _testcqrunit_h

#include "ap.h"
#include "ialglib.h"

#include "creflections.h"
#include "cqr.h"


/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testcqr(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcqrunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcqrunit_test();


#endif

