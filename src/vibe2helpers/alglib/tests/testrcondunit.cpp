
#include <stdafx.h>
#include <stdio.h>
#include "testrcondunit.h"

static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static void generaterandommatrixcond(ap::real_2d_array& a0, int n, double c);
static bool invmattr(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular);
static bool invmatlu(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n);
static bool invmat(ap::real_2d_array& a, int n);
static void refrcond(const ap::real_2d_array& a,
     int n,
     double& rc1,
     double& rcinf);

bool testrcond(bool silent)
{
    bool result;
    ap::real_2d_array a;
    ap::real_2d_array lua;
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
            generaterandommatrixcond(a, n, exp(ap::randomreal()*log(double(1000))));
            makeacopy(a, n, n, lua);
            rmatrixlu(lua, n, n, p);
            refrcond(a, n, erc1, ercinf);
            
            //
            // 1-norm, normal
            //
            v = 1/rmatrixrcond1(a, n);
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
            v = 1/rmatrixlurcond1(lua, n);
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
            v = 1/rmatrixrcondinf(a, n);
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
            v = 1/rmatrixlurcondinf(lua, n);
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
        printf("TESTING RCOND (REAL)\n");
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
Generate matrix with given condition number C (2-norm)
*************************************************************************/
static void generaterandommatrixcond(ap::real_2d_array& a0, int n, double c)
{
    double t;
    double lambda;
    int s;
    int i;
    int j;
    double u1;
    double u2;
    ap::real_1d_array w;
    ap::real_1d_array v;
    double sm;
    double l1;
    double l2;
    ap::real_2d_array a;

    if( n<=0||ap::fp_less(c,1) )
    {
        return;
    }
    a.setbounds(1, n, 1, n);
    a0.setbounds(0, n-1, 0, n-1);
    if( n==1 )
    {
        a0(0,0) = 1;
        return;
    }
    w.setbounds(1, n);
    v.setbounds(1, n);
    
    //
    // N>=2, prepare A
    //
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            a(i,j) = 0;
        }
    }
    l1 = 0;
    l2 = log(1/c);
    a(1,1) = exp(l1);
    for(i = 2; i <= n-1; i++)
    {
        a(i,i) = exp(ap::randomreal()*(l2-l1)+l1);
    }
    a(n,n) = exp(l2);
    
    //
    // Multiply A by random Q from the right (using Stewart algorithm)
    //
    for(s = 2; s <= n; s++)
    {
        
        //
        // Prepare v and Lambda = v'*v
        //
        do
        {
            i = 1;
            while(i<=s)
            {
                u1 = 2*ap::randomreal()-1;
                u2 = 2*ap::randomreal()-1;
                sm = u1*u1+u2*u2;
                if( ap::fp_eq(sm,0)||ap::fp_greater(sm,1) )
                {
                    continue;
                }
                sm = sqrt(-2*log(sm)/sm);
                v(i) = u1*sm;
                if( i+1<=s )
                {
                    v(i+1) = u2*sm;
                }
                i = i+2;
            }
            lambda = ap::vdotproduct(&v(1), &v(1), ap::vlen(1,s));
        }
        while(ap::fp_eq(lambda,0));
        lambda = 2/lambda;
        
        //
        // A * (I - 2 vv'/v'v ) =
        //   = A - (2/v'v) * A * v * v' =
        //   = A - (2/v'v) * w * v'
        //  where w = Av
        //
        // Note that size of A is SxS, not SxN
        // due to A structure!!!
        //
        for(i = 1; i <= s; i++)
        {
            t = ap::vdotproduct(&a(i, 1), &v(1), ap::vlen(1,s));
            w(i) = t;
        }
        for(i = 1; i <= s; i++)
        {
            t = w(i)*lambda;
            ap::vsub(&a(i, 1), &v(1), ap::vlen(1,s), t);
        }
    }
    
    //
    // Multiply A by random Q from the left (using Stewart algorithm)
    //
    for(s = 2; s <= n; s++)
    {
        
        //
        // Prepare v and Lambda = v'*v
        //
        do
        {
            i = 1;
            while(i<=s)
            {
                u1 = 2*ap::randomreal()-1;
                u2 = 2*ap::randomreal()-1;
                sm = u1*u1+u2*u2;
                if( ap::fp_eq(sm,0)||ap::fp_greater(sm,1) )
                {
                    continue;
                }
                sm = sqrt(-2*log(sm)/sm);
                v(i) = u1*sm;
                if( i+1<=s )
                {
                    v(i+1) = u2*sm;
                }
                i = i+2;
            }
            lambda = ap::vdotproduct(&v(1), &v(1), ap::vlen(1,s));
        }
        while(ap::fp_eq(lambda,0));
        lambda = 2/lambda;
        
        //
        // (I - 2 vv'/v'v ) * A =
        //   = A - (2/v'v) * v * v' * A =
        //   = A - (2/v'v) * v * w
        //  where w = v'A
        //
        // Note that size of A is SxN, not SxS!!!
        //
        for(i = 1; i <= n; i++)
        {
            w(i) = 0;
        }
        for(i = 1; i <= s; i++)
        {
            t = v(i);
            ap::vadd(&w(1), &a(i, 1), ap::vlen(1,n), t);
        }
        for(i = 1; i <= s; i++)
        {
            t = v(i)*lambda;
            ap::vsub(&a(i, 1), &w(1), ap::vlen(1,n), t);
        }
    }
    
    //
    //
    //
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            a0(i-1,j-1) = a(i,j);
        }
    }
}


