
#ifndef _testhsunit_h
#define _testhsunit_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "hessenberg.h"


/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool tesths(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhsunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testhsunit_test();


#endif

