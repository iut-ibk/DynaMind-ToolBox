
#include <stdafx.h>
#include <stdio.h>
#include "testcrcondunit.h"

static void makeacopy(const ap::complex_2d_array& a,
     int m,
     int n,
     ap::complex_2d_array& b);
static void generaterandommatrix(ap::complex_2d_array& a0, int n);
static bool invmattr(ap::complex_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular);
static bool invmatlu(ap::complex_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n);
static bool invmat(ap::complex_2d_array& a, int n);
static void refrcond(const ap::complex_2d_array& a,
     int n,
     double& rc1,
     double& rcinf);

bool testcrcond(bool silent)
{
    bool result;
    ap::complex_2d_array a;
    ap::complex_2d_array lua;
    ap::integer_1d_array p;
    int n;
    int maxn;
    int i;
    int j;
    int pass;
    int passcount;
    bool waserrors;
    bool err50;
    bool err90;
    bool errless;
    double erc1;
    double ercinf;
    ap::real_1d_array q50;
    ap::real_1d_array q90;
    double v;
    double threshold50;
    double threshold90;

    waserrors = false;
    err50 = false;
    err90 = false;
    errless = false;
    maxn = 10;
    passcount = 100;
    threshold50 = 0.5;
    threshold90 = 0.1;
    q50.setbounds(0, 3);
    q90.setbounds(0, 3);
    
    //
    // process
    //
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        for(i = 0; i <= 3; i++)
        {
            q50(i) = 0;
            q90(i) = 0;
        }
        for(pass = 1; pass <= passcount; pass++)
        {
            generaterandommatrix(a, n);
            makeacopy(a, n, n, lua);
            cmatrixlu(lua, n, n, p);
            refrcond(a, n, erc1, ercinf);
            
            //
            // 1-norm, normal
            //
            v = 1/cmatrixrcond1(a, n);
            if( ap::fp_greater_eq(v,threshold50*erc1) )
            {
                q50(0) = q50(0)+double(1)/double(passcount);
            }
            if( ap::fp_greater_eq(v,threshold90*erc1) )
            {
                q90(0) = q90(0)+double(1)/double(passcount);
            }
            errless = errless||ap::fp_greater(v,erc1*1.001);
            
            //
            // 1-norm, LU
            //
            v = 1/cmatrixlurcond1(lua, n);
            if( ap::fp_greater_eq(v,threshold50*erc1) )
            {
                q50(1) = q50(1)+double(1)/double(passcount);
            }
            if( ap::fp_greater_eq(v,threshold90*erc1) )
            {
                q90(1) = q90(1)+double(1)/double(passcount);
            }
            errless = errless||ap::fp_greater(v,erc1*1.001);
            
            //
            // Inf-norm, normal
            //
            v = 1/cmatrixrcondinf(a, n);
            if( ap::fp_greater_eq(v,threshold50*ercinf) )
            {
                q50(2) = q50(2)+double(1)/double(passcount);
            }
            if( ap::fp_greater_eq(v,threshold90*ercinf) )
            {
                q90(2) = q90(2)+double(1)/double(passcount);
            }
            errless = errless||ap::fp_greater(v,ercinf*1.001);
            
            //
            // Inf-norm, LU
            //
            v = 1/cmatrixlurcondinf(lua, n);
            if( ap::fp_greater_eq(v,threshold50*ercinf) )
            {
                q50(3) = q50(3)+double(1)/double(passcount);
            }
            if( ap::fp_greater_eq(v,threshold90*ercinf) )
            {
                q90(3) = q90(3)+double(1)/double(passcount);
            }
            errless = errless||ap::fp_greater(v,ercinf*1.001);
        }
        for(i = 0; i <= 3; i++)
        {
            err50 = err50||ap::fp_less(q50(i),0.50);
            err90 = err90||ap::fp_less(q90(i),0.90);
        }
    }
    
    //
    // report
    //
    waserrors = err50||err90||errless;
    if( !silent )
    {
        printf("TESTING RCOND (COMPLEX)\n");
        printf("50%% within [0.5*cond,cond]:              ");
        if( err50||errless )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("90%% within [0.1*cond,cond]               ");
        if( err90||errless )
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
Generate matrix with given condition number C (2-norm)
*************************************************************************/
static void generaterandommatrix(ap::complex_2d_array& a0, int n)
{
    int i;
    int j;

    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            a0(i,j).x = 2*ap::randomreal()-1;
            a0(i,j).y = 2*ap::randomreal()-1;
        }
    }
}


