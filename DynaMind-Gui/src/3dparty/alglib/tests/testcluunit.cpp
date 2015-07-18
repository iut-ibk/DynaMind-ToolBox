
#include <stdafx.h>
#include <stdio.h>
#include "testcluunit.h"

static void testcluproblem(const ap::complex_2d_array& a,
     int m,
     int n,
     double& diffpu,
     double& luerr);

bool testclu(bool silent)
{
    bool result;
    ap::complex_2d_array a;
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
            testcluproblem(a, i, j, differr, luerr);
        }
    }
    
    //
    // Long non-zero matrix
    //
    for(i = 1; i <= maxmn; i++)
    {
        for(j = 1; j <= 5; j++)
        {
            a(i,j).x = 2*ap::randomreal()-1;
            a(i,j).y = 2*ap::randomreal()-1;
        }
    }
    for(i = 1; i <= maxmn; i++)
    {
        for(j = 1; j <= 5; j++)
        {
            testcluproblem(a, i, j, differr, luerr);
        }
    }
    for(i = 1; i <= 5; i++)
    {
        for(j = 1; j <= maxmn; j++)
        {
            a(i,j).x = 2*ap::randomreal()-1;
            a(i,j).y = 2*ap::randomreal()-1;
        }
    }
    for(i = 1; i <= 5; i++)
    {
        for(j = 1; j <= maxmn; j++)
        {
            testcluproblem(a, i, j, differr, luerr);
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
                            a(i,j).x = 2*ap::randomreal()-1;
                            a(i,j).y = 2*ap::randomreal()-1;
                        }
                        else
                        {
                            a(i,j) = 0;
                        }
                    }
                }
                testcluproblem(a, m, n, differr, luerr);
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
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                }
            }
            testcluproblem(a, m, n, differr, luerr);
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(differr,threshold)||ap::fp_greater(luerr,threshold);
    if( !silent )
    {
        printf("TESTING COMPLEX LU DECOMPOSITION\n");
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


static void testcluproblem(const ap::complex_2d_array& a,
     int m,
     int n,
     double& diffpu,
     double& luerr)
{
    ap::complex_2d_array t1;
    ap::complex_2d_array t2;
    ap::complex_2d_array t3;
    ap::integer_1d_array it1;
    ap::integer_1d_array it2;
    int i;
    int j;
    int k;
    ap::complex v;
    ap::complex_2d_array a0;
    ap::integer_1d_array p0;
    double mx;
    int i_;

    mx = 0;
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( ap::fp_greater(ap::abscomplex(a(i,j)),mx) )
            {
                mx = ap::abscomplex(a(i,j));
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
        for(i_=1; i_<=n;i_++)
        {
            t1(i,i_) = a(i,i_);
        }
    }
    complexludecomposition(t1, m, n, it1);
    complexludecompositionunpacked(a, m, n, t2, t3, it2);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= ap::minint(m, n); j++)
        {
            if( i>j )
            {
                diffpu = ap::maxreal(diffpu, ap::abscomplex(t1(i,j)-t2(i,j))/mx);
            }
            if( i==j )
            {
                diffpu = ap::maxreal(diffpu, ap::abscomplex(1-t2(i,j))/mx);
            }
            if( i<j )
            {
                diffpu = ap::maxreal(diffpu, ap::abscomplex(0-t2(i,j))/mx);
            }
        }
    }
    for(i = 1; i <= ap::minint(m, n); i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( i>j )
            {
                diffpu = ap::maxreal(diffpu, ap::abscomplex(0-t3(i,j))/mx);
            }
            if( i<=j )
            {
                diffpu = ap::maxreal(diffpu, ap::abscomplex(t1(i,j)-t3(i,j))/mx);
            }
        }
    }
    for(i = 1; i <= ap::minint(m, n); i++)
    {
        diffpu = ap::maxreal(diffpu, ap::abscomplex(it1(i)-it2(i)));
    }
    
    //
    // Test unpacked LU
    //
    complexludecompositionunpacked(a, m, n, t1, t2, it1);
    t3.setbounds(1, m, 1, n);
    k = ap::minint(m, n);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            v = 0.0;
            for(i_=1; i_<=k;i_++)
            {
                v += t1(i,i_)*t2(i_,j);
            }
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
            luerr = ap::maxreal(luerr, ap::abscomplex(a(i,j)-t3(i,j))/mx);
        }
    }
    
    //
    // Test 0-based LU
    //
    t1.setbounds(1, m, 1, n);
    for(i = 1; i <= m; i++)
    {
        for(i_=1; i_<=n;i_++)
        {
            t1(i,i_) = a(i,i_);
        }
    }
    complexludecomposition(t1, m, n, it1);
    a0.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            a0(i,j) = a(i+1,j+1);
        }
    }
    cmatrixlu(a0, m, n, p0);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            diffpu = ap::maxreal(diffpu, ap::abscomplex(a0(i,j)-t1(i+1,j+1)));
        }
    }
    for(i = 0; i <= ap::minint(m-1, n-1); i++)
    {
        diffpu = ap::maxreal(diffpu, ap::abscomplex(p0(i)+1-it1(i+1)));
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcluunit_test_silent()
{
    bool result;

    result = testclu(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcluunit_test()
{
    bool result;

    result = testclu(false);
    return result;
}




