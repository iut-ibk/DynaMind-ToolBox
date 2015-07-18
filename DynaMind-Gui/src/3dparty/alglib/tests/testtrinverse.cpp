
#include <stdafx.h>
#include <stdio.h>
#include "testtrinverse.h"

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testtrinv(bool silent)
{
    bool result;
    int shortmn;
    int maxn;
    int passcount;
    double threshold;
    ap::real_2d_array a;
    ap::real_2d_array b;
    int n;
    int pass;
    int i;
    int j;
    int task;
    bool isupper;
    bool isunit;
    double v;
    bool invfailed;
    bool inverrors;
    bool structerrors;
    bool waserrors;

    invfailed = false;
    inverrors = false;
    structerrors = false;
    waserrors = false;
    maxn = 15;
    passcount = 5;
    threshold = 5*100*ap::machineepsilon;
    
    //
    // Test
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        b.setbounds(0, n-1, 0, n-1);
        for(task = 0; task <= 3; task++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                
                //
                // Determine task
                //
                isupper = task%2==0;
                isunit = task/2%2==0;
                
                //
                // Generate matrix
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( i==j )
                        {
                            a(i,j) = 1.5+ap::randomreal();
                        }
                        else
                        {
                            a(i,j) = 2*ap::randomreal()-1;
                        }
                        b(i,j) = a(i,j);
                    }
                }
                
                //
                // Inverse
                //
                if( !rmatrixtrinverse(b, n, isupper, isunit) )
                {
                    invfailed = true;
                    continue;
                }
                
                //
                // Structural test
                //
                if( isunit )
                {
                    for(i = 0; i <= n-1; i++)
                    {
                        structerrors = structerrors||ap::fp_neq(a(i,i),b(i,i));
                    }
                }
                if( isupper )
                {
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = 0; j <= i-1; j++)
                        {
                            structerrors = structerrors||ap::fp_neq(a(i,j),b(i,j));
                        }
                    }
                }
                else
                {
                    for(i = 0; i <= n-1; i++)
                    {
                        for(j = i+1; j <= n-1; j++)
                        {
                            structerrors = structerrors||ap::fp_neq(a(i,j),b(i,j));
                        }
                    }
                }
                
                //
                // Inverse test
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( j<i&&isupper||j>i&&!isupper )
                        {
                            a(i,j) = 0;
                            b(i,j) = 0;
                        }
                    }
                }
                if( isunit )
                {
                    for(i = 0; i <= n-1; i++)
                    {
                        a(i,i) = 1;
                        b(i,i) = 1;
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        v = ap::vdotproduct(a.getrow(i, 0, n-1), b.getcolumn(j, 0, n-1));
                        if( j!=i )
                        {
                            inverrors = inverrors||ap::fp_greater(fabs(v),threshold);
                        }
                        else
                        {
                            inverrors = inverrors||ap::fp_greater(fabs(v-1),threshold);
                        }
                    }
                }
            }
        }
    }
    
    //
    // report
    //
    waserrors = inverrors||structerrors||invfailed;
    if( !silent )
    {
        printf("TESTING TRIANGULAR INVERSE\n");
        if( invfailed )
        {
            printf("SOME INVERSIONS FAILED\n");
        }
        printf("INVERSION TEST:                          ");
        if( inverrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("STRUCTURE TEST:                          ");
        if( structerrors )
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
bool testtrinverse_test_silent()
{
    bool result;

    result = testtrinv(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testtrinverse_test()
{
    bool result;

    result = testtrinv(false);
    return result;
}




