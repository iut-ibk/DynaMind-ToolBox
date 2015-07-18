
#ifndef _testmlpeunit_h
#define _testmlpeunit_h

#include "ap.h"
#include "ialglib.h"

#include "mlpbase.h"
#include "trinverse.h"
#include "lbfgs.h"
#include "cholesky.h"
#include "spdsolve.h"
#include "mlptrain.h"
#include "tsort.h"
#include "descriptivestatistics.h"
#include "bdss.h"
#include "mlpe.h"


bool testmlpe(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testmlpeunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testmlpeunit_test();


#endif

