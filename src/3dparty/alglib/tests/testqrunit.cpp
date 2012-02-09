
#include <stdafx.h>
#include <stdio.h>
#include "testqrunit.h"

static double threshold;
static bool structerrors;
static bool decomperrors;
static bool othererrors;

static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity);
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static void testproblem(const ap::real_2d_array& a, int m, int n);

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testqr(bool silent)
{
    bool result;
    int shortmn;
    int maxmn;
    int gpasscount;
    ap::real_2d_array a;
    int m;
    int n;
    int gpass;
    int i;
    int j;
    bool waserrors;

    decomperrors = false;
    othererrors = false;
    structerrors = false;
    waserrors = false;
    shortmn = 5;
    maxmn = 15;
    gpasscount = 5;
    threshold = 5*100*ap::machineepsilon;
    a.setbounds(0, maxmn-1, 0, maxmn-1);
    
    //
    // Different problems
    //
    for(gpass = 1; gpass <= gpasscount; gpass++)
    {
        
        //
        // zero matrix, several cases
        //
        for(i = 0; i <= maxmn-1; i++)
        {
            for(j = 0; j <= maxmn-1; j++)
            {
                a(i,j) = 0;
            }
        }
        for(i = 1; i <= maxmn; i++)
        {
            for(j = 1; j <= maxmn; j++)
            {
                testproblem(a, i, j);
            }
        }
        
        //
        // Long dense matrix
        //
        for(i = 0; i <= maxmn-1; i++)
        {
            for(j = 0; j <= shortmn-1; j++)
            {
                a(i,j) = 2*ap::randomreal()-1;
            }
        }
        for(i = shortmn+1; i <= maxmn; i++)
        {
            testproblem(a, i, shortmn);
        }
        for(i = 0; i <= shortmn-1; i++)
        {
            for(j = 0; j <= maxmn-1; j++)
            {
                a(i,j) = 2*ap::randomreal()-1;
            }
        }
        for(j = shortmn+1; j <= maxmn; j++)
        {
            testproblem(a, shortmn, j);
        }
        
        //
        // Dense matrices
        //
        for(m = 1; m <= maxmn; m++)
        {
            for(n = 1; n <= maxmn; n++)
            {
                for(i = 0; i <= m-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        a(i,j) = 2*ap::randomreal()-1;
                    }
                }
                testproblem(a, m, n);
            }
        }
        
        //
        // Sparse matrices, very sparse matrices, incredible sparse matrices
        //
        for(m = 1; m <= maxmn; m++)
        {
            for(n = 1; n <= maxmn; n++)
            {
                fillsparsea(a, m, n, 0.8);
                testproblem(a, m, n);
                fillsparsea(a, m, n, 0.9);
                testproblem(a, m, n);
                fillsparsea(a, m, n, 0.95);
                testproblem(a, m, n);
            }
        }
    }
    
    //
    // report
    //
    waserrors = structerrors||decomperrors||othererrors;
    if( !silent )
    {
        printf("TESTING RMatrixQR\n");
        printf("STRUCTURAL ERRORS:                       ");
        if( !structerrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("DECOMPOSITION ERRORS:                    ");
        if( !decomperrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("OTHER ERRORS:                            ");
        if( !othererrors )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
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
Sparse fill
*************************************************************************/
static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity)
{
    int i;
    int j;

    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            if( ap::fp_greater_eq(ap::randomreal(),sparcity) )
            {
                a(i,j) = 2*ap::randomreal()-1;
            }
            else
            {
                a(i,j) = 0;
            }
        }
    }
}


/*************************************************************************
Copy
*************************************************************************/
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b)
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
Problem testing
*************************************************************************/
static void testproblem(const ap::real_2d_array& a, int m, int n)
{
    int i;
    int j;
    int k;
    double mx;
    ap::real_2d_array b;
    ap::real_1d_array taub;
    ap::real_2d_array q;
    ap::real_2d_array r;
    ap::real_2d_array q2;
    double v;

    
    //
    // MX - estimate of the matrix norm
    //
    mx = 0;
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
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
    // Test decompose-and-unpack error
    //
    makeacopy(a, m, n, b);
    rmatrixqr(b, m, n, taub);
    rmatrixqrunpackq(b, m, n, taub, m, q);
    rmatrixqrunpackr(b, m, n, r);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(q.getrow(i, 0, m-1), r.getcolumn(j, 0, m-1));
            decomperrors = decomperrors||ap::fp_greater_eq(fabs(v-a(i,j)),threshold);
        }
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= ap::minint(i, n-1)-1; j++)
        {
            structerrors = structerrors||ap::fp_neq(r(i,j),0);
        }
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= m-1; j++)
        {
            v = ap::vdotproduct(&q(i, 0), &q(j, 0), ap::vlen(0,m-1));
            if( i==j )
            {
                structerrors = structerrors||ap::fp_greater_eq(fabs(v-1),threshold);
            }
            else
            {
                structerrors = structerrors||ap::fp_greater_eq(fabs(v),threshold);
            }
        }
    }
    
    //
    // Test for other errors
    //
    for(k = 1; k <= m-1; k++)
    {
        rmatrixqrunpackq(b, m, n, taub, k, q2);
        for(i = 0; i <= m-1; i++)
        {
            for(j = 0; j <= k-1; j++)
            {
                othererrors = othererrors||ap::fp_greater(fabs(q2(i,j)-q(i,j)),10*ap::machineepsilon);
            }
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testqrunit_test_silent()
{
    bool result;

    result = testqr(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testqrunit_test()
{
    bool result;

    result = testqr(false);
    return result;
}




