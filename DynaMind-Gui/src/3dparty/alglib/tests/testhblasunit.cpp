
#include <stdafx.h>
#include <stdio.h>
#include "testhblasunit.h"

bool testhblas(bool silent)
{
    bool result;
    ap::complex_2d_array a;
    ap::complex_2d_array ua;
    ap::complex_2d_array la;
    ap::complex_1d_array x;
    ap::complex_1d_array y1;
    ap::complex_1d_array y2;
    ap::complex_1d_array y3;
    int n;
    int maxn;
    int i;
    int j;
    int i1;
    int i2;
    int gpass;
    bool waserrors;
    double mverr;
    double threshold;
    ap::complex alpha;
    ap::complex v;
    int i_;
    int i1_;

    mverr = 0;
    waserrors = false;
    maxn = 10;
    threshold = 1000*ap::machineepsilon;
    
    //
    // Test MV
    //
    for(n = 2; n <= maxn; n++)
    {
        a.setbounds(1, n, 1, n);
        ua.setbounds(1, n, 1, n);
        la.setbounds(1, n, 1, n);
        x.setbounds(1, n);
        y1.setbounds(1, n);
        y2.setbounds(1, n);
        y3.setbounds(1, n);
        
        //
        // fill A, UA, LA
        //
        for(i = 1; i <= n; i++)
        {
            a(i,i).x = 2*ap::randomreal()-1;
            a(i,i).y = 0;
            for(j = i+1; j <= n; j++)
            {
                a(i,j).x = 2*ap::randomreal()-1;
                a(i,j).y = 2*ap::randomreal()-1;
                a(j,i) = ap::conj(a(i,j));
            }
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            {
                ua(i,j) = 0;
            }
        }
        for(i = 1; i <= n; i++)
        {
            for(j = i; j <= n; j++)
            {
                ua(i,j) = a(i,j);
            }
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            {
                la(i,j) = 0;
            }
        }
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= i; j++)
            {
                la(i,j) = a(i,j);
            }
        }
        
        //
        // test on different I1, I2
        //
        for(i1 = 1; i1 <= n; i1++)
        {
            for(i2 = i1; i2 <= n; i2++)
            {
                
                //
                // Fill X, choose Alpha
                //
                for(i = 1; i <= i2-i1+1; i++)
                {
                    x(i).x = 2*ap::randomreal()-1;
                    x(i).y = 2*ap::randomreal()-1;
                }
                alpha.x = 2*ap::randomreal()-1;
                alpha.y = 2*ap::randomreal()-1;
                
                //
                // calculate A*x, UA*x, LA*x
                //
                for(i = i1; i <= i2; i++)
                {
                    i1_ = (1)-(i1);
                    v = 0.0;
                    for(i_=i1; i_<=i2;i_++)
                    {
                        v += a(i,i_)*x(i_+i1_);
                    }
                    y1(i-i1+1) = alpha*v;
                }
                hermitianmatrixvectormultiply(ua, true, i1, i2, x, alpha, y2);
                hermitianmatrixvectormultiply(la, false, i1, i2, x, alpha, y3);
                
                //
                // Calculate error
                //
                for(i_=1; i_<=i2-i1+1;i_++)
                {
                    y2(i_) = y2(i_) - y1(i_);
                }
                v = 0.0;
                for(i_=1; i_<=i2-i1+1;i_++)
                {
                    v += y2(i_)*ap::conj(y2(i_));
                }
                mverr = ap::maxreal(mverr, sqrt(ap::abscomplex(v)));
                for(i_=1; i_<=i2-i1+1;i_++)
                {
                    y3(i_) = y3(i_) - y1(i_);
                }
                v = 0.0;
                for(i_=1; i_<=i2-i1+1;i_++)
                {
                    v += y3(i_)*ap::conj(y3(i_));
                }
                mverr = ap::maxreal(mverr, sqrt(ap::abscomplex(v)));
            }
        }
    }
    
    //
    // report
    //
    waserrors = ap::fp_greater(mverr,threshold);
    if( !silent )
    {
        printf("TESTING HERMITIAN BLAS\n");
        printf("MV error:                                %5.3le\n",
            double(mverr));
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
Silent unit test
*************************************************************************/
bool testhblasunit_test_silent()
{
    bool result;

    result = testhblas(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testhblasunit_test()
{
    bool result;

    result = testhblas(false);
    return result;
}




