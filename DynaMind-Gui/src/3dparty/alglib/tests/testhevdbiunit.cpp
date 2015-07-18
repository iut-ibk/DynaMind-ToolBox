
#include <stdafx.h>
#include <stdio.h>
#include "testhevdbiunit.h"

static void unset2d(ap::complex_2d_array& a);
static void unset1d(ap::real_1d_array& a);
static bool refevd(ap::complex_2d_array a,
     int n,
     ap::real_1d_array& lambda,
     ap::complex_2d_array& z);
static bool hbitridiagonalqlieigenvaluesandvectors(ap::real_1d_array& d,
     ap::real_1d_array e,
     int n,
     ap::complex_2d_array& z);
static double testproduct(const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     const ap::real_2d_array& z,
     const ap::real_1d_array& lambda);
static double testort(const ap::real_2d_array& z, int n);
static void testevdproblem(const ap::complex_2d_array& afull,
     const ap::complex_2d_array& al,
     const ap::complex_2d_array& au,
     int n,
     double& valerr,
     double& vecerr,
     bool& wnsorted,
     int& failc);

/*************************************************************************
Testing hermitian EVD, BI
*************************************************************************/
bool testhevdbi(bool silent)
{
    bool result;
    ap::complex_2d_array a;
    ap::complex_2d_array al;
    ap::complex_2d_array au;
    int pass;
    int n;
    int i;
    int j;
    int mkind;
    int passcount;
    int maxn;
    double valerr;
    double vecerr;
    bool wnsorted;
    int failc;
    int runs;
    double failr;
    double failthreshold;
    double threshold;
    bool waserrors;
    bool wfailed;
    int m;
    ap::real_2d_array z;

    failthreshold = 0.005;
    threshold = 1000*ap::machineepsilon;
    valerr = 0;
    vecerr = 0;
    wnsorted = false;
    wfailed = false;
    failc = 0;
    runs = 0;
    maxn = 10;
    passcount = 3;
    
    //
    // Main cycle
    //
    for(n = 1; n <= maxn; n++)
    {
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Prepare
            //
            a.setbounds(0, n-1, 0, n-1);
            al.setbounds(0, n-1, 0, n-1);
            au.setbounds(0, n-1, 0, n-1);
            for(i = 0; i <= n-1; i++)
            {
                for(j = i+1; j <= n-1; j++)
                {
                    
                    //
                    // A
                    //
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                    a(j,i) = ap::conj(a(i,j));
                    
                    //
                    // A lower
                    //
                    al(i,j).x = 2*ap::randomreal()-1;
                    al(i,j).y = 2*ap::randomreal()-1;
                    al(j,i) = a(j,i);
                    
                    //
                    // A upper
                    //
                    au(i,j) = a(i,j);
                    au(j,i).x = 2*ap::randomreal()-1;
                    au(j,i).y = 2*ap::randomreal()-1;
                }
                a(i,i) = 2*ap::randomreal()-1;
                al(i,i) = a(i,i);
                au(i,i) = a(i,i);
            }
            testevdproblem(a, al, au, n, valerr, vecerr, wnsorted, failc);
            runs = runs+1;
        }
    }
    
    //
    // report
    //
    failr = double(failc)/double(runs);
    wfailed = ap::fp_greater(failr,failthreshold);
    waserrors = ap::fp_greater(valerr,threshold)||ap::fp_greater(vecerr,threshold)||wnsorted||wfailed;
    if( !silent )
    {
        printf("TESTING HERMITIAN BISECTION AND INVERSE ITERATION EVD\n");
        printf("EVD values error (different variants):   %5.3le\n",
            double(valerr));
        printf("EVD vectors error:                       %5.3le\n",
            double(vecerr));
        printf("Eigen values order:                      ");
        if( !wnsorted )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("Always successfully converged:           ");
        if( !wfailed )
        {
            printf("YES\n");
        }
        else
        {
            printf("NO\n");
            printf("Fail ratio:                              %5.3lf\n",
                double(failr));
        }
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


/*************************************************************************
Unsets 2D array.
*************************************************************************/
static void unset2d(ap::complex_2d_array& a)
{

    a.setbounds(0, 0, 0, 0);
    a(0,0) = 2*ap::randomreal()-1;
}


/*************************************************************************
Unsets 1D array.
*************************************************************************/
static void unset1d(ap::real_1d_array& a)
{

    a.setbounds(0, 0);
    a(0) = 2*ap::randomreal()-1;
}


/*************************************************************************
Reference EVD
*************************************************************************/
static bool refevd(ap::complex_2d_array a,
     int n,
     ap::real_1d_array& lambda,
     ap::complex_2d_array& z)
{
    bool result;
    ap::complex_2d_array z2;
    ap::complex_2d_array z1;
    ap::complex_1d_array tau;
    ap::real_1d_array d;
    ap::real_1d_array e;
    ap::real_1d_array d1;
    ap::real_1d_array e1;
    int i;
    int j;
    int k;
    double vr;
    ap::complex v;

    
    //
    // to tridiagonal
    //
    hmatrixtd(a, n, true, tau, d, e);
    hmatrixtdunpackq(a, n, true, tau, z2);
    
    //
    // TDEVD
    //
    z1.setbounds(1, n, 1, n);
    for(i = 1; i <= n; i++)
    {
        for(j = 1; j <= n; j++)
        {
            z1(i,j) = z2(i-1,j-1);
        }
    }
    d1.setbounds(1, n);
    for(i = 1; i <= n; i++)
    {
        d1(i) = d(i-1);
    }
    e1.setbounds(1, n);
    for(i = 2; i <= n; i++)
    {
        e1(i) = e(i-2);
    }
    result = hbitridiagonalqlieigenvaluesandvectors(d1, e1, n, z1);
    if( result )
    {
        
        //
        // copy
        //
        lambda.setbounds(0, n-1);
        for(i = 0; i <= n-1; i++)
        {
            lambda(i) = d1(i+1);
        }
        z.setbounds(0, n-1, 0, n-1);
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                z(i,j) = z1(i+1,j+1);
            }
        }
        
        //
        // Use Selection Sort to minimize swaps of eigenvectors
        //
        for(i = 0; i <= n-2; i++)
        {
            k = i;
            for(j = i+1; j <= n-1; j++)
            {
                if( ap::fp_less(lambda(j),lambda(k)) )
                {
                    k = j;
                }
            }
            if( k!=i )
            {
                vr = lambda(i);
                lambda(i) = lambda(k);
                lambda(k) = vr;
                for(j = 0; j <= n-1; j++)
                {
                    v = z(j,i);
                    z(j,i) = z(j,k);
                    z(j,k) = v;
                }
            }
        }
    }
    return result;
}


