
#ifndef _testcinvunit_h
#define _testcinvunit_h

#include "ap.h"
#include "ialglib.h"

#include "clu.h"
#include "ctrinverse.h"
#include "cinverse.h"


/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testcinv(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcinvunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testcinvunit_test();


#endif