/*************************************************************************
triangular inverse
*************************************************************************/
static bool invmattr(ap::complex_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular)
{
    bool result;
    bool nounit;
    int i;
    int j;
    ap::complex v;
    ap::complex ajj;
    ap::complex_1d_array t;
    int i_;

    result = true;
    t.setbounds(0, n-1);
    
    //
    // Test the input parameters.
    //
    nounit = !isunittriangular;
    if( isupper )
    {
        
        //
        // Compute inverse of upper triangular matrix.
        //
        for(j = 0; j <= n-1; j++)
        {
            if( nounit )
            {
                if( a(j,j)==0 )
                {
                    result = false;
                    return result;
                }
                a(j,j) = 1/a(j,j);
                ajj = -a(j,j);
            }
            else
            {
                ajj = -1;
            }
            
            //
            // Compute elements 1:j-1 of j-th column.
            //
            if( j>0 )
            {
                for(i_=0; i_<=j-1;i_++)
                {
                    t(i_) = a(i_,j);
                }
                for(i = 0; i <= j-1; i++)
                {
                    if( i<j-1 )
                    {
                        v = 0.0;
                        for(i_=i+1; i_<=j-1;i_++)
                        {
                            v += a(i,i_)*t(i_);
                        }
                    }
                    else
                    {
                        v = 0;
                    }
                    if( nounit )
                    {
                        a(i,j) = v+a(i,i)*t(i);
                    }
                    else
                    {
                        a(i,j) = v+t(i);
                    }
                }
                for(i_=0; i_<=j-1;i_++)
                {
                    a(i_,j) = ajj*a(i_,j);
                }
            }
        }
    }
    else
    {
        
        //
        // Compute inverse of lower triangular matrix.
        //
        for(j = n-1; j >= 0; j--)
        {
            if( nounit )
            {
                if( a(j,j)==0 )
                {
                    result = false;
                    return result;
                }
                a(j,j) = 1/a(j,j);
                ajj = -a(j,j);
            }
            else
            {
                ajj = -1;
            }
            if( j<n-1 )
            {
                
                //
                // Compute elements j+1:n of j-th column.
                //
                for(i_=j+1; i_<=n-1;i_++)
                {
                    t(i_) = a(i_,j);
                }
                for(i = j+1; i <= n-1; i++)
                {
                    if( i>j+1 )
                    {
                        v = 0.0;
                        for(i_=j+1; i_<=i-1;i_++)
                        {
                            v += a(i,i_)*t(i_);
                        }
                    }
                    else
                    {
                        v = 0;
                    }
                    if( nounit )
                    {
                        a(i,j) = v+a(i,i)*t(i);
                    }
                    else
                    {
                        a(i,j) = v+t(i);
                    }
                }
                for(i_=j+1; i_<=n-1;i_++)
                {
                    a(i_,j) = ajj*a(i_,j);
                }
            }
        }
    }
    return result;
}


/*************************************************************************
LU inverse
*************************************************************************/
static bool invmatlu(ap::complex_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n)
{
    bool result;
    ap::complex_1d_array work;
    int i;
    int iws;
    int j;
    int jb;
    int jj;
    int jp;
    ap::complex v;
    int i_;

    result = true;
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    work.setbounds(0, n-1);
    
    //
    // Form inv(U)
    //
    if( !invmattr(a, n, true, false) )
    {
        result = false;
        return result;
    }
    
    //
    // Solve the equation inv(A)*L = inv(U) for inv(A).
    //
    for(j = n-1; j >= 0; j--)
    {
        
        //
        // Copy current column of L to WORK and replace with zeros.
        //
        for(i = j+1; i <= n-1; i++)
        {
            work(i) = a(i,j);
            a(i,j) = 0;
        }
        
        //
        // Compute current column of inv(A).
        //
        if( j<n-1 )
        {
            for(i = 0; i <= n-1; i++)
            {
                v = 0.0;
                for(i_=j+1; i_<=n-1;i_++)
                {
                    v += a(i,i_)*work(i_);
                }
                a(i,j) = a(i,j)-v;
            }
        }
    }
    
    //
    // Apply column interchanges.
    //
    for(j = n-2; j >= 0; j--)
    {
        jp = pivots(j);
        if( jp!=j )
        {
            for(i_=0; i_<=n-1;i_++)
            {
                work(i_) = a(i_,j);
            }
            for(i_=0; i_<=n-1;i_++)
            {
                a(i_,j) = a(i_,jp);
            }
            for(i_=0; i_<=n-1;i_++)
            {
                a(i_,jp) = work(i_);
            }
        }
    }
    return result;
}


/*************************************************************************
Matrix inverse
*************************************************************************/
static bool invmat(ap::complex_2d_array& a, int n)
{
    bool result;
    ap::integer_1d_array pivots;

    cmatrixlu(a, n, n, pivots);
    result = invmatlu(a, pivots, n);
    return result;
}


/*************************************************************************
reference RCond
*************************************************************************/
static void refrcond(const ap::complex_2d_array& a,
     int n,
     double& rc1,
     double& rcinf)
{
    ap::complex_2d_array inva;
    double nrm1a;
    double nrminfa;
    double nrm1inva;
    double nrminfinva;
    double v;
    int k;
    int i;

    
    //
    // inv A
    //
    makeacopy(a, n, n, inva);
    if( !invmat(inva, n) )
    {
        rc1 = 0;
        rcinf = 0;
        return;
    }
    
    //
    // norm A
    //
    nrm1a = 0;
    nrminfa = 0;
    for(k = 0; k <= n-1; k++)
    {
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+ap::abscomplex(a(i,k));
        }
        nrm1a = ap::maxreal(nrm1a, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+ap::abscomplex(a(k,i));
        }
        nrminfa = ap::maxreal(nrminfa, v);
    }
    
    //
    // norm inv A
    //
    nrm1inva = 0;
    nrminfinva = 0;
    for(k = 0; k <= n-1; k++)
    {
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+ap::abscomplex(inva(i,k));
        }
        nrm1inva = ap::maxreal(nrm1inva, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+ap::abscomplex(inva(k,i));
        }
        nrminfinva = ap::maxreal(nrminfinva, v);
    }
    
    //
    // result
    //
    rc1 = nrm1inva*nrm1a;
    rcinf = nrminfinva*nrminfa;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testcrcondunit_test_silent()
{
    bool result;

    result = testcrcond(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testcrcondunit_test()
{
    bool result;

    result = testcrcond(false);
    return result;
}




