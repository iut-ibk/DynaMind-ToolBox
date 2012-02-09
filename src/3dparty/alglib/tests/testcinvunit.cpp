
#include <stdafx.h>
#include <stdio.h>
#include "testcinvunit.h"

static bool inverrors;
static double threshold;

static void testproblem(const ap::complex_2d_array& a, int n);
static void makeacopy(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b);
static void makeacopyoldmem(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b);
static double matrixdiff(const ap::complex_2d_array& a,
     const ap::complex_2d_array& b,
     int m,
     int n);
static void internalmatrixmatrixmultiply(const ap::complex_2d_array& a,
     int ai1,
     int ai2,
     int aj1,
     int aj2,
     bool transa,
     const ap::complex_2d_array& b,
     int bi1,
     int bi2,
     int bj1,
     int bj2,
     bool transb,
     ap::complex_2d_array& c,
     int ci1,
     int ci2,
     int cj1,
     int cj2);

/*************************************************************************
Main unittest subroutine
*************************************************************************/
bool testcinv(bool silent)
{
    bool result;
    int maxn;
    int gpasscount;
    ap::complex_2d_array a;
    int n;
    int gpass;
    int i;
    int j;
    int k;
    bool waserrors;

    inverrors = false;
    threshold = 5*1000*ap::machineepsilon;
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
            // diagonal matrix, several cases
            //
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                a(i,i).x = 2*ap::randomreal()-1;
                a(i,i).y = 2*ap::randomreal()-1;
            }
            testproblem(a, n);
            
            //
            // shifted diagonal matrix, several cases
            //
            k = ap::randominteger(n);
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= n-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                a(i,(i+k)%n).x = 2*ap::randomreal()-1;
                a(i,(i+k)%n).y = 2*ap::randomreal()-1;
            }
            testproblem(a, n);
            
            //
            // Dense matrices
            //
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
    waserrors = inverrors;
    if( !silent )
    {
        printf("TESTING COMPLEX INVERSE\n");
        printf("INVERSE ERRORS                           ");
        if( inverrors )
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
Problem testing
*************************************************************************/
static void testproblem(const ap::complex_2d_array& a, int n)
{
    ap::complex_2d_array b;
    ap::complex_2d_array blu;
    ap::complex_2d_array t1;
    ap::integer_1d_array p;
    int i;
    int j;
    ap::complex v;

    
    //
    // Decomposition
    //
    makeacopy(a, n, n, b);
    cmatrixinverse(b, n);
    makeacopy(a, n, n, blu);
    cmatrixlu(blu, n, n, p);
    cmatrixluinverse(blu, p, n);
    
    //
    // Test
    //
    t1.setbounds(0, n-1, 0, n-1);
    internalmatrixmatrixmultiply(a, 0, n-1, 0, n-1, false, b, 0, n-1, 0, n-1, false, t1, 0, n-1, 0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = t1(i,j);
            if( i==j )
            {
                v = v-1;
            }
            inverrors = inverrors||ap::fp_greater(ap::abscomplex(v),threshold);
        }
    }
    internalmatrixmatrixmultiply(a, 0, n-1, 0, n-1, false, blu, 0, n-1, 0, n-1, false, t1, 0, n-1, 0, n-1);
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = t1(i,j);
            if( i==j )
            {
                v = v-1;
            }
            inverrors = inverrors||ap::fp_greater(ap::abscomplex(v),threshold);
        }
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
Copy
*************************************************************************/
static void makeacopyoldmem(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b)
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
static double matrixdiff(const ap::complex_2d_array& a,
     const ap::complex_2d_array& b,
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
            result = ap::maxreal(result, ap::abscomplex(b(i,j)-a(i,j)));
        }
    }
    return result;
}


/*************************************************************************
Matrix multiplication
*************************************************************************/
static void internalmatrixmatrixmultiply(const ap::complex_2d_array& a,
     int ai1,
     int ai2,
     int aj1,
     int aj2,
     bool transa,
     const ap::complex_2d_array& b,
     int bi1,
     int bi2,
     int bj1,
     int bj2,
     bool transb,
     ap::complex_2d_array& c,
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
    ap::complex v;
    ap::complex_1d_array work;
    ap::complex beta;
    ap::complex alpha;
    int i_;
    int i1_;

    
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
    if( beta==0 )
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
            for(i_=cj1; i_<=cj2;i_++)
            {
                c(i,i_) = beta*c(i,i_);
            }
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
                i1_ = (bj1) - (cj1);
                for(i_=cj1; i_<=cj2;i_++)
                {
                    c(k,i_) = c(k,i_) + v*b(r,i_+i1_);
                }
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
                    i1_ = (bj1)-(aj1);
                    v = 0.0;
                    for(i_=aj1; i_<=aj2;i_++)
                    {
                        v += a(l,i_)*b(r,i_+i1_);
                    }
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
                    i1_ = (bj1)-(aj1);
                    v = 0.0;
                    for(i_=aj1; i_<=aj2;i_++)
                    {
                        v += a(l,i_)*b(r,i_+i1_);
                    }
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
                i1_ = (bj1) - (cj1);
                for(i_=cj1; i_<=cj2;i_++)
                {
                    c(k,i_) = c(k,i_) + v*b(r,i_+i1_);
                }
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
                    i1_ = (aj1) - (1);
                    for(i_=1; i_<=crows;i_++)
                    {
                        work(i_) = work(i_) + v*a(l,i_+i1_);
                    }
                }
                i1_ = (1) - (ci1);
                for(i_=ci1; i_<=ci2;i_++)
                {
                    c(i_,k) = c(i_,k) + work(i_+i1_);
                }
            }
            return;
        }
        else
        {
            for(l = aj1; l <= aj2; l++)
            {
                k = ai2-ai1+1;
                i1_ = (ai1) - (1);
                for(i_=1; i_<=k;i_++)
                {
                    work(i_) = a(i_+i1_,l);
                }
                for(r = bi1; r <= bi2; r++)
                {
                    i1_ = (bj1)-(1);
                    v = 0.0;
                    for(i_=1; i_<=k;i_++)
                    {
                        v += work(i_)*b(r,i_+i1_);
                    }
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
bool testcinvunit_test_silent()
{
    bool result;

    result = testcinv(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcinvunit_test()
{
    bool result;

    result = testcinv(false);
    return result;
}




