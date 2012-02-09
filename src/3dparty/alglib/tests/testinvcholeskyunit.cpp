
#include <stdafx.h>
#include <stdio.h>
#include "testinvcholeskyunit.h"

bool testinvcholesky(bool silent)
{
    bool result;
    ap::real_2d_array l;
    ap::real_2d_array a;
    ap::real_2d_array a2;
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
        a2.setbounds(0, n-1, 0, n-1);
        for(htask = 0; htask <= 1; htask++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                upperin = htask==0;
                
                //
                // Prepare task:
                // * A contains upper (or lower) half of SPD matrix
                // * L contains its Cholesky factor (upper or lower)
                // * A2 contains copy of A
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = i+1; j <= n-1; j++)
                    {
                        l(i,j) = 2*ap::randomreal()-1;
                        l(j,i) = l(i,j);
                    }
                    l(i,i) = 1.1+ap::randomreal();
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = ap::vdotproduct(l.getrow(i, 0, minij), l.getcolumn(j, 0, minij));
                        a(i,j) = v;
                        a2(i,j) = v;
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
                                l(i,j) = 0;
                                a2(i,j) = 0;
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                l(i,j) = 0;
                                a2(i,j) = 0;
                            }
                        }
                    }
                }
                
                //
                // test inv(A):
                // 1. invert
                // 2. complement with missing triangle
                // 3. test
                //
                if( !spdmatrixinverse(a2, n, upperin) )
                {
                    wf = true;
                    continue;
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( upperin )
                        {
                            if( j<i )
                            {
                                a2(i,j) = a2(j,i);
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                a2(i,j) = a2(j,i);
                            }
                        }
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        v = ap::vdotproduct(a.getrow(i, 0, n-1), a2.getcolumn(j, 0, n-1));
                        if( j==i )
                        {
                            err = ap::maxreal(err, fabs(v-1));
                        }
                        else
                        {
                            err = ap::maxreal(err, fabs(v));
                        }
                    }
                }
                
                //
                // test inv(cholesky(A)):
                // 1. invert
                // 2. complement with missing triangle
                // 3. test
                //
                if( !spdmatrixcholeskyinverse(l, n, upperin) )
                {
                    wf = true;
                    continue;
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( upperin )
                        {
                            if( j<i )
                            {
                                l(i,j) = l(j,i);
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                l(i,j) = l(j,i);
                            }
                        }
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        v = ap::vdotproduct(a.getrow(i, 0, n-1), l.getcolumn(j, 0, n-1));
                        if( j==i )
                        {
                            err = ap::maxreal(err, fabs(v-1));
                        }
                        else
                        {
                            err = ap::maxreal(err, fabs(v));
                        }
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
        printf("TESTING SPD INVERSE\n");
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
bool testinvcholeskyunit_test_silent()
{
    bool result;

    result = testinvcholesky(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testinvcholeskyunit_test()
{
    bool result;

    result = testinvcholesky(false);
    return result;
}




