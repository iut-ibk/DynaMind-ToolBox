
#ifndef _testautogk_h
#define _testautogk_h

#include "ap.h"
#include "ialglib.h"

#include "tsort.h"
#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "gammafunc.h"
#include "gq.h"
#include "gkq.h"
#include "autogk.h"


/*************************************************************************
Test
*************************************************************************/
bool testautogkunit(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testautogk_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testautogk_test();


#endif

