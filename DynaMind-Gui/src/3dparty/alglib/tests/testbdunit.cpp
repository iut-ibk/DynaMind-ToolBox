
#include <stdafx.h>
#include <stdio.h>
#include "testbdunit.h"

static bool decomperrors;
static bool properrors;
static bool parterrors;
static bool mulerrors;
static double threshold;

static void fillsparsea(ap::real_2d_array& a, int m, int n, double sparcity);
static void testproblem(const ap::real_2d_array& a, int m, int n);
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
bool testbd(bool silent)
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
    properrors = false;
    mulerrors = false;
    parterrors = false;
    threshold = 5*100*ap::machineepsilon;
    waserrors = false;
    shortmn = 5;
    maxmn = 10;
    gpasscount = 5;
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
    waserrors = decomperrors||properrors||parterrors||mulerrors;
    if( !silent )
    {
        printf("TESTING 2BIDIAGONAL\n");
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
        printf("PARTIAL UNPACKING                        ");
        if( parterrors )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("MULTIPLICATION TEST                      ");
        if( mulerrors )
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
static void testproblem(const ap::real_2d_array& a, int m, int n)
{
    int i;
    int j;
    int k;
    double mx;
    ap::real_2d_array t;
    ap::real_2d_array pt;
    ap::real_2d_array q;
    ap::real_2d_array r;
    ap::real_2d_array bd;
    ap::real_2d_array x;
    ap::real_2d_array r1;
    ap::real_2d_array r2;
    ap::real_1d_array taup;
    ap::real_1d_array tauq;
    ap::real_1d_array d;
    ap::real_1d_array e;
    bool up;
    double v;
    int mtsize;

    
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
    // Bidiagonal decomposition error
    //
    makeacopy(a, m, n, t);
    rmatrixbd(t, m, n, tauq, taup);
    rmatrixbdunpackq(t, m, n, tauq, m, q);
    rmatrixbdunpackpt(t, m, n, taup, n, pt);
    rmatrixbdunpackdiagonals(t, m, n, up, d, e);
    bd.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            bd(i,j) = 0;
        }
    }
    for(i = 0; i <= ap::minint(m, n)-1; i++)
    {
        bd(i,i) = d(i);
    }
    if( up )
    {
        for(i = 0; i <= ap::minint(m, n)-2; i++)
        {
            bd(i,i+1) = e(i);
        }
    }
    else
    {
        for(i = 0; i <= ap::minint(m, n)-2; i++)
        {
            bd(i+1,i) = e(i);
        }
    }
    r.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(q.getrow(i, 0, m-1), bd.getcolumn(j, 0, m-1));
            r(i,j) = v;
        }
    }
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(r.getrow(i, 0, n-1), pt.getcolumn(j, 0, n-1));
            decomperrors = decomperrors||ap::fp_greater(fabs(v-a(i,j)),threshold);
        }
    }
    
    //
    // Orthogonality test for Q/PT
    //
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= m-1; j++)
        {
            v = ap::vdotproduct(q.getcolumn(i, 0, m-1), q.getcolumn(j, 0, m-1));
            if( i==j )
            {
                properrors = properrors||ap::fp_greater(fabs(v-1),threshold);
            }
            else
            {
                properrors = properrors||ap::fp_greater(fabs(v),threshold);
            }
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(&pt(i, 0), &pt(j, 0), ap::vlen(0,n-1));
            if( i==j )
            {
                properrors = properrors||ap::fp_greater(fabs(v-1),threshold);
            }
            else
            {
                properrors = properrors||ap::fp_greater(fabs(v),threshold);
            }
        }
    }
    
    //
    // Partial unpacking test
    //
    for(k = 1; k <= m-1; k++)
    {
        rmatrixbdunpackq(t, m, n, tauq, k, r);
        for(i = 0; i <= m-1; i++)
        {
            for(j = 0; j <= k-1; j++)
            {
                parterrors = parterrors||ap::fp_greater(fabs(r(i,j)-q(i,j)),10*ap::machineepsilon);
            }
        }
    }
    for(k = 1; k <= n-1; k++)
    {
        rmatrixbdunpackpt(t, m, n, taup, k, r);
        for(i = 0; i <= k-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                parterrors = parterrors||ap::fp_neq(r(i,j)-pt(i,j),0);
            }
        }
    }
    
    //
    // Multiplication test
    //
    x.setbounds(0, ap::maxint(m, n)-1, 0, ap::maxint(m, n)-1);
    r.setbounds(0, ap::maxint(m, n)-1, 0, ap::maxint(m, n)-1);
    r1.setbounds(0, ap::maxint(m, n)-1, 0, ap::maxint(m, n)-1);
    r2.setbounds(0, ap::maxint(m, n)-1, 0, ap::maxint(m, n)-1);
    for(i = 0; i <= ap::maxint(m, n)-1; i++)
    {
        for(j = 0; j <= ap::maxint(m, n)-1; j++)
        {
            x(i,j) = 2*ap::randomreal()-1;
        }
    }
    mtsize = 1+ap::randominteger(ap::maxint(m, n));
    makeacopyoldmem(x, mtsize, m, r);
    internalmatrixmatrixmultiply(r, 0, mtsize-1, 0, m-1, false, q, 0, m-1, 0, m-1, false, r1, 0, mtsize-1, 0, m-1);
    makeacopyoldmem(x, mtsize, m, r2);
    rmatrixbdmultiplybyq(t, m, n, tauq, r2, mtsize, m, true, false);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, mtsize, m),threshold);
    makeacopyoldmem(x, mtsize, m, r);
    internalmatrixmatrixmultiply(r, 0, mtsize-1, 0, m-1, false, q, 0, m-1, 0, m-1, true, r1, 0, mtsize-1, 0, m-1);
    makeacopyoldmem(x, mtsize, m, r2);
    rmatrixbdmultiplybyq(t, m, n, tauq, r2, mtsize, m, true, true);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, mtsize, m),threshold);
    makeacopyoldmem(x, m, mtsize, r);
    internalmatrixmatrixmultiply(q, 0, m-1, 0, m-1, false, r, 0, m-1, 0, mtsize-1, false, r1, 0, m-1, 0, mtsize-1);
    makeacopyoldmem(x, m, mtsize, r2);
    rmatrixbdmultiplybyq(t, m, n, tauq, r2, m, mtsize, false, false);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, m, mtsize),threshold);
    makeacopyoldmem(x, m, mtsize, r);
    internalmatrixmatrixmultiply(q, 0, m-1, 0, m-1, true, r, 0, m-1, 0, mtsize-1, false, r1, 0, m-1, 0, mtsize-1);
    makeacopyoldmem(x, m, mtsize, r2);
    rmatrixbdmultiplybyq(t, m, n, tauq, r2, m, mtsize, false, true);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, m, mtsize),threshold);
    makeacopyoldmem(x, mtsize, n, r);
    internalmatrixmatrixmultiply(r, 0, mtsize-1, 0, n-1, false, pt, 0, n-1, 0, n-1, true, r1, 0, mtsize-1, 0, n-1);
    makeacopyoldmem(x, mtsize, n, r2);
    rmatrixbdmultiplybyp(t, m, n, taup, r2, mtsize, n, true, false);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, mtsize, n),threshold);
    makeacopyoldmem(x, mtsize, n, r);
    internalmatrixmatrixmultiply(r, 0, mtsize-1, 0, n-1, false, pt, 0, n-1, 0, n-1, false, r1, 0, mtsize-1, 0, n-1);
    makeacopyoldmem(x, mtsize, n, r2);
    rmatrixbdmultiplybyp(t, m, n, taup, r2, mtsize, n, true, true);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, mtsize, n),threshold);
    makeacopyoldmem(x, n, mtsize, r);
    internalmatrixmatrixmultiply(pt, 0, n-1, 0, n-1, true, r, 0, n-1, 0, mtsize-1, false, r1, 0, n-1, 0, mtsize-1);
    makeacopyoldmem(x, n, mtsize, r2);
    rmatrixbdmultiplybyp(t, m, n, taup, r2, n, mtsize, false, false);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, n, mtsize),threshold);
    makeacopyoldmem(x, n, mtsize, r);
    internalmatrixmatrixmultiply(pt, 0, n-1, 0, n-1, false, r, 0, n-1, 0, mtsize-1, false, r1, 0, n-1, 0, mtsize-1);
    makeacopyoldmem(x, n, mtsize, r2);
    rmatrixbdmultiplybyp(t, m, n, taup, r2, n, mtsize, false, true);
    mulerrors = mulerrors||ap::fp_greater(matrixdiff(r1, r2, n, mtsize),threshold);
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
bool testbdunit_test_silent()
{
    bool result;

    result = testbd(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testbdunit_test()
{
    bool result;

    result = testbd(false);
    return result;
}




