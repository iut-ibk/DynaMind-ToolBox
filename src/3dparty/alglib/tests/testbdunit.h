
#ifndef _testbdunit_h
#define _testbdunit_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "bidiagonal.h"


/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testbd(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testbdunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testbdunit_test();


#endif