/*************************************************************************
triangular inverse
*************************************************************************/
static bool invmattr(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular)
{
    bool result;
    bool nounit;
    int i;
    int j;
    double v;
    double ajj;
    ap::real_1d_array t;

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
                if( ap::fp_eq(a(j,j),0) )
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
                ap::vmove(t.getvector(0, j-1), a.getcolumn(j, 0, j-1));
                for(i = 0; i <= j-1; i++)
                {
                    if( i<j-1 )
                    {
                        v = ap::vdotproduct(&a(i, i+1), &t(i+1), ap::vlen(i+1,j-1));
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
                ap::vmul(a.getcolumn(j, 0, j-1), ajj);
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
                if( ap::fp_eq(a(j,j),0) )
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
                ap::vmove(t.getvector(j+1, n-1), a.getcolumn(j, j+1, n-1));
                for(i = j+1; i <= n-1; i++)
                {
                    if( i>j+1 )
                    {
                        v = ap::vdotproduct(&a(i, j+1), &t(j+1), ap::vlen(j+1,i-1));
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
                ap::vmul(a.getcolumn(j, j+1, n-1), ajj);
            }
        }
    }
    return result;
}


/*************************************************************************
LU inverse
*************************************************************************/
static bool invmatlu(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n)
{
    bool result;
    ap::real_1d_array work;
    int i;
    int iws;
    int j;
    int jb;
    int jj;
    int jp;
    double v;

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
                v = ap::vdotproduct(&a(i, j+1), &work(j+1), ap::vlen(j+1,n-1));
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
            ap::vmove(work.getvector(0, n-1), a.getcolumn(j, 0, n-1));
            ap::vmove(a.getcolumn(j, 0, n-1), a.getcolumn(jp, 0, n-1));
            ap::vmove(a.getcolumn(jp, 0, n-1), work.getvector(0, n-1));
        }
    }
    return result;
}


/*************************************************************************
Matrix inverse
*************************************************************************/
static bool invmat(ap::real_2d_array& a, int n)
{
    bool result;
    ap::integer_1d_array pivots;

    rmatrixlu(a, n, n, pivots);
    result = invmatlu(a, pivots, n);
    return result;
}


/*************************************************************************
reference RCond
*************************************************************************/
static void refrcond(const ap::real_2d_array& a,
     int n,
     double& rc1,
     double& rcinf)
{
    ap::real_2d_array inva;
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
            v = v+fabs(a(i,k));
        }
        nrm1a = ap::maxreal(nrm1a, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(a(k,i));
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
            v = v+fabs(inva(i,k));
        }
        nrm1inva = ap::maxreal(nrm1inva, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(inva(k,i));
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
bool testrcondunit_test_silent()
{
    bool result;

    result = testrcond(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testrcondunit_test()
{
    bool result;

    result = testrcond(false);
    return result;
}




