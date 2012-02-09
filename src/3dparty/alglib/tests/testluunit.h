
#ifndef _testluunit_h
#define _testluunit_h

#include "ap.h"
#include "ialglib.h"

#include "lu.h"


bool testlu(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testluunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testluunit_test();


#endif

