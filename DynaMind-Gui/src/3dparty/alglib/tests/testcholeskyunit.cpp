
#include <stdafx.h>
#include <stdio.h>
#include "testcholeskyunit.h"

bool testcholesky(bool silent)
{
    bool result;
    ap::real_2d_array l;
    ap::real_2d_array a;
    int n;
    int pass;
    int i;
    int j;
    int minij;
    bool upperin;
    bool cr;
    double v;
    double err;
    bool wf;
    bool waserrors;
    int passcount;
    int maxn;
    int htask;
    double threshold;

    err = 0;
    wf = false;
    passcount = 10;
    maxn = 20;
    threshold = 1000*ap::machineepsilon;
    waserrors = false;
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        l.setbounds(0, n-1, 0, n-1);
        a.setbounds(0, n-1, 0, n-1);
        for(htask = 0; htask <= 1; htask++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                upperin = htask==0;
                
                //
                // Prepare task:
                // * A contains upper (or lower) half of SPD matrix
                // * L contains its Cholesky factor (upper or lower)
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = i+1; j <= n-1; j++)
                    {
                        l(i,j) = 2*ap::randomreal()-1;
                        l(j,i) = l(i,j);
                    }
                    l(i,i) = 0.8+ap::randomreal();
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = ap::vdotproduct(l.getrow(i, 0, minij), l.getcolumn(j, 0, minij));
                        a(i,j) = v;
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( upperin )
                        {
                            if( j<i )
                            {
                                a(i,j) = 0;
                                l(i,j) = 0;
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                a(i,j) = 0;
                                l(i,j) = 0;
                            }
                        }
                    }
                }
                
                //
                // decomposition
                //
                if( !spdmatrixcholesky(a, n, upperin) )
                {
                    wf = true;
                    continue;
                }
                
                //
                // Test
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        err = ap::maxreal(err, fabs(a(i,j)-l(i,j)));
                    }
                }
            }
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(err,threshold)||wf;
    if( !silent )
    {
        printf("TESTING CHOLESKY DECOMPOSITION\n");
        printf("ERROR:                                   %5.3le\n",
            double(err));
        printf("ALWAYS SUCCEDED:                         ");
        if( wf )
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
    result = !waserrors;
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcholeskyunit_test_silent()
{
    bool result;

    result = testcholesky(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcholeskyunit_test()
{
    bool result;

    result = testcholesky(false);
    return result;
}




