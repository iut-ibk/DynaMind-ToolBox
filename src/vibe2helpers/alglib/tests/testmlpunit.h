
#ifndef _testmlpunit_h
#define _testmlpunit_h

#include "ap.h"
#include "ialglib.h"

#include "mlpbase.h"
#include "trinverse.h"
#include "lbfgs.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "mlptrain.h"


bool testmlp(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testmlpunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testmlpunit_test();


#endif

