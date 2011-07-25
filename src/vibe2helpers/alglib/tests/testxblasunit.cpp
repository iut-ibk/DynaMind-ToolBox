
#include <stdafx.h>
#include <stdio.h>
#include "testxblasunit.h"

static const double xchunk = 1048576;
static const int xchunkcount = 4;

bool testxblas(bool silent)
{
    bool result;
    bool approxerrors;
    bool exactnesserrors;
    bool waserrors;
    double approxthreshold;
    int maxn;
    int passcount;
    int n;
    int i;
    int pass;
    double v1;
    double v2;
    double v2err;
    ap::real_1d_array x;
    ap::real_1d_array y;
    ap::real_1d_array temp;
    double b;
    double s;

    approxerrors = false;
    exactnesserrors = false;
    waserrors = false;
    approxthreshold = 1000*ap::machineepsilon;
    maxn = 1000;
    passcount = 10;
    
    //
    // tests:
    // 1. ability to calculate dot product
    // 2. higher precision
    //
    for(n = 1; n <= maxn; n++)
    {
        for(pass = 1; pass <= passcount; pass++)
        {
            x.setlength(n);
            y.setlength(n);
            temp.setlength(n);
            
            //
            //  ability to approximately calculate dot product
            //
            for(i = 0; i <= n-1; i++)
            {
                if( ap::fp_greater(ap::randomreal(),0.2) )
                {
                    x(i) = 2*ap::randomreal()-1;
                }
                else
                {
                    x(i) = 0;
                }
                if( ap::fp_greater(ap::randomreal(),0.2) )
                {
                    y(i) = 2*ap::randomreal()-1;
                }
                else
                {
                    y(i) = 0;
                }
            }
            v1 = ap::vdotproduct(&x(0), &y(0), ap::vlen(0,n-1));
            xdot(x, y, n, temp, v2, v2err);
            approxerrors = approxerrors||ap::fp_greater(fabs(v1-v2),approxthreshold);
        }
    }
    
    //
    // test of precision
    //
    n = 10000;
    x.setlength(n);
    y.setlength(n);
    temp.setlength(n);
    for(pass = 0; pass <= passcount-1; pass++)
    {
        
        //
        // First test: X + X + ... + X - X - X - ... - X = 1*X
        //
        s = exp(double(ap::maxint(pass, 50)));
        if( pass==passcount-1&&pass>1 )
        {
            s = ap::maxrealnumber;
        }
        y(0) = (2*ap::randomreal()-1)*s*sqrt(2*ap::randomreal());
        for(i = 1; i <= n-1; i++)
        {
            y(i) = y(0);
        }
        for(i = 0; i <= n/2-1; i++)
        {
            x(i) = 1;
        }
        for(i = n/2; i <= n-2; i++)
        {
            x(i) = -1;
        }
        x(n-1) = 0;
        xdot(x, y, n, temp, v2, v2err);
        exactnesserrors = exactnesserrors||ap::fp_less(v2err,0);
        exactnesserrors = exactnesserrors||ap::fp_greater(v2err,4*ap::machineepsilon*fabs(y(0)));
        exactnesserrors = exactnesserrors||ap::fp_greater(fabs(v2-y(0)),v2err);
        
        //
        // First test: X + X + ... + X = N*X
        //
        s = exp(double(ap::maxint(pass, 50)));
        if( pass==passcount-1&&pass>1 )
        {
            s = ap::maxrealnumber;
        }
        y(0) = (2*ap::randomreal()-1)*s*sqrt(2*ap::randomreal());
        for(i = 1; i <= n-1; i++)
        {
            y(i) = y(0);
        }
        for(i = 0; i <= n-1; i++)
        {
            x(i) = 1;
        }
        xdot(x, y, n, temp, v2, v2err);
        exactnesserrors = exactnesserrors||ap::fp_less(v2err,0);
        exactnesserrors = exactnesserrors||ap::fp_greater(v2err,4*ap::machineepsilon*fabs(y(0))*n);
        exactnesserrors = exactnesserrors||ap::fp_greater(fabs(v2-n*y(0)),v2err);
    }
    
    //
    // report
    //
    waserrors = approxerrors||exactnesserrors;
    if( !silent )
    {
        printf("TESTING XBLAS\n");
        printf("APPROX.TESTS:                            ");
        if( approxerrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("EXACT TESTS:                             ");
        if( exactnesserrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
        printf("\n\n");
    }
    
    //
    // end
    //
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testxblasunit_test_silent()
{
    bool result;

    result = testxblas(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testxblasunit_test()
{
    bool result;

    result = testxblas(false);
    return result;
}