static bool hbitridiagonalqlieigenvaluesandvectors(ap::real_1d_array& d,
     ap::real_1d_array e,
     int n,
     ap::complex_2d_array& z)
{
    bool result;
    int m;
    int l;
    int iter;
    int i;
    int k;
    double s;
    double r;
    double p;
    double g;
    double f;
    ap::complex fc;
    double dd;
    double c;
    double b;

    result = true;
    if( n==1 )
    {
        return result;
    }
    for(i = 2; i <= n; i++)
    {
        e(i-1) = e(i);
    }
    e(n) = 0.0;
    for(l = 1; l <= n; l++)
    {
        iter = 0;
        do
        {
            for(m = l; m <= n-1; m++)
            {
                dd = fabs(d(m))+fabs(d(m+1));
                if( ap::fp_eq(fabs(e(m))+dd,dd) )
                {
                    break;
                }
            }
            if( m!=l )
            {
                if( iter==30 )
                {
                    result = false;
                    return result;
                }
                iter = iter+1;
                g = (d(l+1)-d(l))/(2*e(l));
                if( ap::fp_less(fabs(g),1) )
                {
                    r = sqrt(1+ap::sqr(g));
                }
                else
                {
                    r = fabs(g)*sqrt(1+ap::sqr(1/g));
                }
                if( ap::fp_less(g,0) )
                {
                    g = d(m)-d(l)+e(l)/(g-r);
                }
                else
                {
                    g = d(m)-d(l)+e(l)/(g+r);
                }
                s = 1;
                c = 1;
                p = 0;
                for(i = m-1; i >= l; i--)
                {
                    f = s*e(i);
                    b = c*e(i);
                    if( ap::fp_less(fabs(f),fabs(g)) )
                    {
                        r = fabs(g)*sqrt(1+ap::sqr(f/g));
                    }
                    else
                    {
                        r = fabs(f)*sqrt(1+ap::sqr(g/f));
                    }
                    e(i+1) = r;
                    if( ap::fp_eq(r,0) )
                    {
                        d(i+1) = d(i+1)-p;
                        e(m) = 0;
                        break;
                    }
                    s = f/r;
                    c = g/r;
                    g = d(i+1)-p;
                    r = (d(i)-g)*s+2.0*c*b;
                    p = s*r;
                    d(i+1) = g+p;
                    g = c*r-b;
                    for(k = 1; k <= n; k++)
                    {
                        fc = z(k,i+1);
                        z(k,i+1) = s*z(k,i)+c*fc;
                        z(k,i) = c*z(k,i)-s*fc;
                    }
                }
                if( ap::fp_eq(r,0)&&i>=1 )
                {
                    continue;
                }
                d(l) = d(l)-p;
                e(l) = g;
                e(m) = 0.0;
            }
        }
        while(m!=l);
    }
    return result;
}


