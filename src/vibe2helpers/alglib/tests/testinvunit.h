
#ifndef _testinvunit_h
#define _testinvunit_h

#include "ap.h"
#include "ialglib.h"

#include "lu.h"
#include "trinverse.h"
#include "inv.h"


/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testinv(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testinvunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testinvunit_test();


#endif

