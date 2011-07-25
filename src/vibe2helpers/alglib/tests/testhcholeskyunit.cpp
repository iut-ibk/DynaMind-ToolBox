
#include <stdafx.h>
#include <stdio.h>
#include "testhcholeskyunit.h"

bool testhcholesky(bool silent)
{
    bool result;
    ap::complex_2d_array l;
    ap::complex_2d_array a;
    int n;
    int pass;
    int i;
    int j;
    int minij;
    bool upperin;
    bool cr;
    ap::complex v;
    double err;
    bool wf;
    bool waserrors;
    int passcount;
    int maxn;
    int htask;
    double threshold;
    int i_;

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
                        l(i,j).x = 2*ap::randomreal()-1;
                        l(i,j).y = 2*ap::randomreal()-1;
                        l(j,i) = ap::conj(l(i,j));
                    }
                    l(i,i) = 1.2+ap::randomreal();
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = 0.0;
                        for(i_=0; i_<=minij;i_++)
                        {
                            v += l(i,i_)*l(i_,j);
                        }
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
                if( !hmatrixcholesky(a, n, upperin) )
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
                        err = ap::maxreal(err, ap::abscomplex(a(i,j)-l(i,j)));
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
        printf("TESTING HERMITIAN CHOLESKY DECOMPOSITION\n");
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
bool testhcholeskyunit_test_silent()
{
    bool result;

    result = testhcholesky(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testhcholeskyunit_test()
{
    bool result;

    result = testhcholesky(false);
    return result;
}




