
#include <stdafx.h>
#include <stdio.h>
#include "testluunit.h"

static void testluproblem(const ap::real_2d_array& a,
     int m,
     int n,
     double& diffpu,
     double& luerr);

bool testlu(bool silent)
{
    bool result;
    ap::real_2d_array a;
    int m;
    int n;
    int maxmn;
    int i;
    int j;
    int pass;
    bool waserrors;
    double differr;
    double luerr;
    double threshold;

    differr = 0;
    luerr = 0;
    waserrors = false;
    maxmn = 50;
    threshold = ap::machineepsilon*maxmn;
    a.setbounds(1, maxmn, 1, maxmn);
    
    //
    // zero matrix, several cases
    //
    for(i = 1; i <= maxmn; i++)
    {
        for(j = 1; j <= maxmn; j++)
        {
            a(i,j) = 0;
        }
    }
    for(i = 1; i <= 5; i++)
    {
        for(j = 1; j <= 5; j++)
        {
            testluproblem(a, i, j, differr, luerr);
        }
    }
    
    //
    // Long non-zero matrix
    //
    for(i = 1; i <= maxmn; i++)
    {
        for(j = 1; j <= 5; j++)
        {
            a(i,j) = 2*ap::randomreal()-1;
        }
    }
    for(i = 1; i <= maxmn; i++)
    {
        for(j = 1; j <= 5; j++)
        {
            testluproblem(a, i, j, differr, luerr);
        }
    }
    for(i = 1; i <= 5; i++)
    {
        for(j = 1; j <= maxmn; j++)
        {
            a(i,j) = 2*ap::randomreal()-1;
        }
    }
    for(i = 1; i <= 5; i++)
    {
        for(j = 1; j <= maxmn; j++)
        {
            testluproblem(a, i, j, differr, luerr);
        }
    }
    
    //
    // Sparse matrices
    //
    for(m = 1; m <= 10; m++)
    {
        for(n = 1; n <= 10; n++)
        {
            for(pass = 1; pass <= 5; pass++)
            {
                for(i = 1; i <= m; i++)
                {
                    for(j = 1; j <= n; j++)
                    {
                        if( ap::fp_greater(ap::randomreal(),0.8) )
                        {
                            a(i,j) = 2*ap::randomreal()-1;
                        }
                        else
                        {
                            a(i,j) = 0;
                        }
                    }
                }
                testluproblem(a, m, n, differr, luerr);
            }
        }
    }
    
    //
    // Dense matrices
    //
    for(m = 1; m <= 10; m++)
    {
        for(n = 1; n <= 10; n++)
        {
            for(i = 1; i <= m; i++)
            {
                for(j = 1; j <= n; j++)
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
            }
            testluproblem(a, m, n, differr, luerr);
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(differr,threshold)||ap::fp_greater(luerr,threshold);
    if( !silent )
    {
        printf("TESTING LU DECOMPOSITION\n");
        printf("Difference (normal/packed/0-based LU):   %5.3le\n",
            double(differr));
        printf("LU decomposition error:                  %5.3le\n",
            double(luerr));
        printf("Threshold:                               %5.3le\n",
            double(threshold));
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


static void testluproblem(const ap::real_2d_array& a,
     int m,
     int n,
     double& diffpu,
     double& luerr)
{
    ap::real_2d_array t1;
    ap::real_2d_array t2;
    ap::real_2d_array t3;
    ap::integer_1d_array it1;
    ap::integer_1d_array it2;
    int i;
    int j;
    int k;
    double v;
    double mx;
    ap::real_2d_array a0;
    ap::integer_1d_array p0;

    mx = 0;
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( ap::fp_greater(fabs(a(i,j)),mx) )
            {
                mx = fabs(a(i,j));
            }
        }
    }
    if( ap::fp_eq(mx,0) )
    {
        mx = 1;
    }
    
    //
    // Compare LU and unpacked LU
    //
    t1.setbounds(1, m, 1, n);
    for(i = 1; i <= m; i++)
    {
        ap::vmove(&t1(i, 1), &a(i, 1), ap::vlen(1,n));
    }
    ludecomposition(t1, m, n, it1);
    ludecompositionunpacked(a, m, n, t2, t3, it2);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= ap::minint(m, n); j++)
        {
            if( i>j )
            {
                diffpu = ap::maxreal(diffpu, fabs(t1(i,j)-t2(i,j))/mx);
            }
            if( i==j )
            {
                diffpu = ap::maxreal(diffpu, fabs(1-t2(i,j))/mx);
            }
            if( i<j )
            {
                diffpu = ap::maxreal(diffpu, fabs(0-t2(i,j))/mx);
            }
        }
    }
    for(i = 1; i <= ap::minint(m, n); i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( i>j )
            {
                diffpu = ap::maxreal(diffpu, fabs(0-t3(i,j))/mx);
            }
            if( i<=j )
            {
                diffpu = ap::maxreal(diffpu, fabs(t1(i,j)-t3(i,j))/mx);
            }
        }
    }
    for(i = 1; i <= ap::minint(m, n); i++)
    {
        diffpu = ap::maxreal(diffpu, fabs(double(it1(i)-it2(i))));
    }
    
    //
    // Test unpacked LU
    //
    ludecompositionunpacked(a, m, n, t1, t2, it1);
    t3.setbounds(1, m, 1, n);
    k = ap::minint(m, n);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            v = ap::vdotproduct(t1.getrow(i, 1, k), t2.getcolumn(j, 1, k));
            t3(i,j) = v;
        }
    }
    for(i = ap::minint(m, n); i >= 1; i--)
    {
        if( i!=it1(i) )
        {
            for(j = 1; j <= n; j++)
            {
                v = t3(i,j);
                t3(i,j) = t3(it1(i),j);
                t3(it1(i),j) = v;
            }
        }
    }
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            luerr = ap::maxreal(luerr, fabs(a(i,j)-t3(i,j))/mx);
        }
    }
    
    //
    // Test 0-based LU
    //
    t1.setbounds(1, m, 1, n);
    for(i = 1; i <= m; i++)
    {
        ap::vmove(&t1(i, 1), &a(i, 1), ap::vlen(1,n));
    }
    ludecomposition(t1, m, n, it1);
    a0.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            a0(i,j) = a(i+1,j+1);
        }
    }
    rmatrixlu(a0, m, n, p0);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            diffpu = ap::maxreal(diffpu, fabs(a0(i,j)-t1(i+1,j+1)));
        }
    }
    for(i = 0; i <= ap::minint(m-1, n-1); i++)
    {
        diffpu = ap::maxreal(diffpu, fabs(double(p0(i)+1-it1(i+1))));
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testluunit_test_silent()
{
    bool result;

    result = testlu(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testluunit_test()
{
    bool result;

    result = testlu(false);
    return result;
}




