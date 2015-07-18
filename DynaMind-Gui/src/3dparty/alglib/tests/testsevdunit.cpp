
#include <stdafx.h>
#include <stdio.h>
#include "testsevdunit.h"

static void unset2d(ap::real_2d_array& a);
static void unset1d(ap::real_1d_array& a);
static double testproduct(const ap::real_2d_array& a,
     int n,
     const ap::real_2d_array& z,
     const ap::real_1d_array& lambda);
static double testort(const ap::real_2d_array& z, int n);
static void testevdproblem(const ap::real_2d_array& a,
     const ap::real_2d_array& al,
     const ap::real_2d_array& au,
     int n,
     double& materr,
     double& valerr,
     double& orterr,
     bool& wnsorted,
     int& failc);

/*************************************************************************
Testing symmetric EVD subroutine
*************************************************************************/
bool testsevd(bool silent)
{
    bool result;
    ap::real_2d_array a;
    ap::real_2d_array al;
    ap::real_2d_array au;
    ap::real_2d_array z;
    int pass;
    int n;
    int i;
    int j;
    int mkind;
    int passcount;
    int maxn;
    double materr;
    double valerr;
    double orterr;
    bool wnsorted;
    int failc;
    int runs;
    double failr;
    double failthreshold;
    double threshold;
    bool waserrors;
    bool wfailed;

    failthreshold = 0.005;
    threshold = 1000*ap::machineepsilon;
    materr = 0;
    valerr = 0;
    orterr = 0;
    wnsorted = false;
    wfailed = false;
    failc = 0;
    runs = 0;
    maxn = 20;
    passcount = 10;
    
    //
    // Main cycle
    //
    for(n = 1; n <= maxn; n++)
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
                a(i,j) = 2*ap::randomreal()-1;
                a(j,i) = a(i,j);
                
                //
                // A lower
                //
                al(i,j) = 2*ap::randomreal()-1;
                al(j,i) = a(i,j);
                
                //
                // A upper
                //
                au(i,j) = a(i,j);
                au(j,i) = 2*ap::randomreal()-1;
            }
            a(i,i) = 2*ap::randomreal()-1;
            al(i,i) = a(i,i);
            au(i,i) = a(i,i);
        }
        
        //
        // Test
        //
        testevdproblem(a, al, au, n, materr, valerr, orterr, wnsorted, failc);
        runs = runs+1;
    }
    
    //
    // report
    //
    failr = double(failc)/double(runs);
    wfailed = ap::fp_greater(failr,failthreshold);
    waserrors = ap::fp_greater(materr,threshold)||ap::fp_greater(valerr,threshold)||ap::fp_greater(orterr,threshold)||wnsorted||wfailed;
    if( !silent )
    {
        printf("TESTING SYMMETRIC EVD\n");
        printf("EVD matrix error:                        %5.3le\n",
            double(materr));
        printf("EVD values error (different variants):   %5.3le\n",
            double(valerr));
        printf("EVD orthogonality error:                 %5.3le\n",
            double(orterr));
        printf("Eigen values order:                      ");
        if( !wnsorted )
        {
            printf("OK\n");
        }
        else
        {
            printf("FAILED\n");
        }
        printf("Always converged:                        ");
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
static void unset2d(ap::real_2d_array& a)
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
Tests Z*Lambda*Z' against tridiag(D,E).
Returns relative error.
*************************************************************************/
static double testproduct(const ap::real_2d_array& a,
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
            result = ap::maxreal(result, fabs(v-a(i,j)));
        }
    }
    mx = 0;
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            mx = ap::maxreal(mx, fabs(a(i,j)));
        }
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
static void testevdproblem(const ap::real_2d_array& a,
     const ap::real_2d_array& al,
     const ap::real_2d_array& au,
     int n,
     double& materr,
     double& valerr,
     double& orterr,
     bool& wnsorted,
     int& failc)
{
    ap::real_1d_array lambda;
    ap::real_1d_array lambdaref;
    ap::real_2d_array z;
    bool wsucc;
    int i;
    int j;
    double v;

    
    //
    // Test simple EVD: values and full vectors, lower A
    //
    unset1d(lambdaref);
    unset2d(z);
    wsucc = smatrixevd(al, n, 1, false, lambdaref, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    materr = ap::maxreal(materr, testproduct(a, n, z, lambdaref));
    orterr = ap::maxreal(orterr, testort(z, n));
    for(i = 0; i <= n-2; i++)
    {
        if( ap::fp_less(lambdaref(i+1),lambdaref(i)) )
        {
            wnsorted = true;
        }
    }
    
    //
    // Test simple EVD: values and full vectors, upper A
    //
    unset1d(lambda);
    unset2d(z);
    wsucc = smatrixevd(au, n, 1, true, lambda, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    materr = ap::maxreal(materr, testproduct(a, n, z, lambda));
    orterr = ap::maxreal(orterr, testort(z, n));
    for(i = 0; i <= n-2; i++)
    {
        if( ap::fp_less(lambda(i+1),lambda(i)) )
        {
            wnsorted = true;
        }
    }
    
    //
    // Test simple EVD: values only, lower A
    //
    unset1d(lambda);
    unset2d(z);
    wsucc = smatrixevd(al, n, 0, false, lambda, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        valerr = ap::maxreal(valerr, fabs(lambda(i)-lambdaref(i)));
    }
    
    //
    // Test simple EVD: values only, upper A
    //
    unset1d(lambda);
    unset2d(z);
    wsucc = smatrixevd(au, n, 0, true, lambda, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        valerr = ap::maxreal(valerr, fabs(lambda(i)-lambdaref(i)));
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testsevdunit_test_silent()
{
    bool result;

    result = testsevd(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testsevdunit_test()
{
    bool result;

    result = testsevd(false);
    return result;
}




