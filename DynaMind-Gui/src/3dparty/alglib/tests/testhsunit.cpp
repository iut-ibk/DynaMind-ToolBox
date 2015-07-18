
#include <stdafx.h>
#include <stdio.h>
#include "testhsunit.h"

static bool decomperrors;
static bool properrors;
static double threshold;

static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity);
static void testproblem(const ap::real_2d_array& a, int n);
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static void makeacopyoldmem(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static double matrixdiff(const ap::real_2d_array& a,
     const ap::real_2d_array& b,
     int m,
     int n);
static void internalmatrixmatrixmultiply(const ap::real_2d_array& a,
     int ai1,
     int ai2,
     int aj1,
     int aj2,
     bool transa,
     const ap::real_2d_array& b,
     int bi1,
     int bi2,
     int bj1,
     int bj2,
     bool transb,
     ap::real_2d_array& c,
     int ci1,
     int ci2,
     int cj1,
     int cj2);

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool tesths(bool silent)
{
    bool result;
    int maxn;
    int gpasscount;
    ap::real_2d_array a;
    int n;
    int gpass;
    int i;
    int j;
    bool waserrors;

    decomperrors = false;
    properrors = false;
    threshold = 5*100*ap::machineepsilon;
    waserrors = false;
    maxn = 10;
    gpasscount = 30;
    
    //
    // Different problems
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        for(gpass = 1; gpass <= gpasscount; gpass++)
        {
            
            //
            // zero matrix, several cases
            //
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            testproblem(a, n);
            
            //
            // Dense matrices
            //
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
            }
            testproblem(a, n);
            
            //
            // Sparse matrices, very sparse matrices, incredible sparse matrices
            //
            fillsparsea(a, n, n, 0.8);
            testproblem(a, n);
            fillsparsea(a, n, n, 0.9);
            testproblem(a, n);
            fillsparsea(a, n, n, 0.95);
            testproblem(a, n);
        }
    }
    
    //
    // report
    //
    waserrors = decomperrors||properrors;
    if( !silent )
    {
        printf("TESTING 2HESSENBERG\n");
        printf("DECOMPOSITION ERRORS                     ");
        if( decomperrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("MATRIX PROPERTIES                        ");
        if( properrors )
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
Sparse matrix
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
Problem testing
*************************************************************************/
static void testproblem(const ap::real_2d_array& a, int n)
{
    ap::real_2d_array b;
    ap::real_2d_array h;
    ap::real_2d_array q;
    ap::real_2d_array t1;
    ap::real_2d_array t2;
    ap::real_1d_array tau;
    int i;
    int j;
    double v;

    makeacopy(a, n, n, b);
    
    //
    // Decomposition
    //
    rmatrixhessenberg(b, n, tau);
    rmatrixhessenbergunpackq(b, n, tau, q);
    rmatrixhessenbergunpackh(b, n, h);
    
    //
    // Matrix properties
    //
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(q.getcolumn(i, 0, n-1), q.getcolumn(j, 0, n-1));
            if( i==j )
            {
                v = v-1;
            }
            properrors = properrors||ap::fp_greater(fabs(v),threshold);
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= i-2; j++)
        {
            properrors = properrors||ap::fp_neq(h(i,j),0);
        }
    }
    
    //
    // Decomposition error
    //
    t1.setbounds(0, n-1, 0, n-1);
    t2.setbounds(0, n-1, 0, n-1);
    internalmatrixmatrixmultiply(q, 0, n-1, 0, n-1, false, h, 0, n-1, 0, n-1, false, t1, 0, n-1, 0, n-1);
    internalmatrixmatrixmultiply(t1, 0, n-1, 0, n-1, false, q, 0, n-1, 0, n-1, true, t2, 0, n-1, 0, n-1);
    decomperrors = decomperrors||ap::fp_greater(matrixdiff(t2, a, n, n),threshold);
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
Copy
*************************************************************************/
static void makeacopyoldmem(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b)
{
    int i;
    int j;

    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            b(i,j) = a(i,j);
        }
    }
}


/*************************************************************************
Diff
*************************************************************************/
static double matrixdiff(const ap::real_2d_array& a,
     const ap::real_2d_array& b,
     int m,
     int n)
{
    double result;
    int i;
    int j;

    result = 0;
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            result = ap::maxreal(result, fabs(b(i,j)-a(i,j)));
        }
    }
    return result;
}