/*************************************************************************
Tests Z*Lambda*Z' against tridiag(D,E).
Returns relative error.
*************************************************************************/
static double testproduct(const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     const ap::real_2d_array& z,
     const ap::real_1d_array& lambda)
{
    double result;
    int i;
    int j;
    int k;
    double v;
    double mx;

    result = 0;
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            
            //
            // Calculate V = A[i,j], A = Z*Lambda*Z'
            //
            v = 0;
            for(k = 0; k <= n-1; k++)
            {
                v = v+z(i,k)*lambda(k)*z(j,k);
            }
            
            //
            // Compare
            //
            if( abs(i-j)==0 )
            {
                result = ap::maxreal(result, fabs(v-d(i)));
            }
            if( abs(i-j)==1 )
            {
                result = ap::maxreal(result, fabs(v-e(ap::minint(i, j))));
            }
            if( abs(i-j)>1 )
            {
                result = ap::maxreal(result, fabs(v));
            }
        }
    }
    mx = 0;
    for(i = 0; i <= n-1; i++)
    {
        mx = ap::maxreal(mx, fabs(d(i)));
    }
    for(i = 0; i <= n-2; i++)
    {
        mx = ap::maxreal(mx, fabs(e(i)));
    }
    if( ap::fp_eq(mx,0) )
    {
        mx = 1;
    }
    result = result/mx;
    return result;
}


/*************************************************************************
Tests Z*Z' against diag(1...1)
Returns absolute error.
*************************************************************************/
static double testort(const ap::real_2d_array& z, int n)
{
    double result;
    int i;
    int j;
    double v;

    result = 0;
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(z.getcolumn(i, 0, n-1), z.getcolumn(j, 0, n-1));
            if( i==j )
            {
                v = v-1;
            }
            result = ap::maxreal(result, fabs(v));
        }
    }
    return result;
}


