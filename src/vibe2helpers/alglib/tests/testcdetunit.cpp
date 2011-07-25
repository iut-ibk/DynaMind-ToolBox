
#include <stdafx.h>
#include <stdio.h>
#include "testcdetunit.h"

static bool deterrors;

static void fillsparsea(ap::complex_2d_array& a,
     int m,
     int n,
     double sparcity);
static void testproblem(const ap::complex_2d_array& a, int n);
static void makeacopy(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b);
static ap::complex dettriangle(ap::complex_2d_array a, const int& n);

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testcdet(bool silent)
{
    bool result;
    int maxn;
    int gpasscount;
    double threshold;
    ap::complex_2d_array a;
    int n;
    int gpass;
    int i;
    int j;
    bool waserrors;

    deterrors = false;
    waserrors = false;
    maxn = 8;
    gpasscount = 5;
    threshold = 5*100*ap::machineepsilon;
    a.setbounds(0, maxn-1, 0, maxn-1);
    
    //
    // Different problems
    //
    for(gpass = 1; gpass <= gpasscount; gpass++)
    {
        
        //
        // zero matrix, several cases
        //
        for(i = 0; i <= maxn-1; i++)
        {
            for(j = 0; j <= maxn-1; j++)
            {
                a(i,j) = 0;
            }
        }
        for(i = 1; i <= maxn; i++)
        {
            testproblem(a, i);
        }
        
        //
        // Dense matrices
        //
        for(n = 1; n <= maxn; n++)
        {
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                }
            }
            testproblem(a, n);
        }
    }
    
    //
    // report
    //
    waserrors = deterrors;
    if( !silent )
    {
        printf("TESTING DET\n");
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
Sparse matrix
*************************************************************************/
static void fillsparsea(ap::complex_2d_array& a,
     int m,
     int n,
     double sparcity)
{
    int i;
    int j;

    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            if( ap::fp_greater_eq(ap::randomreal(),sparcity) )
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
}


/*************************************************************************
Problem testing
*************************************************************************/
static void testproblem(const ap::complex_2d_array& a, int n)
{
    int i;
    int j;
    ap::complex_2d_array b;
    ap::complex_2d_array c;
    ap::integer_1d_array pivots;
    ap::complex v1;
    ap::complex v2;
    ap::complex ve;

    b.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            b(i,j) = a(i-1,j-1);
        }
    }
    ve = dettriangle(b, n);
    v1 = cmatrixdet(a, n);
    makeacopy(a, n, n, c);
    cmatrixlu(c, n, n, pivots);
    v2 = cmatrixludet(c, pivots, n);
    if( ve!=0 )
    {
        deterrors = deterrors||ap::fp_greater(ap::abscomplex((v1-ve)/ap::maxreal(ap::abscomplex(ve), double(1))),1.0E-9);
        deterrors = deterrors||ap::fp_greater(ap::abscomplex((v1-ve)/ap::maxreal(ap::abscomplex(ve), double(1))),1.0E-9);
    }
    else
    {
        deterrors = deterrors||v1!=ve;
        deterrors = deterrors||v2!=ve;
    }
}


/*************************************************************************
Copy
*************************************************************************/
static void makeacopy(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b)
{
    int i;
    int j;

    b.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            b(i,j) = a(i,j);
        }
    }
}


/*************************************************************************
Basic det subroutine
*************************************************************************/
static ap::complex dettriangle(ap::complex_2d_array a, const int& n)
{
    ap::complex result;
    int i;
    int j;
    int k;
    int l;
    int f;
    int z;
    ap::complex t;
    ap::complex m1;

    result = 1;
    k = 1;
    do
    {
        m1 = 0;
        i = k;
        while(i<=n)
        {
            t = a(i,k);
            if( ap::fp_greater(ap::abscomplex(t),ap::abscomplex(m1)) )
            {
                m1 = t;
                j = i;
            }
            i = i+1;
        }
        if( ap::fp_eq(ap::abscomplex(m1),0) )
        {
            result = 0;
            k = n+1;
        }
        else
        {
            if( j!=k )
            {
                result = -result;
                l = k;
                while(l<=n)
                {
                    t = a(j,l);
                    a(j,l) = a(k,l);
                    a(k,l) = t;
                    l = l+1;
                }
            }
            f = k+1;
            while(f<=n)
            {
                t = a(f,k)/m1;
                z = k+1;
                while(z<=n)
                {
                    a(f,z) = a(f,z)-t*a(k,z);
                    z = z+1;
                }
                f = f+1;
            }
            result = result*a(k,k);
        }
        k = k+1;
    }
    while(k<=n);
    return result;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcdetunit_test_silent()
{
    bool result;

    result = testcdet(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcdetunit_test()
{
    bool result;

    result = testcdet(false);
    return result;
}




