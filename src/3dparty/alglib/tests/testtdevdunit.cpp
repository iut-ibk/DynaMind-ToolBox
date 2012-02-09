
#include <stdafx.h>
#include <stdio.h>
#include "testtdevdunit.h"

static void fillde(ap::real_1d_array& d,
     ap::real_1d_array& e,
     int n,
     int filltype);
static double testproduct(const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     const ap::real_2d_array& z,
     const ap::real_1d_array& lambda);
static double testort(const ap::real_2d_array& z, int n);
static void testevdproblem(const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     double& materr,
     double& valerr,
     double& orterr,
     bool& wnsorted,
     int& failc);

/*************************************************************************
Testing bidiagonal SVD decomposition subroutine
*************************************************************************/
bool testtdevd(bool silent)
{
    bool result;
    ap::real_1d_array d;
    ap::real_1d_array e;
    int pass;
    int n;
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
        d.setbounds(0, n-1);
        if( n>1 )
        {
            e.setbounds(0, n-2);
        }
        
        //
        // Different tasks
        //
        for(mkind = 0; mkind <= 4; mkind++)
        {
            fillde(d, e, n, mkind);
            testevdproblem(d, e, n, materr, valerr, orterr, wnsorted, failc);
            runs = runs+1;
        }
    }
    
    //
    // report
    //
    failr = double(failc)/double(runs);
    wfailed = ap::fp_greater(failr,failthreshold);
    waserrors = ap::fp_greater(materr,threshold)||ap::fp_greater(valerr,threshold)||ap::fp_greater(orterr,threshold)||wnsorted||wfailed;
    if( !silent )
    {
        printf("TESTING TRIDIAGONAL EVD\n");
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
Fills D and E
*************************************************************************/
static void fillde(ap::real_1d_array& d,
     ap::real_1d_array& e,
     int n,
     int filltype)
{
    int i;
    int j;

    if( filltype==0 )
    {
        
        //
        // Zero matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            d(i) = 0;
        }
        for(i = 0; i <= n-2; i++)
        {
            e(i) = 0;
        }
        return;
    }
    if( filltype==1 )
    {
        
        //
        // Diagonal matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            d(i) = 2*ap::randomreal()-1;
        }
        for(i = 0; i <= n-2; i++)
        {
            e(i) = 0;
        }
        return;
    }
    if( filltype==2 )
    {
        
        //
        // Off-diagonal matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            d(i) = 0;
        }
        for(i = 0; i <= n-2; i++)
        {
            e(i) = 2*ap::randomreal()-1;
        }
        return;
    }
    if( filltype==3 )
    {
        
        //
        // Dense matrix with blocks
        //
        for(i = 0; i <= n-1; i++)
        {
            d(i) = 2*ap::randomreal()-1;
        }
        for(i = 0; i <= n-2; i++)
        {
            e(i) = 2*ap::randomreal()-1;
        }
        j = 1;
        i = 2;
        while(j<=n-2)
        {
            e(j) = 0;
            j = j+i;
            i = i+1;
        }
        return;
    }
    
    //
    // dense matrix
    //
    for(i = 0; i <= n-1; i++)
    {
        d(i) = 2*ap::randomreal()-1;
    }
    for(i = 0; i <= n-2; i++)
    {
        e(i) = 2*ap::randomreal()-1;
    }
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
static void testevdproblem(const ap::real_1d_array& d,
     const ap::real_1d_array& e,
     int n,
     double& materr,
     double& valerr,
     double& orterr,
     bool& wnsorted,
     int& failc)
{
    ap::real_1d_array lambda;
    ap::real_1d_array ee;
    ap::real_1d_array lambda2;
    ap::real_2d_array z;
    ap::real_2d_array zref;
    ap::real_2d_array a1;
    ap::real_2d_array a2;
    bool wsucc;
    int i;
    int j;
    double v;

    lambda.setbounds(0, n-1);
    lambda2.setbounds(0, n-1);
    zref.setbounds(0, n-1, 0, n-1);
    a1.setbounds(0, n-1, 0, n-1);
    a2.setbounds(0, n-1, 0, n-1);
    if( n>1 )
    {
        ee.setbounds(0, n-2);
    }
    
    //
    // Test simple EVD: values and full vectors
    //
    for(i = 0; i <= n-1; i++)
    {
        lambda(i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        ee(i) = e(i);
    }
    wsucc = smatrixtdevd(lambda, ee, n, 2, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    materr = ap::maxreal(materr, testproduct(d, e, n, z, lambda));
    orterr = ap::maxreal(orterr, testort(z, n));
    for(i = 0; i <= n-2; i++)
    {
        if( ap::fp_less(lambda(i+1),lambda(i)) )
        {
            wnsorted = true;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            zref(i,j) = z(i,j);
        }
    }
    
    //
    // Test values only variant
    //
    for(i = 0; i <= n-1; i++)
    {
        lambda2(i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        ee(i) = e(i);
    }
    wsucc = smatrixtdevd(lambda2, ee, n, 0, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        valerr = ap::maxreal(valerr, fabs(lambda2(i)-lambda(i)));
    }
    
    //
    // Test multiplication variant
    //
    for(i = 0; i <= n-1; i++)
    {
        lambda2(i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        ee(i) = e(i);
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            a1(i,j) = 2*ap::randomreal()-1;
            a2(i,j) = a1(i,j);
        }
    }
    wsucc = smatrixtdevd(lambda2, ee, n, 1, a1);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        valerr = ap::maxreal(valerr, fabs(lambda2(i)-lambda(i)));
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(a2.getrow(i, 0, n-1), zref.getcolumn(j, 0, n-1));
            materr = ap::maxreal(materr, fabs(v-a1(i,j)));
        }
    }
    
    //
    // Test first row variant
    //
    for(i = 0; i <= n-1; i++)
    {
        lambda2(i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        ee(i) = e(i);
    }
    wsucc = smatrixtdevd(lambda2, ee, n, 3, z);
    if( !wsucc )
    {
        failc = failc+1;
        return;
    }
    for(i = 0; i <= n-1; i++)
    {
        valerr = ap::maxreal(valerr, fabs(lambda2(i)-lambda(i)));
        materr = ap::maxreal(materr, fabs(z(0,i)-zref(0,i)));
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testtdevdunit_test_silent()
{
    bool result;

    result = testtdevd(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testtdevdunit_test()
{
    bool result;

    result = testtdevd(false);
    return result;
}