/*************************************************************************
Tests EVD problem
*************************************************************************/
static void testevdproblem(const ap::complex_2d_array& afull,
     const ap::complex_2d_array& al,
     const ap::complex_2d_array& au,
     int n,
     double& valerr,
     double& vecerr,
     bool& wnsorted,
     int& failc)
{
    ap::real_1d_array lambda;
    ap::real_1d_array lambdaref;
    ap::complex_2d_array z;
    ap::complex_2d_array zref;
    bool wsucc;
    int i;
    int j;
    int k;
    int m;
    int i1;
    int i2;
    ap::complex v;
    double a;
    double b;
    int i_;

    lambdaref.setbounds(0, n-1);
    zref.setbounds(0, n-1, 0, n-1);
    
    //
    // Reference EVD
    //
    if( !refevd(afull, n, lambdaref, zref) )
    {
        failc = failc+1;
        return;
    }
    
    //
    // Test different combinations
    //
    for(i1 = 0; i1 <= n-1; i1++)
    {
        for(i2 = i1; i2 <= n-1; i2++)
        {
            
            //
            // Select A, B
            //
            if( i1>0 )
            {
                a = 0.5*(lambdaref(i1)+lambdaref(i1-1));
            }
            else
            {
                a = lambdaref(0)-1;
            }
            if( i2<n-1 )
            {
                b = 0.5*(lambdaref(i2)+lambdaref(i2+1));
            }
            else
            {
                b = lambdaref(n-1)+1;
            }
            
            //
            // Test interval, no vectors, lower A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdr(al, n, 0, false, a, b, m, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            if( m!=i2-i1+1 )
            {
                failc = failc+1;
                return;
            }
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            
            //
            // Test interval, no vectors, upper A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdr(au, n, 0, true, a, b, m, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            if( m!=i2-i1+1 )
            {
                failc = failc+1;
                return;
            }
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            
            //
            // Test indexes, no vectors, lower A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdi(al, n, 0, false, i1, i2, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            m = i2-i1+1;
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            
            //
            // Test indexes, no vectors, upper A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdi(au, n, 0, true, i1, i2, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            m = i2-i1+1;
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            
            //
            // Test interval, do not transform vectors, lower A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdr(al, n, 1, false, a, b, m, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            if( m!=i2-i1+1 )
            {
                failc = failc+1;
                return;
            }
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            for(j = 0; j <= m-1; j++)
            {
                v = 0.0;
                for(i_=0; i_<=n-1;i_++)
                {
                    v += z(i_,j)*ap::conj(zref(i_,i1+j));
                }
                v = 1/v;
                for(i_=0; i_<=n-1;i_++)
                {
                    z(i_,j) = v*z(i_,j);
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= m-1; j++)
                {
                    vecerr = ap::maxreal(vecerr, ap::abscomplex(z(i,j)-zref(i,i1+j)));
                }
            }
            
            //
            // Test interval, do not transform vectors, upper A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdr(au, n, 1, true, a, b, m, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            if( m!=i2-i1+1 )
            {
                failc = failc+1;
                return;
            }
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            for(j = 0; j <= m-1; j++)
            {
                v = 0.0;
                for(i_=0; i_<=n-1;i_++)
                {
                    v += z(i_,j)*ap::conj(zref(i_,i1+j));
                }
                v = 1/v;
                for(i_=0; i_<=n-1;i_++)
                {
                    z(i_,j) = v*z(i_,j);
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= m-1; j++)
                {
                    vecerr = ap::maxreal(vecerr, ap::abscomplex(z(i,j)-zref(i,i1+j)));
                }
            }
            
            //
            // Test indexes, do not transform vectors, lower A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdi(al, n, 1, false, i1, i2, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            m = i2-i1+1;
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            for(j = 0; j <= m-1; j++)
            {
                v = 0.0;
                for(i_=0; i_<=n-1;i_++)
                {
                    v += z(i_,j)*ap::conj(zref(i_,i1+j));
                }
                v = 1/v;
                for(i_=0; i_<=n-1;i_++)
                {
                    z(i_,j) = v*z(i_,j);
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= m-1; j++)
                {
                    vecerr = ap::maxreal(vecerr, ap::abscomplex(z(i,j)-zref(i,i1+j)));
                }
            }
            
            //
            // Test indexes, do not transform vectors, upper A
            //
            unset1d(lambda);
            unset2d(z);
            if( !hmatrixevdi(au, n, 1, true, i1, i2, lambda, z) )
            {
                failc = failc+1;
                return;
            }
            m = i2-i1+1;
            for(k = 0; k <= m-1; k++)
            {
                valerr = ap::maxreal(valerr, fabs(lambda(k)-lambdaref(i1+k)));
            }
            for(j = 0; j <= m-1; j++)
            {
                v = 0.0;
                for(i_=0; i_<=n-1;i_++)
                {
                    v += z(i_,j)*ap::conj(zref(i_,i1+j));
                }
                v = 1/v;
                for(i_=0; i_<=n-1;i_++)
                {
                    z(i_,j) = v*z(i_,j);
                }
            }
            for(i = 0; i <= n-1; i++)
            {
                for(j = 0; j <= m-1; j++)
                {
                    vecerr = ap::maxreal(vecerr, ap::abscomplex(z(i,j)-zref(i,i1+j)));
                }
            }
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testhevdbiunit_test_silent()
{
    bool result;

    result = testhevdbi(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testhevdbiunit_test()
{
    bool result;

    result = testhevdbi(false);
    return result;
}




