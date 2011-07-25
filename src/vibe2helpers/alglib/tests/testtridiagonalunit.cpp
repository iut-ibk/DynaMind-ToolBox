
#include <stdafx.h>
#include <stdio.h>
#include "testtridiagonalunit.h"

static void teststdproblem(const ap::real_2d_array& a,
     int n,
     double& materr,
     double& orterr);

bool testtridiagonal(bool silent)
{
    bool result;
    int pass;
    int passcount;
    int maxn;
    double materr;
    double orterr;
    int n;
    int i;
    int j;
    int k;
    ap::real_2d_array a;
    double threshold;
    bool waserrors;

    materr = 0;
    orterr = 0;
    waserrors = false;
    threshold = 1000*ap::machineepsilon;
    maxn = 15;
    passcount = 20;
    for(n = 1; n <= maxn; n++)
    {
        a.setbounds(0, n-1, 0, n-1);
        
        //
        // Test zero matrix
        //
        for(i = 0; i <= n-1; i++)
        {
            for(j = 0; j <= n-1; j++)
            {
                a(i,j) = 0;
            }
        }
        teststdproblem(a, n, materr, orterr);
        
        //
        // Test other matrix types
        //
        for(pass = 1; pass <= passcount; pass++)
        {
            
            //
            // Test dense matrix
            //
            for(i = 0; i <= n-1; i++)
            {
                a(i,i) = 2*ap::randomreal()-1;
                for(j = i+1; j <= n-1; j++)
                {
                    a(i,j) = 2*ap::randomreal()-1;
                    a(j,i) = a(i,j);
                }
            }
            teststdproblem(a, n, materr, orterr);
            
            //
            // Diagonal matrix
            //
            for(i = 0; i <= n-1; i++)
            {
                a(i,i) = 2*ap::randomreal()-1;
                for(j = i+1; j <= n-1; j++)
                {
                    a(i,j) = 0;
                    a(j,i) = 0;
                }
            }
            teststdproblem(a, n, materr, orterr);
            
            //
            // sparse matrix
            //
            for(i = 0; i <= n-1; i++)
            {
                a(i,i) = 0;
                for(j = i+1; j <= n-1; j++)
                {
                    a(i,j) = 0;
                    a(j,i) = 0;
                }
            }
            for(k = 1; k <= 2; k++)
            {
                i = ap::randominteger(n);
                j = ap::randominteger(n);
                if( i==j )
                {
                    a(i,j) = 2*ap::randomreal()-1;
                }
                else
                {
                    a(i,j) = 2*ap::randomreal()-1;
                    a(j,i) = a(i,j);
                }
            }
            teststdproblem(a, n, materr, orterr);
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(materr,threshold)||ap::fp_greater(orterr,threshold);
    if( !silent )
    {
        printf("TESTING SYMMETRIC TO TRIDIAGONAL\n");
        printf("Matrix error:                            %5.3le\n",
            double(materr));
        printf("Q orthogonality error:                   %5.3le\n",
            double(orterr));
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


static void teststdproblem(const ap::real_2d_array& a,
     int n,
     double& materr,
     double& orterr)
{
    int i;
    int j;
    ap::real_2d_array ua;
    ap::real_2d_array la;
    ap::real_2d_array t;
    ap::real_2d_array q;
    ap::real_2d_array t2;
    ap::real_2d_array t3;
    ap::real_1d_array tau;
    ap::real_1d_array d;
    ap::real_1d_array e;
    double v;

    ua.setbounds(0, n-1, 0, n-1);
    la.setbounds(0, n-1, 0, n-1);
    t.setbounds(0, n-1, 0, n-1);
    q.setbounds(0, n-1, 0, n-1);
    t2.setbounds(0, n-1, 0, n-1);
    t3.setbounds(0, n-1, 0, n-1);
    
    //
    // fill
    //
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            ua(i,j) = 0;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = i; j <= n-1; j++)
        {
            ua(i,j) = a(i,j);
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            la(i,j) = 0;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= i; j++)
        {
            la(i,j) = a(i,j);
        }
    }
    
    //
    // Test 2tridiagonal: upper
    //
    smatrixtd(ua, n, true, tau, d, e);
    smatrixtdunpackq(ua, n, true, tau, q);
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            t(i,j) = 0;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        t(i,i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        t(i,i+1) = e(i);
        t(i+1,i) = e(i);
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(q.getcolumn(i, 0, n-1), a.getcolumn(j, 0, n-1));
            t2(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(t2.getrow(i, 0, n-1), q.getcolumn(j, 0, n-1));
            t3(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            materr = ap::maxreal(materr, ap::abscomplex(t3(i,j)-t(i,j)));
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(&q(i, 0), &q(j, 0), ap::vlen(0,n-1));
            if( i==j )
            {
                orterr = ap::maxreal(orterr, ap::abscomplex(v-1));
            }
            else
            {
                orterr = ap::maxreal(orterr, ap::abscomplex(v));
            }
        }
    }
    
    //
    // Test 2tridiagonal: lower
    //
    smatrixtd(la, n, false, tau, d, e);
    smatrixtdunpackq(la, n, false, tau, q);
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            t(i,j) = 0;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        t(i,i) = d(i);
    }
    for(i = 0; i <= n-2; i++)
    {
        t(i,i+1) = e(i);
        t(i+1,i) = e(i);
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(q.getcolumn(i, 0, n-1), a.getcolumn(j, 0, n-1));
            t2(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(t2.getrow(i, 0, n-1), q.getcolumn(j, 0, n-1));
            t3(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            materr = ap::maxreal(materr, ap::abscomplex(t3(i,j)-t(i,j)));
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = ap::vdotproduct(&q(i, 0), &q(j, 0), ap::vlen(0,n-1));
            if( i==j )
            {
                orterr = ap::maxreal(orterr, ap::abscomplex(v-1));
            }
            else
            {
                orterr = ap::maxreal(orterr, ap::abscomplex(v));
            }
        }
    }
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testtridiagonalunit_test_silent()
{
    bool result;

    result = testtridiagonal(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testtridiagonalunit_test()
{
    bool result;

    result = testtridiagonal(false);
    return result;
}




