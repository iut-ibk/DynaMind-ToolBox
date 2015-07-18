
#include <stdafx.h>
#include <stdio.h>
#include "testhtridiagonalunit.h"

static void testhtdproblem(const ap::complex_2d_array& a,
     int n,
     double& materr,
     double& orterr);

bool testhtridiagonal(bool silent)
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
    ap::complex_2d_array a;
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
        testhtdproblem(a, n, materr, orterr);
        
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
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                    a(j,i) = ap::conj(a(i,j));
                }
            }
            testhtdproblem(a, n, materr, orterr);
            
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
            testhtdproblem(a, n, materr, orterr);
            
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
                    a(i,j).x = 2*ap::randomreal()-1;
                    a(i,j).y = 2*ap::randomreal()-1;
                    a(j,i) = ap::conj(a(i,j));
                }
            }
            testhtdproblem(a, n, materr, orterr);
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(materr,threshold)||ap::fp_greater(orterr,threshold);
    if( !silent )
    {
        printf("TESTING HERMITIAN TO TRIDIAGONAL\n");
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


static void testhtdproblem(const ap::complex_2d_array& a,
     int n,
     double& materr,
     double& orterr)
{
    int i;
    int j;
    ap::complex_2d_array ua;
    ap::complex_2d_array la;
    ap::complex_2d_array t;
    ap::complex_2d_array q;
    ap::complex_2d_array t2;
    ap::complex_2d_array t3;
    ap::complex_1d_array tau;
    ap::real_1d_array d;
    ap::real_1d_array e;
    ap::complex v;
    int i_;

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
    hmatrixtd(ua, n, true, tau, d, e);
    hmatrixtdunpackq(ua, n, true, tau, q);
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
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += ap::conj(q(i_,i))*a(i_,j);
            }
            t2(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += t2(i,i_)*q(i_,j);
            }
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
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += q(i,i_)*ap::conj(q(j,i_));
            }
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
    hmatrixtd(la, n, false, tau, d, e);
    hmatrixtdunpackq(la, n, false, tau, q);
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
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += ap::conj(q(i_,i))*a(i_,j);
            }
            t2(i,j) = v;
        }
    }
    for(i = 0; i <= n-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += t2(i,i_)*q(i_,j);
            }
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
            v = 0.0;
            for(i_=0; i_<=n-1;i_++)
            {
                v += q(i,i_)*ap::conj(q(j,i_));
            }
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
bool testhtridiagonalunit_test_silent()
{
    bool result;

    result = testhtridiagonal(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testhtridiagonalunit_test()
{
    bool result;

    result = testhtridiagonal(false);
    return result;
}