/*************************************************************************
Matrix multiplication
*************************************************************************/
static void internalmatrixmatrixmultiply(const ap::real_2d_array& a,
     int ai1,
     int ai2,
     int aj1,
     int aj2,
     bool transa,
     const ap::real_2d_array& b,
     int bi1,
     int bi2,
     int bj1,
     int bj2,
     bool transb,
     ap::real_2d_array& c,
     int ci1,
     int ci2,
     int cj1,
     int cj2)
{
    int arows;
    int acols;
    int brows;
    int bcols;
    int crows;
    int ccols;
    int i;
    int j;
    int k;
    int l;
    int r;
    double v;
    ap::real_1d_array work;
    double beta;
    double alpha;

    
    //
    // Pre-setup
    //
    k = ap::maxint(ai2-ai1+1, aj2-aj1+1);
    k = ap::maxint(k, bi2-bi1+1);
    k = ap::maxint(k, bj2-bj1+1);
    work.setbounds(1, k);
    beta = 0;
    alpha = 1;
    
    //
    // Setup
    //
    if( !transa )
    {
        arows = ai2-ai1+1;
        acols = aj2-aj1+1;
    }
    else
    {
        arows = aj2-aj1+1;
        acols = ai2-ai1+1;
    }
    if( !transb )
    {
        brows = bi2-bi1+1;
        bcols = bj2-bj1+1;
    }
    else
    {
        brows = bj2-bj1+1;
        bcols = bi2-bi1+1;
    }
    ap::ap_error::make_assertion(acols==brows, "MatrixMatrixMultiply: incorrect matrix sizes!");
    if( arows<=0||acols<=0||brows<=0||bcols<=0 )
    {
        return;
    }
    crows = arows;
    ccols = bcols;
    
    //
    // Test WORK
    //
    i = ap::maxint(arows, acols);
    i = ap::maxint(brows, i);
    i = ap::maxint(i, bcols);
    work(1) = 0;
    work(i) = 0;
    
    //
    // Prepare C
    //
    if( ap::fp_eq(beta,0) )
    {
        for(i = ci1; i <= ci2; i++)
        {
            for(j = cj1; j <= cj2; j++)
            {
                c(i,j) = 0;
            }
        }
    }
    else
    {
        for(i = ci1; i <= ci2; i++)
        {
            ap::vmul(&c(i, cj1), ap::vlen(cj1,cj2), beta);
        }
    }
    
    //
    // A*B
    //
    if( !transa&&!transb )
    {
        for(l = ai1; l <= ai2; l++)
        {
            for(r = bi1; r <= bi2; r++)
            {
                v = alpha*a(l,aj1+r-bi1);
                k = ci1+l-ai1;
                ap::vadd(&c(k, cj1), &b(r, bj1), ap::vlen(cj1,cj2), v);
            }
        }
        return;
    }
    
    //
    // A*B'
    //
    if( !transa&&transb )
    {
        if( arows*acols<brows*bcols )
        {
            for(r = bi1; r <= bi2; r++)
            {
                for(l = ai1; l <= ai2; l++)
                {
                    v = ap::vdotproduct(&a(l, aj1), &b(r, bj1), ap::vlen(aj1,aj2));
                    c(ci1+l-ai1,cj1+r-bi1) = c(ci1+l-ai1,cj1+r-bi1)+alpha*v;
                }
            }
            return;
        }
        else
        {
            for(l = ai1; l <= ai2; l++)
            {
                for(r = bi1; r <= bi2; r++)
                {
                    v = ap::vdotproduct(&a(l, aj1), &b(r, bj1), ap::vlen(aj1,aj2));
                    c(ci1+l-ai1,cj1+r-bi1) = c(ci1+l-ai1,cj1+r-bi1)+alpha*v;
                }
            }
            return;
        }
    }
    
    //
    // A'*B
    //
    if( transa&&!transb )
    {
        for(l = aj1; l <= aj2; l++)
        {
            for(r = bi1; r <= bi2; r++)
            {
                v = alpha*a(ai1+r-bi1,l);
                k = ci1+l-aj1;
                ap::vadd(&c(k, cj1), &b(r, bj1), ap::vlen(cj1,cj2), v);
            }
        }
        return;
    }
    
    //
    // A'*B'
    //
    if( transa&&transb )
    {
        if( arows*acols<brows*bcols )
        {
            for(r = bi1; r <= bi2; r++)
            {
                for(i = 1; i <= crows; i++)
                {
                    work(i) = 0.0;
                }
                for(l = ai1; l <= ai2; l++)
                {
                    v = alpha*b(r,bj1+l-ai1);
                    k = cj1+r-bi1;
                    ap::vadd(&work(1), &a(l, aj1), ap::vlen(1,crows), v);
                }
                ap::vadd(c.getcolumn(k, ci1, ci2), work.getvector(1, crows));
            }
            return;
        }
        else
        {
            for(l = aj1; l <= aj2; l++)
            {
                k = ai2-ai1+1;
                ap::vmove(work.getvector(1, k), a.getcolumn(l, ai1, ai2));
                for(r = bi1; r <= bi2; r++)
                {
                    v = ap::vdotproduct(&work(1), &b(r, bj1), ap::vlen(1,k));
                    c(ci1+l-aj1,cj1+r-bi1) = c(ci1+l-aj1,cj1+r-bi1)+alpha*v;
                }
            }
            return;
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhsunit_test_silent()
{
    bool result;

    result = tesths(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testhsunit_test()
{
    bool result;

    result = tesths(false);
    return result;
}




